/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
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

#include <baseboard/variants.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <memory_info.h>
#include <soc/cnl_memcfg_init.h>
#include <soc/romstage.h>
#include <string.h>

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	struct cnl_mb_cfg memcfg;

	const struct spd_info spd = {
		.spd_by_index = true,
		.spd_spec.spd_index = variant_memory_sku(),
	};

	variant_memory_params(&memcfg);
	cannonlake_memcfg_init(&memupd->FspmConfig,
			       &memcfg, &spd);
}

void mainboard_get_dram_part_num(const char **part_num, size_t *len)
{
	static char part_num_store[DIMM_INFO_PART_NUMBER_SIZE];
	static enum {
		PART_NUM_NOT_READ,
		PART_NUM_AVAILABLE,
		PART_NUM_NOT_IN_CBI,
	} part_num_state = PART_NUM_NOT_READ;

	if (part_num_state == PART_NUM_NOT_READ) {
		if (google_chromeec_cbi_get_dram_part_num(&part_num_store[0],
						sizeof(part_num_store)) < 0) {
			printk(BIOS_ERR, "No DRAM part number in CBI!\n");
			part_num_state = PART_NUM_NOT_IN_CBI;
		} else {
			part_num_state = PART_NUM_AVAILABLE;
		}
	}

	if (part_num_state == PART_NUM_NOT_IN_CBI)
		return;

	*part_num = &part_num_store[0];
	*len = strlen(part_num_store) + 1;
}
