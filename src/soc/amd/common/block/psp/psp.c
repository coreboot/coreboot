/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/mmio.h>
#include <cpu/x86/msr.h>
#include <cbfs.h>
#include <region_file.h>
#include <timer.h>
#include <device/pci_def.h>
#include <bootstate.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <amdblocks/psp.h>
#include <soc/iomap.h>
#include <soc/northbridge.h>

static const char *psp_status_nobase = "error: PSP BAR3 not assigned";
static const char *psp_status_halted = "error: PSP in halted state";
static const char *psp_status_recovery = "error: PSP recovery required";
static const char *psp_status_errcmd = "error sending command";
static const char *psp_status_init_timeout = "error: PSP init timeout";
static const char *psp_status_cmd_timeout = "error: PSP command timeout";
static const char *psp_status_noerror = "";

static const char *status_to_string(int err)
{
	switch (err) {
	case -PSPSTS_NOBASE:
		return psp_status_nobase;
	case -PSPSTS_HALTED:
		return psp_status_halted;
	case -PSPSTS_RECOVERY:
		return psp_status_recovery;
	case -PSPSTS_SEND_ERROR:
		return psp_status_errcmd;
	case -PSPSTS_INIT_TIMEOUT:
		return psp_status_init_timeout;
	case -PSPSTS_CMD_TIMEOUT:
		return psp_status_cmd_timeout;
	default:
		return psp_status_noerror;
	}
}

static u32 rd_mbox_sts(struct psp_mbox *mbox)
{
	return read32(&mbox->mbox_status);
}

static void wr_mbox_cmd(struct psp_mbox *mbox, u32 cmd)
{
	write32(&mbox->mbox_command, cmd);
}

static u32 rd_mbox_cmd(struct psp_mbox *mbox)
{
	return read32(&mbox->mbox_command);
}

static void wr_mbox_cmd_resp(struct psp_mbox *mbox, void *buffer)
{
	write64(&mbox->cmd_response, (uintptr_t)buffer);
}

static u32 rd_resp_sts(struct mbox_default_buffer *buffer)
{
	return read32(&buffer->header.status);
}

static int wait_initialized(struct psp_mbox *mbox)
{
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, PSP_INIT_TIMEOUT);

	do {
		if (rd_mbox_sts(mbox) & STATUS_INITIALIZED)
			return 0;
	} while (!stopwatch_expired(&sw));

	return -PSPSTS_INIT_TIMEOUT;
}

static int wait_command(struct psp_mbox *mbox)
{
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, PSP_CMD_TIMEOUT);

	do {
		if (!rd_mbox_cmd(mbox))
			return 0;
	} while (!stopwatch_expired(&sw));

	return -PSPSTS_CMD_TIMEOUT;
}

static int send_psp_command(u32 command, void *buffer)
{
	struct psp_mbox *mbox = soc_get_mbox_address();
	if (!mbox)
		return -PSPSTS_NOBASE;

	/* check for PSP error conditions */
	if (rd_mbox_sts(mbox) & STATUS_HALT)
		return -PSPSTS_HALTED;

	if (rd_mbox_sts(mbox) & STATUS_RECOVERY)
		return -PSPSTS_RECOVERY;

	/* PSP must be finished with init and ready to accept a command */
	if (wait_initialized(mbox))
		return -PSPSTS_INIT_TIMEOUT;

	if (wait_command(mbox))
		return -PSPSTS_CMD_TIMEOUT;

	/* set address of command-response buffer and write command register */
	wr_mbox_cmd_resp(mbox, buffer);
	wr_mbox_cmd(mbox, command);

	/* PSP clears command register when complete */
	if (wait_command(mbox))
		return -PSPSTS_CMD_TIMEOUT;

	/* check delivery status */
	if (rd_mbox_sts(mbox) & (STATUS_ERROR | STATUS_TERMINATED))
		return -PSPSTS_SEND_ERROR;

	return 0;
}

/*
 * Print meaningful status to the console.  Caller only passes a pointer to a
 * buffer if it's expected to contain its own status.
 */
static void print_cmd_status(int cmd_status, struct mbox_default_buffer *buffer)
{
	if (buffer && rd_resp_sts(buffer))
		printk(BIOS_DEBUG, "buffer status=0x%x ", rd_resp_sts(buffer));

	if (cmd_status)
		printk(BIOS_DEBUG, "%s\n", status_to_string(cmd_status));
	else
		printk(BIOS_DEBUG, "OK\n");
}

/*
 * Notify the PSP that DRAM is present.  Upon receiving this command, the PSP
 * will load its OS into fenced DRAM that is not accessible to the x86 cores.
 */
int psp_notify_dram(void)
{
	int cmd_status;
	struct mbox_default_buffer buffer = {
		.header = {
			.size = sizeof(buffer)
		}
	};

	printk(BIOS_DEBUG, "PSP: Notify that DRAM is available... ");

	cmd_status = send_psp_command(MBOX_BIOS_CMD_DRAM_INFO, &buffer);

	/* buffer's status shouldn't change but report it if it does */
	print_cmd_status(cmd_status, &buffer);

	return cmd_status;
}

/*
 * Notify the PSP that the system is completing the boot process.  Upon
 * receiving this command, the PSP will only honor commands where the buffer
 * is in SMM space.
 */
static void psp_notify_boot_done(void *unused)
{
	int cmd_status;
	struct mbox_default_buffer buffer = {
		.header = {
			.size = sizeof(buffer)
		}
	};

	printk(BIOS_DEBUG, "PSP: Notify that POST is finishing... ");

	cmd_status = send_psp_command(MBOX_BIOS_CMD_BOOT_DONE, &buffer);

	/* buffer's status shouldn't change but report it if it does */
	print_cmd_status(cmd_status, &buffer);
}

/*
 * Tell the PSP to load a firmware blob from a location in the BIOS image.
 */
int psp_load_named_blob(enum psp_blob_type type, const char *name)
{
	int cmd_status;
	u32 command;
	void *blob;
	struct cbfsf cbfs_file;
	struct region_device rdev;

	switch (type) {
	case BLOB_SMU_FW:
		command = MBOX_BIOS_CMD_SMU_FW;
		break;
	case BLOB_SMU_FW2:
		command = MBOX_BIOS_CMD_SMU_FW2;
		break;
	default:
		printk(BIOS_ERR, "BUG: Invalid PSP blob type %x\n", type);
		return -PSPSTS_INVALID_BLOB;
	}

	/* type can only be BLOB_SMU_FW or BLOB_SMU_FW2 here, so don't re-check for this */
	if (!CONFIG(SOC_AMD_PSP_SELECTABLE_SMU_FW)) {
		printk(BIOS_ERR, "BUG: Selectable firmware is not supported\n");
		return -PSPSTS_UNSUPPORTED;
	}

	if (cbfs_boot_locate(&cbfs_file, name, NULL)) {
		printk(BIOS_ERR, "BUG: Cannot locate blob for PSP loading\n");
		return -PSPSTS_INVALID_NAME;
	}

	cbfs_file_data(&rdev, &cbfs_file);
	blob = rdev_mmap_full(&rdev);
	if (!blob) {
		printk(BIOS_ERR, "BUG: Cannot map blob for PSP loading\n");
		return -PSPSTS_INVALID_NAME;
	}

	printk(BIOS_DEBUG, "PSP: Load blob type %x from @%p... ", type, blob);

	/* Blob commands use the buffer registers as data, not pointer to buf */
	cmd_status = send_psp_command(command, blob);
	print_cmd_status(cmd_status, NULL);

	rdev_munmap(&rdev, blob);
	return cmd_status;
}

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY,
		psp_notify_boot_done, NULL);
