/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/x86/msr.h>
#include <device/mmio.h>
#include <timer.h>
#include <amdblocks/psp.h>
#include <soc/iomap.h>
#include "psp_def.h"

#define PSP_MAILBOX_OFFSET		0x10570

static uintptr_t soc_get_psp_base_address(void)
{
	uintptr_t psp_mmio = rdmsr(MSR_PSP_ADDR).lo;
	if (!psp_mmio)
		printk(BIOS_ERR, "PSP: MSR_PSP_ADDR uninitialized\n");
	return psp_mmio;
}

void *soc_get_mbox_address(void)
{
	uintptr_t psp_mmio = soc_get_psp_base_address();
	if (!psp_mmio)
		return 0;
	return (void *)(psp_mmio + PSP_MAILBOX_OFFSET);
}

static u16 rd_mbox_sts(struct pspv2_mbox *mbox)
{
	union {
		u32 val;
		struct pspv2_mbox_cmd_fields fields;
	} tmp = { 0 };

	tmp.val = read32(&mbox->val);
	return tmp.fields.mbox_status;
}

static void wr_mbox_cmd(struct pspv2_mbox *mbox, u8 cmd)
{
	union {
		u32 val;
		struct pspv2_mbox_cmd_fields fields;
	} tmp = { 0 };

	/* Write entire 32-bit area to begin command execution */
	tmp.fields.mbox_command = cmd;
	write32(&mbox->val, tmp.val);
}

static u8 rd_mbox_recovery(struct pspv2_mbox *mbox)
{
	union {
		u32 val;
		struct pspv2_mbox_cmd_fields fields;
	} tmp = { 0 };

	tmp.val = read32(&mbox->val);
	return !!tmp.fields.recovery;
}

static void wr_mbox_cmd_resp(struct pspv2_mbox *mbox, void *buffer)
{
	write64(&mbox->cmd_response, (uintptr_t)buffer);
}

static int wait_command(struct pspv2_mbox *mbox, bool wait_for_ready)
{
	struct pspv2_mbox and_mask = { .val = ~0 };
	struct pspv2_mbox expected = { .val = 0 };
	struct stopwatch sw;
	u32 tmp;

	/* Zero fields from and_mask that should be kept */
	and_mask.fields.mbox_command = 0;
	and_mask.fields.ready = wait_for_ready ? 0 : 1;

	/* Expect mbox_cmd == 0 but ready depends */
	if (wait_for_ready)
		expected.fields.ready = 1;

	stopwatch_init_msecs_expire(&sw, PSP_CMD_TIMEOUT);

	do {
		tmp = read32(&mbox->val);
		tmp &= ~and_mask.val;
		if (tmp == expected.val)
			return 0;
	} while (!stopwatch_expired(&sw));

	return -PSPSTS_CMD_TIMEOUT;
}

int send_psp_command(u32 command, void *buffer)
{
	struct pspv2_mbox *mbox = soc_get_mbox_address();
	if (!mbox)
		return -PSPSTS_NOBASE;

	if (rd_mbox_recovery(mbox))
		return -PSPSTS_RECOVERY;

	if (wait_command(mbox, true))
		return -PSPSTS_CMD_TIMEOUT;

	/* set address of command-response buffer and write command register */
	wr_mbox_cmd_resp(mbox, buffer);
	wr_mbox_cmd(mbox, command);

	/* PSP clears command register when complete.  All commands except
	 * SxInfo set the Ready bit. */
	if (wait_command(mbox, command != MBOX_BIOS_CMD_SX_INFO))
		return -PSPSTS_CMD_TIMEOUT;

	/* check delivery status */
	if (rd_mbox_sts(mbox))
		return -PSPSTS_SEND_ERROR;

	return 0;
}
