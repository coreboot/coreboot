/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <ec/google/chromeec/ec.h>
#include <smbios.h>
#include <string.h>
#include <sar.h>

#define SKU_UNKNOWN     0xFFFFFFFF

uint32_t variant_board_sku(void)
{
	static uint32_t sku_id = SKU_UNKNOWN;
	uint32_t id;

	if (sku_id != SKU_UNKNOWN)
		return sku_id;
	if (google_chromeec_cbi_get_sku_id(&id))
		return SKU_UNKNOWN;
	sku_id = id;

	return sku_id;
}

const char *smbios_system_sku(void)
{
	static char sku_str[14]; /* sku{0..4294967295} */

	snprintf(sku_str, sizeof(sku_str), "sku%u", variant_board_sku());

	return sku_str;
}

const char *get_wifi_sar_cbfs_filename(void)
{
	uint32_t sku_id;

	sku_id = variant_board_sku();
	if (sku_id & 0x200)
		return "wifi_sar-shyvana.hex";
	else
		return "wifi_sar-leona.hex";

	return WIFI_SAR_CBFS_DEFAULT_FILENAME;
}
