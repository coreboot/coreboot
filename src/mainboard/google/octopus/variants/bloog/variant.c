/* SPDX-License-Identifier: GPL-2.0-only */

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
	uint32_t sku_id = google_chromeec_get_board_sku();

	switch (sku_id) {
	case SKU_33_BLOOG:
	case SKU_34_BLOOG:
	case SKU_35_BLOOG:
	case SKU_36_BLOOG:
		return "wifi_sar-bloog.hex";
	case SKU_49_BLOOGUARD:
	case SKU_50_BLOOGUARD:
	case SKU_51_BLOOGUARD:
	case SKU_52_BLOOGUARD:
	case SKU_53_BIPSHIP:
	case SKU_54_BIPSHIP:
		return "wifi_sar-blooguard.hex";
	}
	return WIFI_SAR_CBFS_DEFAULT_FILENAME;
}

const char *mainboard_vbt_filename(void)
{
	uint32_t sku_id;

	sku_id = google_chromeec_get_board_sku();

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
