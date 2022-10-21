/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <fw_config.h>
#include <sar.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	if (fw_config_probe(FW_CONFIG(WIFI_SAR_ID, ID_0))) {
		printk(BIOS_INFO, "Use wifi_sar_0.hex.\n");
		return "wifi_sar_0.hex";
	}

	else if (fw_config_probe(FW_CONFIG(WIFI_SAR_ID, UNUSED))) {
		printk(BIOS_INFO, "Wi-Fi SAR not used, return NULL!\n");
	}

	return NULL;
}
