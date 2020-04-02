/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <bootblock_common.h>
#include <superio/fintek/common/fintek.h>
#include <superio/fintek/f71869ad/f71869ad.h>

/* Ensure Super I/O config address (i.e., 0x2e or 0x4e) matches that of devicetree.cb */
#define SERIAL_DEV PNP_DEV(0x2e, F71869AD_SP1)

void bootblock_mainboard_early_init(void)
{
	fintek_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
