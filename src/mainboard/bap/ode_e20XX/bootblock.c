/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <amdblocks/acpimmio.h>
#include <bootblock_common.h>
#include <superio/fintek/common/fintek.h>
#include <superio/fintek/f81866d/f81866d.h>

#define SERIAL_DEV1 PNP_DEV(0x4e, F81866D_SP1)

void bootblock_mainboard_early_init(void)
{
	/* Disable PCI-PCI bridge and release GPIO32/33 for other uses. */
	pm_write8(0xea, 0x1);

	fintek_enable_serial(SERIAL_DEV1, CONFIG_TTYS0_BASE);
}
