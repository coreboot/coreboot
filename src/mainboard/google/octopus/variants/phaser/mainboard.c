/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <ec/google/chromeec/ec.h>
#include <sar.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	uint32_t sku_id;

	if (google_chromeec_cbi_get_sku_id(&sku_id) || (sku_id != 5))
		return WIFI_SAR_CBFS_DEFAULT_FILENAME;

	return "wifi_sar-laser.hex";
}
