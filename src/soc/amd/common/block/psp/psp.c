/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012-2018 Advanced Micro Devices, Inc.
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

#include <arch/io.h>
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

static void psp_bar_init_early(void)
{
	u32 psp_mmio_size;
	u32 value32;
	u32 base, limit;

	/* Check for presence of the PSP */
	if (pci_read_config32(SOC_PSP_DEV, PCI_VENDOR_ID) == 0xffffffff) {
		printk(BIOS_WARNING, "PSP: SOC_PSP_DEV device not found at D%xF%x\n",
			PSP_DEV, PSP_FUNC);
		return;
	}

	/* Check if PSP BAR has been assigned, and if so, just return */
	if (pci_read_config32(SOC_PSP_DEV, PCI_BASE_ADDRESS_4) &
			~PCI_BASE_ADDRESS_MEM_ATTR_MASK)
		return;

	/* Otherwise, do an early init of the BAR */
	pci_write_config32(SOC_PSP_DEV, PCI_BASE_ADDRESS_4, 0xffffffff);
	psp_mmio_size = ~pci_read_config32(SOC_PSP_DEV, PCI_BASE_ADDRESS_4) + 1;
	printk(BIOS_SPEW, "PSP: BAR size is 0x%x\n", psp_mmio_size);
	/* Assign BAR to an initial temporarily defined region */
	pci_write_config32(SOC_PSP_DEV, PCI_BASE_ADDRESS_4,
			PSP_MAILBOX_BAR3_BASE);

	/* Route MMIO through the northbridge */
	pci_write_config32(SOC_PSP_DEV, PCI_COMMAND,
			(PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER));
	limit = ((PSP_MAILBOX_BAR3_BASE + psp_mmio_size - 1) >> 8) & ~0xff;
	pci_write_config32(SOC_ADDR_DEV, NB_MMIO_LIMIT_LO(7), limit);
	base = (PSP_MAILBOX_BAR3_BASE >> 8) | MMIO_WE | MMIO_RE;
	pci_write_config32(SOC_ADDR_DEV, NB_MMIO_BASE_LO(7), base);
	pci_write_config32(SOC_PSP_DEV, PSP_PCI_EXT_HDR_CTRL, MAGIC_ENABLES);

	/* Update the capability chain */
	value32 = pci_read_config32(SOC_PSP_DEV, PSP_PCI_MIRRORCTRL1_REG);
	value32 &= ~PMNXTPTRW_MASK;
	value32 |= PMNXTPTRW_EXPOSE;
	pci_write_config32(SOC_PSP_DEV, PSP_PCI_MIRRORCTRL1_REG, value32);
}

static uintptr_t get_psp_bar3_addr(void)
{
	uintptr_t psp_mmio;

	/* Check for presence of the PSP */
	if (pci_read_config32(SOC_PSP_DEV, PCI_VENDOR_ID) == 0xffffffff) {
		printk(BIOS_WARNING, "PSP: No SOC_PSP_DEV found at D%xF%x\n",
			PSP_DEV, PSP_FUNC);
		return 0;
	}

	/* D8F0x48[12] is the Bar3Hide flag, check it */
	if (pci_read_config32(SOC_PSP_DEV, PSP_PCI_EXT_HDR_CTRL) & BAR3HIDE) {
		psp_mmio = rdmsr(MSR_CU_CBBCFG).lo;
		if (psp_mmio == 0xffffffff) {
			printk(BIOS_WARNING, "PSP: BAR hidden, MSR val uninitialized\n");
			return 0;
		}
		return psp_mmio;
	} else {
		return pci_read_config32(SOC_PSP_DEV, PCI_BASE_ADDRESS_4) &
				~PCI_BASE_ADDRESS_MEM_ATTR_MASK;
	}
}

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

static struct psp_mbox *get_mbox_address(void)
{
	uintptr_t baseptr;

	baseptr = get_psp_bar3_addr();
	if (baseptr == 0) {
		psp_bar_init_early();
		baseptr = get_psp_bar3_addr();
		if (baseptr == 0) {
			printk(BIOS_WARNING, "PSP: %s(), psp_bar_init_early() failed...\n",
					__func__);
			return NULL;
		}
	}
	return (struct psp_mbox *)(baseptr + PSP_MAILBOX_BASE);
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
	struct psp_mbox *mbox = get_mbox_address();
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
	if (rd_resp_sts(&buffer))
		printk(BIOS_DEBUG, "buffer status=0x%x ",
				rd_resp_sts(&buffer));
	if (cmd_status)
		printk(BIOS_DEBUG, "%s\n", status_to_string(cmd_status));
	else
		printk(BIOS_DEBUG, "OK\n");

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
	if (rd_resp_sts(&buffer))
		printk(BIOS_DEBUG, "buffer status=0x%x ",
				rd_resp_sts(&buffer));
	if (cmd_status)
		printk(BIOS_DEBUG, "%s\n", status_to_string(cmd_status));
	else
		printk(BIOS_DEBUG, "OK\n");
}

/*
 * Tell the PSP to load a firmware blob from a location in the BIOS image.
 */
static int psp_load_blob(int type, void *addr)
{
	int cmd_status;

	if (!IS_ENABLED(CONFIG_SOC_AMD_PSP_SELECTABLE_SMU_FW)) {
		printk(BIOS_ERR, "BUG: Selectable firmware is not supported\n");
		return PSPSTS_UNSUPPORTED;
	}

	/* only two types currently supported */
	if (type != MBOX_BIOS_CMD_SMU_FW && type != MBOX_BIOS_CMD_SMU_FW2) {
		printk(BIOS_ERR, "BUG: Invalid PSP blob type %x\n", type);
		return PSPSTS_INVALID_BLOB;
	}

	printk(BIOS_DEBUG, "PSP: Load blob type %x from @%p... ", type, addr);

	/* Blob commands use the buffer registers as data, not pointer to buf */
	cmd_status = send_psp_command(type, addr);

	if (cmd_status)
		printk(BIOS_DEBUG, "%s\n", status_to_string(cmd_status));
	else
		printk(BIOS_DEBUG, "OK\n");

	return cmd_status;
}

int psp_load_named_blob(int type, const char *name)
{
	void *blob;
	struct cbfsf cbfs_file;
	struct region_device rdev;
	int r;

	if (cbfs_boot_locate(&cbfs_file, name, NULL)) {
		printk(BIOS_ERR, "BUG: Cannot locate blob for PSP loading\n");
		return PSPSTS_INVALID_NAME;
	}

	cbfs_file_data(&rdev, &cbfs_file);
	blob = rdev_mmap_full(&rdev);
	if (blob) {
		r = psp_load_blob(type, blob);
		rdev_munmap(&rdev, blob);
	} else {
		printk(BIOS_ERR, "BUG: Cannot map blob for PSP loading\n");
		return PSPSTS_INVALID_NAME;
	}
	return r;
}

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY,
		psp_notify_boot_done, NULL);
