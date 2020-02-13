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
#include <device/pnp_ops.h>

void bootblock_mainboard_early_init(void)
{
	int i;
	u32 val;

	/* Disable PCI-PCI bridge and release GPIO32/33 for other uses. */
	pm_write8(0xea, 0x1);

	/* On Larne, after LpcClkDrvSth is set, it needs some time to be stable, because of the buffer ICS551M */
	for (i = 0; i < 200000; i++)
		val = inb(0xcd6);
}
