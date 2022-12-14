/* SPDX-License-Identifier: GPL-2.0-only */

#include <fw_config.h>
#include <sar.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	if (fw_config_probe(FW_CONFIG(WIFI_SAR_ID, SAR_ID_0)))
		return "wifi_sar_0.hex";

	return NULL;
}
