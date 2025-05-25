/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <northbridge/intel/haswell/haswell.h>
#include <types.h>

#include "haswell.h"

int pcode_ready(void)
{
	int wait_count;
	const int delay_step = 10;

	wait_count = 0;
	do {
		if (!(mchbar_read32(BIOS_MAILBOX_INTERFACE) & MAILBOX_RUN_BUSY))
			return 0;
		wait_count += delay_step;
		udelay(delay_step);
	} while (wait_count < 1000);

	return -1;
}

u32 pcode_mailbox_read(u32 command)
{
	if (pcode_ready() < 0) {
		printk(BIOS_ERR, "PCODE: mailbox timeout on wait ready.\n");
		return 0;
	}

	/* Send command and start transaction */
	mchbar_write32(BIOS_MAILBOX_INTERFACE, command | MAILBOX_RUN_BUSY);

	if (pcode_ready() < 0) {
		printk(BIOS_ERR, "PCODE: mailbox timeout on completion.\n");
		return 0;
	}

	/* Read mailbox */
	return mchbar_read32(BIOS_MAILBOX_DATA);
}

int pcode_mailbox_write(u32 command, u32 data)
{
	if (pcode_ready() < 0) {
		printk(BIOS_ERR, "PCODE: mailbox timeout on wait ready.\n");
		return -1;
	}

	mchbar_write32(BIOS_MAILBOX_DATA, data);

	/* Send command and start transaction */
	mchbar_write32(BIOS_MAILBOX_INTERFACE, command | MAILBOX_RUN_BUSY);

	if (pcode_ready() < 0) {
		printk(BIOS_ERR, "PCODE: mailbox timeout on completion.\n");
		return -1;
	}

	return 0;
}
