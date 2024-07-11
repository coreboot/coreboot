/* SPDX-License-Identifier: GPL-2.0-only */

#include <timer.h>
#include <types.h>
#include <amdblocks/psp.h>
#include <amdblocks/root_complex.h>
#include <amdblocks/smn.h>
#include "psp_def.h"

#define PSP_MAILBOX_COMMAND_OFFSET	0x10570 /* 4 bytes */
#define PSP_MAILBOX_BUFFER_L_OFFSET	0x10574 /* 4 bytes */
#define PSP_MAILBOX_BUFFER_H_OFFSET	0x10578 /* 4 bytes */

#define IOHC_MISC_PSP_MMIO_REG		0x2e0

static uint64_t get_psp_mmio_mask(void)
{
	const struct non_pci_mmio_reg *mmio_regs;
	size_t reg_count;
	mmio_regs = get_iohc_non_pci_mmio_regs(&reg_count);

	for (size_t i = 0; i < reg_count; i++) {
		if (mmio_regs[i].iohc_misc_offset == IOHC_MISC_PSP_MMIO_REG)
			return mmio_regs[i].mask;
	}

	printk(BIOS_ERR, "No PSP MMIO register description found.\n");
	return 0;
}

#define PSP_MMIO_LOCK	BIT(8)

/* Getting the PSP MMIO base from the domain resources only works in ramstage, but not in SMM,
   so we have to read this from the hardware registers */
uintptr_t get_psp_mmio_base(void)
{
	static uintptr_t psp_mmio_base;
	const struct domain_iohc_info *iohc;
	size_t iohc_count;

	if (psp_mmio_base)
		return psp_mmio_base;

	iohc = get_iohc_info(&iohc_count);
	const uint64_t psp_mmio_mask = get_psp_mmio_mask();

	if (!psp_mmio_mask)
		return 0;

	for (size_t i = 0; i < iohc_count; i++) {
		uint64_t reg64 = smn_read64(iohc[i].misc_smn_base | IOHC_MISC_PSP_MMIO_REG);

		if (!(reg64 & IOHC_MMIO_EN))
			continue;

		const uint64_t base = reg64 & psp_mmio_mask;

		if (ENV_X86_32 && base >= 4ull * GiB) {
			printk(BIOS_WARNING, "PSP MMIO base above 4GB.\n");
			continue;
		}

		/* If the PSP MMIO base is enabled but the register isn't locked, set the lock
		   bit. This shouldn't happen, but better be a bit too careful here */
		if (!(reg64 & PSP_MMIO_LOCK)) {
			printk(BIOS_WARNING, "Enabled PSP MMIO in domain %zu isn't locked. "
					     "Locking it.\n", i);
			reg64 |= PSP_MMIO_LOCK;
			/* Since the lock bit lives in the lower one of the two 32 bit SMN
			   registers, we only need to write that one to lock it */
			smn_write32(iohc[i].misc_smn_base | IOHC_MISC_PSP_MMIO_REG,
				    reg64 & 0xffffffff);
		}

		psp_mmio_base = base;
	}

	if (!psp_mmio_base)
		printk(BIOS_ERR, "No usable PSP MMIO found.\n");

	return psp_mmio_base;
}

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
