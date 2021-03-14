/* SPDX-License-Identifier: GPL-2.0-only */

#include <fw_config.h>
#include <sar.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	if (fw_config_probe(FW_CONFIG(TABLETMODE, TABLETMODE_ENABLED)))
		return "wifi_sar-magolor.hex";

	return WIFI_SAR_CBFS_DEFAULT_FILENAME;
}
