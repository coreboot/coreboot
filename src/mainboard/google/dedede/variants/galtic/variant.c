/* SPDX-License-Identifier: GPL-2.0-only */

#include <ec/google/chromeec/ec.h>
#include <fw_config.h>
#include <sar.h>

enum {
	GALTIC_SKU_START    = 0x120000,
	GALTIC_SKU_END      = 0x12ffff,
	GALITH_SKU_START    = 0x130000,
	GALITH_SKU_END      = 0x13ffff,
	GALNAT_SKU_START    = 0x140000,
	GALNAT_SKU_END      = 0x14ffff,
	GALLOP_SKU_START    = 0x150000,
	GALLOP_SKU_END      = 0x15ffff,
	GALTIC360_SKU_START = 0x260000,
	GALTIC360_SKU_END   = 0x26ffff,
	GALITH360_SKU_START = 0x270000,
	GALITH360_SKU_END   = 0x27ffff,
	GALNAT360_SKU_START = 0x2B0000,
	GALNAT360_SKU_END   = 0x2Bffff,
};
const char *get_wifi_sar_cbfs_filename(void)
{
	uint32_t sku_id = google_chromeec_get_board_sku();

	if (sku_id >= GALTIC_SKU_START && sku_id <= GALTIC_SKU_END)
		return "wifi_sar-galtic.hex";
	if (sku_id >= GALNAT_SKU_START && sku_id <= GALNAT_SKU_END)
		return "wifi_sar-galnat.hex";
	if (sku_id >= GALTIC360_SKU_START && sku_id <= GALTIC360_SKU_END)
		return "wifi_sar-galtic360.hex";
	if (sku_id >= GALITH360_SKU_START && sku_id <= GALITH360_SKU_END)
		return "wifi_sar-galith360.hex";
	if (sku_id >= GALNAT360_SKU_START && sku_id <= GALNAT360_SKU_END)
		return "wifi_sar-galnat360.hex";

	return WIFI_SAR_CBFS_DEFAULT_FILENAME;
}
