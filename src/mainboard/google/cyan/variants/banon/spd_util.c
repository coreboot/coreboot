/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <mainboard/google/cyan/spd/spd_util.h>

/*
 * 0b0000 - 4GiB total - 2 x 2GiB Samsung K4E8E304EE-EGCE
 * 0b0001 - 4GiB total - 2 x 2GiB Samsung K4E8E324EB-EGCF
 * 0b0011 - 4GiB total - 2 x 2GiB Hynix H9CCNNN8JTBLAR
 * 0b0100 - 4GiB total - 2 x 2GiB Micron MT52L256M32D1PF
 * 0b0101 - 4GiB total - 2 x 2GiB Hynix H9CCNNN8GTALAR
 * 0b0110 - 4GiB total - 2 x 2GiB NY NT6CL256T32CM-H1
 * 0b1000 - 2GiB total - 1 x 2GiB Samsung K4E8E304EE-EGCE
 * 0b1001 - 2GiB total - 1 x 2GiB Samsung K4E8E324EB-EGCF
 * 0b1011 - 2GiB total - 1 x 2GiB Hynix H9CCNNN8JTBLAR
 * 0b1100 - 2GiB total - 1 x 2GiB Micron MT52L256M32D1PF
 * 0b1101 - 2GiB total - 1 x 2GiB Hynix H9CCNNN8GTALAR
 * 0b1110 - 2GiB total - 1 x 2GiB NY NT6CL256T32CM-H1
 */

int get_variant_spd_index(int ram_id, int *dual)
{
	int spd_index = ram_id & 0x07;

	/* Determine if single or dual channel memory system */
	/* RAMID3 is deterministic for banon */
	*dual = ((ram_id >> 3) & 0x1) ? 0 : 1;

	/* Display the RAM type */
	printk(BIOS_DEBUG, *dual ? "4GiB " : "2GiB ");
	switch (spd_index) {
	case 0:
		printk(BIOS_DEBUG, "Samsung K4E8E304EE-EGCE\n");
		break;
	case 1:
		printk(BIOS_DEBUG, "Samsung K4E8E324EB-EGCF\n");
		break;
	case 2:
		printk(BIOS_DEBUG, "Unknown future LPDDR3\n");
		break;
	case 3:
		printk(BIOS_DEBUG, "Hynix H9CCNNN8JTBLAR\n");
		break;
	case 4:
		printk(BIOS_DEBUG, "Micron MT52L256M32D1PF\n");
		break;
	case 5:
		printk(BIOS_DEBUG, "Hynix H9CCNNN8GTALAR\n");
		break;
	case 6:
		printk(BIOS_DEBUG, "Nanya NT6CL256T32CM-H1\n");
		break;
	}
	return spd_index;
}
