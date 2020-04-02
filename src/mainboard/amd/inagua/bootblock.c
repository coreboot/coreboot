/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <bootblock_common.h>
#include <superio/smsc/kbc1100/kbc1100.h>

#define SERIAL_DEV PNP_DEV(0x2e, SMSCSUPERIO_SP1)

void bootblock_mainboard_early_init(void)
{
	kbc1100_early_init(0x2e);
	kbc1100_early_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
