/* SPDX-License-Identifier: GPL-2.0-only */

#include <sar.h>
#include <fw_config.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	if (fw_config_probe(FW_CONFIG(WIFI_SAR_TABLE, OMNIGUL_WIFI_SAR_0)))
		return "wifi_sar_0.hex";
	if (fw_config_probe(FW_CONFIG(WIFI_SAR_TABLE, OMNIKNIGHT_WIFI_SAR_1)))
		return "wifi_sar_1.hex";

	return NULL;
}
