/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <console/console.h>

#include "spd.h"

/* Get SPD data for on-board memory */
uint8_t *mainboard_find_spd_data(void)
{
	uint8_t *spd_data;
	int spd_index;
	size_t spd_file_len;
	char *spd_file;

	spd_index = 0;

	spd_file = cbfs_map("spd.bin", &spd_file_len);
	if (!spd_file)
		die("SPD data not found.");

	if (spd_file_len < ((spd_index + 1) * SPD_LEN)) {
		printk(BIOS_ERR,
		       "SPD index override to 0 due to incorrect SPD index.\n");
		spd_index = 0;
	}

	if (spd_file_len < SPD_LEN)
		die("Missing SPD data.");

	/* Assume same memory in both channels */
	spd_index *= SPD_LEN;
	spd_data = (uint8_t *)(spd_file + spd_index);

	/* Make sure a valid SPD was found */
	if (spd_data[0] == 0)
		die("Invalid SPD data.");

	return spd_data;
}
