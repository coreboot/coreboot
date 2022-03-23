/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <arch/io.h>
#include <bootblock_common.h>

void bootblock_mainboard_early_init(void)
{
	int i;

	/* Disable PCI-PCI bridge and release GPIO32/33 for other uses. */
	pm_write8(0xea, 0x1);

	/* On Larne, after LpcClkDrvSth is set, it needs some time to be stable, because of the buffer ICS551M */
	for (i = 0; i < 200000; i++)
		inb(0xcd6);
}
