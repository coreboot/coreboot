/*
 * This file is part of the coreboot project.
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
