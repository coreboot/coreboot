/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Advanced Micro Devices, Inc.
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
#include <delay.h>
#include <cbmem.h>
#include <timer.h>
#include <device/pci_def.h>
#include <console/console.h>
#include <amdblocks/psp.h>

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

static struct psp_mbox *get_mbox_address(void)
{
	UINT32 base; /* UINT32 for compatibility with PspBaseLib */
	BOOLEAN bar3_status;
	uintptr_t baseptr;

	bar3_status = GetPspBar3Addr(&base);
	if (!bar3_status) {
		PspBarInitEarly();
		bar3_status = GetPspBar3Addr(&base);
	}
	if (!bar3_status)
		return NULL;

	baseptr = base;
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
	u32 command_reg;
	int status = 0;

	struct psp_mbox *mbox = get_mbox_address();
	if (!mbox)
		return -PSPSTS_NOBASE;

	command_reg = pci_read_config32(PSP_DEV, PCI_COMMAND);
	pci_write_config32(PSP_DEV, PCI_COMMAND, command_reg |
				PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER);

	/* check for PSP error conditions */
	if (rd_mbox_sts(mbox) & STATUS_HALT) {
		status = -PSPSTS_HALTED;
		goto exit;
	}
	if (rd_mbox_sts(mbox) & STATUS_RECOVERY) {
		status = -PSPSTS_RECOVERY;
		goto exit;
	}

	/* PSP must be finished with init and ready to accept a command */
	if (wait_initialized(mbox)) {
		status = -PSPSTS_INIT_TIMEOUT;
		goto exit;
	}
	if (wait_command(mbox)) {
		status = -PSPSTS_CMD_TIMEOUT;
		goto exit;
	}

	/* set address of command-response buffer and write command register */
	wr_mbox_cmd_resp(mbox, buffer);
	wr_mbox_cmd(mbox, command);

	/* PSP clears command register when complete */
	if (wait_command(mbox)) {
		status = -PSPSTS_CMD_TIMEOUT;
		goto exit;
	}

	/* check delivery status */
	if (rd_mbox_sts(mbox) & (STATUS_ERROR | STATUS_TERMINATED)) {
		status = -PSPSTS_SEND_ERROR;
		goto exit;
	}
exit:
	/* restore command register to original value */
	pci_write_config32(PSP_DEV, PCI_COMMAND, command_reg);
	return status;
}

/*
 * Notify the PSP that DRAM is present.  Upon receiving this command, the PSP
 * will load its OS into fenced DRAM that is not accessible to the x86 cores.
 */
int psp_notify_dram(void)
{
	struct mbox_default_buffer buffer;
	int cmd_status;

	printk(BIOS_DEBUG, "PSP: Notify that DRAM is available... ");

	buffer.header.size = sizeof(struct mbox_default_buffer);
	buffer.header.status = 0; /* PSP does not report status for this cmd */

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
