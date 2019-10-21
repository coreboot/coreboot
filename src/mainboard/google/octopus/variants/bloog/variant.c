/*
 * This file is part of the coreboot project.
 *
 * Copyright 2019 Google LLC
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

#include <boardid.h>
#include <ec/google/chromeec/ec.h>
#include <sar.h>
#include <baseboard/variants.h>
#include <drivers/intel/gma/opregion.h>

#define SKU_UNKNOWN     0xFFFFFFFF

enum {
	SKU_33_BLOOG = 33, /* no kb blit, USI Stylus */
	SKU_34_BLOOG = 34, /* kb blit, no USI Stylus */
	SKU_35_BLOOG = 35, /* kb blit, USI Stylus */
	SKU_36_BLOOG = 36, /* no kb blit, no USI Stylus */
	SKU_49_BLOOGUARD = 49, /* kb blit, no USI Stylus */
	SKU_50_BLOOGUARD = 50, /* kb blit, USI Stylus */
	SKU_51_BLOOGUARD = 51, /* no kb blit, no USI Stylus */
	SKU_52_BLOOGUARD = 52, /* no kb blit, USI Stylus */
	SKU_65_BLOOGLET = 65, /* TS, kb blit */
	SKU_66_BLOOGLET = 66, /* TS, no kb blit */
	SKU_67_BLOOGLET = 67, /* non-TS, kb blit */
	SKU_68_BLOOGLET = 68, /* non-TS, no kb blit */
};

const char *get_wifi_sar_cbfs_filename(void)
{
	const char *filename = NULL;
	uint32_t sku_id;
	sku_id = get_board_sku();

	if (sku_id == SKU_UNKNOWN)
		return NULL;

	if (sku_id == SKU_33_BLOOG || sku_id == SKU_34_BLOOG ||
		sku_id == SKU_35_BLOOG || sku_id == SKU_36_BLOOG)
		filename = "wifi_sar-bloog.hex";

	if (sku_id == SKU_49_BLOOGUARD || sku_id == SKU_50_BLOOGUARD ||
		sku_id == SKU_51_BLOOGUARD || sku_id == SKU_52_BLOOGUARD)
		filename = "wifi_sar-blooguard.hex";

	return filename;
}

const char *mainboard_vbt_filename(void)
{
	uint32_t sku_id;

	sku_id = get_board_sku();

	if (sku_id == SKU_49_BLOOGUARD || sku_id == SKU_50_BLOOGUARD ||
		sku_id == SKU_51_BLOOGUARD || sku_id == SKU_52_BLOOGUARD ||
		sku_id == SKU_65_BLOOGLET || sku_id == SKU_66_BLOOGLET ||
		sku_id == SKU_67_BLOOGLET || sku_id == SKU_68_BLOOGLET)
		return "vbt_blooguard.bin";

	return "vbt.bin";
}
