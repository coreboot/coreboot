/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <bootstate.h>
#include <console/console.h>
#include <amdblocks/psp.h>
#include <soc/iomap.h>
#include "psp_def.h"

static const char *psp_status_nobase = "error: PSP_ADDR_MSR and PSP BAR3 not assigned";
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

static u32 rd_resp_sts(struct mbox_buffer_header *header)
{
	return read32(&header->status);
}

/*
 * Print meaningful status to the console.  Caller only passes a pointer to a
 * buffer header if it's expected to contain its own status.
 */
void psp_print_cmd_status(int cmd_status, struct mbox_buffer_header *header)
{
	if (header && rd_resp_sts(header))
		printk(BIOS_DEBUG, "buffer status=0x%x ", rd_resp_sts(header));

	if (cmd_status)
		printk(BIOS_WARNING, "%s\n", status_to_string(cmd_status));
	else
		printk(BIOS_DEBUG, "OK\n");
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
	psp_print_cmd_status(cmd_status, &buffer.header);
}

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY, psp_notify_boot_done, NULL);
