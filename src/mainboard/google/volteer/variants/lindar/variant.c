/* SPDX-License-Identifier: GPL-2.0-only */

#include <fw_config.h>
#include <sar.h>
#include <drivers/intel/gma/opregion.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	return get_wifi_sar_fw_config_filename(FW_CONFIG_FIELD(WIFI_SAR_ID));
}

const char *mainboard_vbt_filename(void)
{
	if (fw_config_probe(FW_CONFIG(OLED_SCREEN, OLED_PRESENT)))
		return "vbt-oled.bin";
	return "vbt.bin";
}
