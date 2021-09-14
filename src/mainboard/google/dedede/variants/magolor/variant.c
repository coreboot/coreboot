/* SPDX-License-Identifier: GPL-2.0-only */

#include <drivers/intel/gma/opregion.h>
#include <ec/google/chromeec/ec.h>
#include <fw_config.h>
#include <sar.h>

enum {
	MAGOLOR_SKU_START = 0x70000,
	MAGOLOR_SKU_END = 0x7ffff,
	MAGLIA_SKU_START = 0x80000,
	MAGLIA_SKU_END = 0x8ffff,
	MAGISTER_SKU_START = 0xc0000,
	MAGISTER_SKU_END = 0xcffff,
	MAGMA_SKU_START = 0xd0000,
	MAGMA_SKU_END = 0xdffff,
};

const char *get_wifi_sar_cbfs_filename(void)
{
	uint32_t sku_id = google_chromeec_get_board_sku();

	if (sku_id >= MAGOLOR_SKU_START && sku_id <= MAGLIA_SKU_END)
		return "wifi_sar-magolor.hex";
	if (sku_id >= MAGISTER_SKU_START && sku_id <= MAGISTER_SKU_END)
		return "wifi_sar-magister.hex";
	if (sku_id >= MAGMA_SKU_START && sku_id <= MAGMA_SKU_END)
		return "wifi_sar-magma.hex";

	return WIFI_SAR_CBFS_DEFAULT_FILENAME;
}

const char *mainboard_vbt_filename(void)
{
	if (fw_config_probe(FW_CONFIG(DB_PORTS, DB_PORTS_1A_HDMI)))
		return "vbt_magister.bin";

	return "vbt.bin";
}
