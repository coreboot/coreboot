/* SPDX-License-Identifier: GPL-2.0-only */

#include <timer.h>
#include <types.h>
#include <amdblocks/psp.h>
#include <amdblocks/smn.h>
#include "psp_def.h"

#define PSP_MAILBOX_COMMAND_OFFSET	0x10570 /* 4 bytes */
#define PSP_MAILBOX_BUFFER_L_OFFSET	0x10574 /* 4 bytes */
#define PSP_MAILBOX_BUFFER_H_OFFSET	0x10578 /* 4 bytes */

union pspv2_mbox_command {
	u32 val;
	struct pspv2_mbox_cmd_fields {
		u16 mbox_status;
		u8 mbox_command;
		u32 reserved:6;
		u32 recovery:1;
		u32 ready:1;
	} __packed fields;
};

static u16 rd_mbox_sts(void)
{
	union pspv2_mbox_command tmp;

	tmp.val = smn_read32(SMN_PSP_PUBLIC_BASE + PSP_MAILBOX_COMMAND_OFFSET);
	return tmp.fields.mbox_status;
}

static void wr_mbox_cmd(u8 cmd)
{
	union pspv2_mbox_command tmp = { .val = 0 };

	/* Write entire 32-bit area to begin command execution */
	tmp.fields.mbox_command = cmd;
	smn_write32(SMN_PSP_PUBLIC_BASE + PSP_MAILBOX_COMMAND_OFFSET, tmp.val);
}

static u8 rd_mbox_recovery(void)
{
	union pspv2_mbox_command tmp;

	tmp.val = smn_read32(SMN_PSP_PUBLIC_BASE + PSP_MAILBOX_COMMAND_OFFSET);
	return !!tmp.fields.recovery;
}

static void wr_mbox_buffer_ptr(void *buffer)
{
	const uint32_t buf_addr_h = (uint64_t)(uintptr_t)buffer >> 32;
	const uint32_t buf_addr_l = (uint64_t)(uintptr_t)buffer & 0xffffffff;
	smn_write32(SMN_PSP_PUBLIC_BASE + PSP_MAILBOX_BUFFER_H_OFFSET, buf_addr_h);
	smn_write32(SMN_PSP_PUBLIC_BASE + PSP_MAILBOX_BUFFER_L_OFFSET, buf_addr_l);
}

static int wait_command(bool wait_for_ready)
{
	union pspv2_mbox_command and_mask = { .val = ~0 };
	union pspv2_mbox_command expected = { .val = 0 };
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
		tmp = smn_read32(SMN_PSP_PUBLIC_BASE + PSP_MAILBOX_COMMAND_OFFSET);
		tmp &= ~and_mask.val;
		if (tmp == expected.val)
			return 0;
	} while (!stopwatch_expired(&sw));

	return -PSPSTS_CMD_TIMEOUT;
}

int send_psp_command(u32 command, void *buffer)
{
	if (rd_mbox_recovery())
		return -PSPSTS_RECOVERY;

	if (wait_command(true))
		return -PSPSTS_CMD_TIMEOUT;

	/* set address of command-response buffer and write command register */
	wr_mbox_buffer_ptr(buffer);
	wr_mbox_cmd(command);

	/* PSP clears command register when complete.  All commands except
	 * SxInfo set the Ready bit. */
	if (wait_command(command != MBOX_BIOS_CMD_SX_INFO))
		return -PSPSTS_CMD_TIMEOUT;

	/* check delivery status */
	if (rd_mbox_sts())
		return -PSPSTS_SEND_ERROR;

	return 0;
}

uint32_t soc_read_c2p38(void)
{
	return smn_read32(SMN_PSP_PUBLIC_BASE + CORE_2_PSP_MSG_38_OFFSET);
}
