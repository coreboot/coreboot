/*
 * This file is part of the coreboot project.
 *
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

#include <console/console.h>
#include <mainboard/google/cyan/spd/spd_util.h>

/* We use RAM_ID3 to indicate dual channel config.
 *
 * 0b0000 - 2GiB total - 1 x 2GiB Samsung K4E8E304EE-EGCE 1600MHz
 * 0b0001 - 2GiB total - 1 x 2GiB Hynix H9CCNNN8GTMLAR-NUD 1600MHz
 * 0b0010 - 2GiB total - 1 x 2GiB Micron EDF8132A3MA-JD-F 1600MHz
 * 0b0011 - 2GiB total - 1 x 2GiB Samsung K4E8E324EB-EGCF 1600MHz
 * 0b0100 - 2GiB total - 1 x 2GiB Hynix H9CCNNN8JTBLAR-NUD 1866MHz
 *
 * 0b1000 - 4GiB total - 2 x 2GiB Samsung K4E8E304EE-EGCE 1600MHz
 * 0b1001 - 4GiB total - 2 x 2GiB Hynix H9CCNNN8GTMLAR-NUD 1600MHz
 * 0b1010 - 4GiB total - 2 x 2GiB Micron EDF8132A3MA-JD-F 1600MHz
 * 0b1011 - 4GiB total - 2 x 2GiB Samsung K4E8E324EB-EGCF 1600MHz
 * 0b1100 - 4GiB total - 2 x 2GiB Hynix H9CCNNN8JTBLAR-NUD 1866MHz
 *
 */

int get_variant_spd_index(int ram_id, int *dual)
{
	int spd_index = ram_id & 0x7;

	/* Determine if single or dual channel memory system */
	/* RAMID3 is deterministic for ultima */
	*dual = !!(ram_id & (1<<3));

	/* Display the RAM type */
	printk(BIOS_DEBUG, *dual ? "4GiB " : "2GiB ");
	switch (spd_index) {
	case 0:
		printk(BIOS_DEBUG, "Samsung K4E8E304EE-EGCE\n");
		break;
	case 1:
		printk(BIOS_DEBUG, "Hynix H9CCNNN8GTMLAR-NUD\n");
		break;
	case 2:
		printk(BIOS_DEBUG, "Micron EDF8132A3MA-JD-F\n");
		break;
	case 3:
		printk(BIOS_DEBUG, "Samsung K4E8E324EB-EGCF\n");
		break;
	case 4:
		printk(BIOS_DEBUG, "Hynix H9CCNNN8JTBLAR-NUD\n");
		break;
	}

	return spd_index;
}
