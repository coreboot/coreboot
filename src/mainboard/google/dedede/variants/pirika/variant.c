/* SPDX-License-Identifier: GPL-2.0-only */

#include <ec/google/chromeec/ec.h>
#include <fw_config.h>
#include <sar.h>

enum {
	PIRETTE_SKU_START   = 0x1D0000,
	PIRETTE_SKU_END     = 0x1Dffff,
	PIRIKA_SKU_START    = 0x1E0000,
	PIRIKA_SKU_END      = 0x1Effff,
	PASARA_SKU_START    = 0x2A0000,
	PASARA_SKU_END      = 0x2Affff,
	PALUTENA_SKU_START  = 0x350000,
	PALUTENA_SKU_END    = 0x35ffff,
};
const char *get_wifi_sar_cbfs_filename(void)
{
	uint32_t sku_id = google_chromeec_get_board_sku();

	if (sku_id >= PASARA_SKU_START && sku_id <= PASARA_SKU_END)
		return "wifi_sar-pasara.hex";

	if (sku_id >= PALUTENA_SKU_START && sku_id <= PALUTENA_SKU_END) {
		if (fw_config_probe(FW_CONFIG(WIFI_SAR_ID, WIFI_SAR_ID_1)))
			return "wifi_sar-palutena_INTEL_AX201.hex";
	}

	return WIFI_SAR_CBFS_DEFAULT_FILENAME;
}
