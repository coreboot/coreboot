/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Intel Corp.
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
#include <string.h>
#include <baseboard/variants.h>
#include <boardid.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <soc/meminit.h>
#include <soc/romstage.h>

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	meminit_lpddr4_by_sku(&memupd->FspmConfig,
		variant_lpddr4_config(), variant_memory_sku());
}

static void save_dimm_info_by_sku_config(void)
{
	save_lpddr4_dimm_info(variant_lpddr4_config(), variant_memory_sku());
}

void mainboard_save_dimm_info(void)
{
	char part_num_store[32];
	const char *part_num = NULL;

	if (!IS_ENABLED(CONFIG_DRAM_PART_NUM_IN_CBI)) {
		save_dimm_info_by_sku_config();
		return;
	}

	if (!IS_ENABLED(CONFIG_DRAM_PART_NUM_ALWAYS_IN_CBI)) {
		/* Fall back on part numbers encoded in lp4cfg array. */
		if (board_id() < CONFIG_DRAM_PART_IN_CBI_BOARD_ID_MIN) {
			save_dimm_info_by_sku_config();
			return;
		}
	}

	if (google_chromeec_cbi_get_dram_part_num(&part_num_store[0],
			ARRAY_SIZE(part_num_store)) < 0)
		printk(BIOS_ERR, "ERROR: Couldn't obtain DRAM part number from CBI\n");
	else
		part_num = &part_num_store[0];

	save_lpddr4_dimm_info_part_num(part_num);
}
