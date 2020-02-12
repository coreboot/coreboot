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
	SKU_53_BIPSHIP = 53, /* no kb blit, TS, 360, no Stylus, no rare-cam */
	SKU_54_BIPSHIP = 54, /* kb blit, TS, 360, no Stylus, no rare-cam */
	SKU_65_BLOOGLET = 65, /* TS, kb blit */
	SKU_66_BLOOGLET = 66, /* TS, no kb blit */
	SKU_67_BLOOGLET = 67, /* non-TS, kb blit */
	SKU_68_BLOOGLET = 68, /* non-TS, no kb blit */
	SKU_255_UNPROVISIONED = 255,
};

const char *get_wifi_sar_cbfs_filename(void)
{
	const char *filename = NULL;
	uint32_t sku_id = get_board_sku();

	switch (sku_id) {
	case SKU_33_BLOOG:
	case SKU_34_BLOOG:
	case SKU_35_BLOOG:
	case SKU_36_BLOOG:
		filename = "wifi_sar-bloog.hex";
		break;
	case SKU_49_BLOOGUARD:
	case SKU_50_BLOOGUARD:
	case SKU_51_BLOOGUARD:
	case SKU_52_BLOOGUARD:
	case SKU_53_BIPSHIP:
	case SKU_54_BIPSHIP:
		filename = "wifi_sar-blooguard.hex";
		break;
	}
	return filename;
}

const char *mainboard_vbt_filename(void)
{
	uint32_t sku_id;

	sku_id = get_board_sku();

	switch (sku_id) {
	case SKU_33_BLOOG:
	case SKU_34_BLOOG:
	case SKU_35_BLOOG:
	case SKU_36_BLOOG:
	case SKU_255_UNPROVISIONED:
		return "vbt.bin";
	default:
		return "vbt_blooguard.bin";
	}
}
