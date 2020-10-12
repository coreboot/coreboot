/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <memory_info.h>

const char *mainboard_get_dram_part_num(void)
{
	static char part_num_store[DIMM_INFO_PART_NUMBER_SIZE];
	static enum {
		PART_NUM_NOT_READ,
		PART_NUM_AVAILABLE,
		PART_NUM_NOT_IN_CBI,
	} part_num_state = PART_NUM_NOT_READ;

	if (part_num_state == PART_NUM_NOT_READ) {
		if (google_chromeec_cbi_get_dram_part_num(part_num_store,
						  sizeof(part_num_store)) < 0) {
			printk(BIOS_DEBUG,
			  "Couldn't obtain DRAM part number from CBI\n");
			part_num_state = PART_NUM_NOT_IN_CBI;
		} else {
			part_num_state = PART_NUM_AVAILABLE;
		}
	}

	if (part_num_state == PART_NUM_NOT_IN_CBI)
		return NULL;

	return part_num_store;
}
