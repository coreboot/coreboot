/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <device/mmio.h>
#include <timer.h>
#include <bootstate.h>
#include <amdblocks/psp.h>
#include <soc/iomap.h>
#include <soc/northbridge.h>
#include "psp_def.h"

static u32 rd_mbox_sts(struct pspv1_mbox *mbox)
{
	return read32(&mbox->mbox_status);
}

static void wr_mbox_cmd(struct pspv1_mbox *mbox, u32 cmd)
{
	write32(&mbox->mbox_command, cmd);
}

static u32 rd_mbox_cmd(struct pspv1_mbox *mbox)
{
	return read32(&mbox->mbox_command);
}

static void wr_mbox_cmd_resp(struct pspv1_mbox *mbox, void *buffer)
{
	write64(&mbox->cmd_response, (uintptr_t)buffer);
}

static int wait_initialized(struct pspv1_mbox *mbox)
{
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, PSP_INIT_TIMEOUT);

	do {
		if (rd_mbox_sts(mbox) & PSPV1_STATUS_INITIALIZED)
			return 0;
	} while (!stopwatch_expired(&sw));

	return -PSPSTS_INIT_TIMEOUT;
}

static int wait_command(struct pspv1_mbox *mbox)
{
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, PSP_CMD_TIMEOUT);

	do {
		if (!rd_mbox_cmd(mbox))
			return 0;
	} while (!stopwatch_expired(&sw));

	return -PSPSTS_CMD_TIMEOUT;
}

int send_psp_command(u32 command, void *buffer)
{
	struct pspv1_mbox *mbox = soc_get_mbox_address();
	if (!mbox)
		return -PSPSTS_NOBASE;

	/* check for PSP error conditions */
	if (rd_mbox_sts(mbox) & PSPV1_STATUS_HALT)
		return -PSPSTS_HALTED;

	if (rd_mbox_sts(mbox) & PSPV1_STATUS_RECOVERY)
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
	if (rd_mbox_sts(mbox) & (PSPV1_STATUS_ERROR | PSPV1_STATUS_TERMINATED))
		return -PSPSTS_SEND_ERROR;

	return 0;
}
