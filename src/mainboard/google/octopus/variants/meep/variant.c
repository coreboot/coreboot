/* SPDX-License-Identifier: GPL-2.0-only */

#include <ec/google/chromeec/ec.h>
#include <drivers/intel/gma/opregion.h>
#include <baseboard/variants.h>
#include <variant/sku.h>
#include <sar.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	uint32_t sku_id = google_chromeec_get_board_sku();

	switch (sku_id) {
	case SKU_1_MEEP:
	case SKU_2_MEEP:
	case SKU_3_MEEP:
	case SKU_4_MEEP:
		return "wifi_sar-meep.hex";
	case SKU_49_VORTININJA:
	case SKU_50_VORTININJA:
	case SKU_51_VORTININJA:
	case SKU_52_VORTININJA:
		return "wifi_sar-vortininja.hex";
	}
	return WIFI_SAR_CBFS_DEFAULT_FILENAME;
}

const char *mainboard_vbt_filename(void)
{
	uint32_t sku_id;

	sku_id = google_chromeec_get_board_sku();

	switch (sku_id) {
	case SKU_33_DORP:
	case SKU_34_DORP:
	case SKU_35_DORP:
	case SKU_36_DORP:
		return "vbt_dorp_hdmi.bin";
	case SKU_49_VORTININJA:
	case SKU_50_VORTININJA:
	case SKU_51_VORTININJA:
	case SKU_52_VORTININJA:
	case SKU_65_VORTICON:
	case SKU_66_VORTICON:
		return "vbt_vortininja.bin";
	default:
		return "vbt.bin";
	}
}
