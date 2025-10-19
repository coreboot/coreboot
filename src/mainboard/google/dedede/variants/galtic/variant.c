/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <device/device.h>
#include <ec/google/chromeec/ec.h>
#include <fw_config.h>
#include <option.h>
#include <sar.h>
#include <static.h>

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

#define TP_TYPE_AUTO_SELECT 0
#define TP_TYPE_ELAN0000 1
#define TP_TYPE_ELAN2702 2

void variant_devtree_update(void)
{
	struct device *tp_elan0000 = DEV_PTR(elan0000);
	struct device *tp_elan2702 = DEV_PTR(elan2702);

	/* Update touchpad device */
	switch (get_uint_option("touchpad", TP_TYPE_AUTO_SELECT)) {
	case TP_TYPE_ELAN0000:
		tp_elan2702->enabled = 0;
		break;
	case TP_TYPE_ELAN2702:
		tp_elan0000->enabled = 0;
		break;
	}
}
