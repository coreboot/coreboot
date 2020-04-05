/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <console/console.h>
#include <mainboard/google/cyan/spd/spd_util.h>

/*
 * RAMID3 -1: Dual channel SKU, 0: Single channel SKU
 * 0b0010 - 2GiB total - 1 x 2GiB Micron  EDF8132A3MA-GD-F-R	1600MHz
 * 0b0011 - 2GiB total - 1 x 2GiB Micron  MT52L256M32D1PF-107WT	1866MHz
 * 0b0100 - 2GiB total - 1 x 2GiB Samsung K4E8E304EE-EGCE	1600MHz
 * 0b0101 - 2GiB total - 1 x 2GiB Samsung K4E8E324EB-EGCF	1866MHz
 *
 * 0b1010 - 4GiB total - 2 x 2GiB Micron  EDF8132A3MA-GD-F-R	1600MHz
 * 0b1011 - 4GiB total - 2 x 2GiB Micron  MT52L256M32D1PF-107WT	1866MHz
 * 0b1100 - 4GiB total - 2 x 2GiB Samsung K4E8E304EE-EGCE	1600MHz
 * 0b1101 - 4GiB total - 2 x 2GiB Samsung K4E8E324EB-EGCF	1866MHz
 */

int get_variant_spd_index(int ram_id, int *dual)
{
	int spd_index = ram_id & 0x03;

	/* Determine if single or dual channel memory system */
	/* RAMID3 is deterministic for terra */
	*dual = ((ram_id >> 3) & 0x1) ? 1 : 0;

	/* Display the RAM type */
	printk(BIOS_DEBUG, *dual ? "4GiB " : "2GiB ");
	switch (spd_index) {
	case 0:
		printk(BIOS_DEBUG, "Samsung K4E8E304EE-EGCE 1600MHz\n");
		break;
	case 1:
		printk(BIOS_DEBUG, "Samsung K4E8E324EB-EGCF 1866MHz\n");
		break;
	case 2:
		printk(BIOS_DEBUG, "Micron EDF8132A3MA-GD-F-R 1600MHz\n");
		break;
	case 3:
		printk(BIOS_DEBUG, "Micron MT52L256M32D1PF-107WT 1866MHz\n");
		break;
	}

	return spd_index;
}
