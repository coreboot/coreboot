/* SPDX-License-Identifier: GPL-2.0-only */

#include <fw_config.h>
#include <sar.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	const char *filename = NULL;

	if (fw_config_probe(FW_CONFIG(WIFI_SAR_ID, WIFI_SAR_ID_0)))
		filename = "wifi_sar_0.hex";
	else if (fw_config_probe(FW_CONFIG(WIFI_SAR_ID, WIFI_SAR_ID_1)))
		filename = "wifi_sar_1.hex";

	return filename;
}
