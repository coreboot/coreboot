/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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

#include <stdint.h>
#include <string.h>
#include <cbfs.h>
#include <console/console.h>
#include <soc/gpio.h>
#include <soc/mrc_wrapper.h>
#include <soc/romstage.h>
#include <variant/variant.h>

static void *get_spd_pointer(char *spd_file_content, int total_spds, int *dual)
{
	int ram_id = 0;

	/* The ram_id[2:0] pullups are too large for the default 20K
	 * pulldown on the pad. Therefore, disable the internal pull resistor to
	 * read high values correctly. */
	ssus_disable_internal_pull(GPIO_SSUS_37_PAD);
	ssus_disable_internal_pull(GPIO_SSUS_38_PAD);
	ssus_disable_internal_pull(GPIO_SSUS_39_PAD);
#ifdef GPIO_SSUS_40_PAD_USE_PULLDOWN
	/* To prevent floating pin on shipped systems. */
	ssus_enable_internal_pull(GPIO_SSUS_40_PAD, PAD_PULL_DOWN | PAD_PU_20K);
#elif defined(GPIO_SSUS_40_PAD)
	ssus_disable_internal_pull(GPIO_SSUS_40_PAD);
#endif
	ram_id |= (ssus_get_gpio(GPIO_SSUS_37_PAD) << 0);
	ram_id |= (ssus_get_gpio(GPIO_SSUS_38_PAD) << 1);
	ram_id |= (ssus_get_gpio(GPIO_SSUS_39_PAD) << 2);
#ifdef GPIO_SSUS_40_PAD
	ram_id |= (ssus_get_gpio(GPIO_SSUS_40_PAD) << 3);
#endif
	printk(BIOS_DEBUG, "ram_id=%d, total_spds: %d\n", ram_id, total_spds);

	if (ram_id >= total_spds)
		return NULL;

	/* Single channel configs */
	if (dual_channel_config & (1 << ram_id))
		*dual = 1;

	return &spd_file_content[SPD_SIZE * ram_id];
}

void mainboard_romstage_entry(struct romstage_params *rp)
{
	void *spd_content;
	int dual_channel = 0;
	void *spd_file;
	size_t spd_fsize;

	struct mrc_params mp = {
		.mainboard = {
			.dram_type = DRAM_DDR3L,
			.dram_info_location = DRAM_INFO_SPD_MEM,
			.weaker_odt_settings = 1,
		},
	};

	spd_file = cbfs_boot_map_with_leak("spd.bin", CBFS_TYPE_SPD,
						&spd_fsize);
	if (!spd_file)
		die("SPD data not found.");

	spd_content = get_spd_pointer(spd_file, spd_fsize / SPD_SIZE,
	                              &dual_channel);
	mp.mainboard.dram_data[0] = spd_content;
	if (dual_channel)
		mp.mainboard.dram_data[1] = spd_content;

	rp->mrc_params = &mp;
	romstage_common(rp);
}
