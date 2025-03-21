/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <fw_config.h>

void variant_update_soc_chip_config(struct soc_intel_pantherlake_config *config)
{
	if (fw_config_probe(FW_CONFIG(WIFI, WIFI_CNVI_6)) || /* CNVi Wi-Fi */
	    fw_config_probe(FW_CONFIG(WIFI, WIFI_CNVI_7))) {
		config->cnvi_wifi_core = true;
		config->cnvi_bt_core = true;
	}
}
