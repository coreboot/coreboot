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

/*
 * RAM_ID[2:0] are on GPIO_SSUS[39:37]
 * 0b000 - 4GiB total - 2 x 2GiB Micron MT41K256M16HA-125:E 1600MHz
 * 0b001 - 4GiB total - 2 x 2GiB Hynix  H5TC4G63AFR-PBA 1600MHz
 * 0b010 - 2GiB total - 2 x 1GiB Micron MT41K128M16JT-125:K 1600MHz
 * 0b011 - 2GiB total - 2 x 1GiB Hynix  H5TC2G63FFR-PBA 1600MHz
 * 0b100 - 2GiB total - 1 x 2GiB Micron MT41K256M16HA-125:E 1600MHz
 * 0b101 - 2GiB total - 1 x 2GiB Hynix  H5TC4G63AFR-PBA 1600MHz
 */
static const uint32_t dual_channel_config =
	(1 << 0) | (1 << 1) | (1 << 2) | (1 << 3);

#define SPD_SIZE 256
#define GPIO_SSUS_37_PAD 57
#define GPIO_SSUS_38_PAD 50
#define GPIO_SSUS_39_PAD 58

static void *get_spd_pointer(char *spd_file_content, int total_spds, int *dual)
{
	int ram_id = 0;

	/* The ram_id[2:0] pullups on rambi are too large for the default 20K
	 * pulldown on the pad. Therefore, disable the internal pull resistor to
	 * read high values correctly. */
	ssus_disable_internal_pull(GPIO_SSUS_37_PAD);
	ssus_disable_internal_pull(GPIO_SSUS_38_PAD);
	ssus_disable_internal_pull(GPIO_SSUS_39_PAD);

	ram_id |= (ssus_get_gpio(GPIO_SSUS_37_PAD) << 0);
	ram_id |= (ssus_get_gpio(GPIO_SSUS_38_PAD) << 1);
	ram_id |= (ssus_get_gpio(GPIO_SSUS_39_PAD) << 2);

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
