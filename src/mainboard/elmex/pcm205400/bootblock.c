/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <bootblock_common.h>
#include <superio/fintek/common/fintek.h>
#include <superio/fintek/f81865f/f81865f.h>

#define SERIAL_DEV PNP_DEV(0x4e, F81865F_SP1)

void bootblock_mainboard_early_init(void)
{
	fintek_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
