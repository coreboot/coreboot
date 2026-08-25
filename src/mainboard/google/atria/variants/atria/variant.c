/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <fw_config.h>
#include <sar.h>
#include <soc/soc_chip.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	return get_wifi_sar_fw_config_filename(FW_CONFIG_FIELD(AP_OEM_2BIT_FIELD0));
}

void variant_update_soc_chip_config(struct soc_intel_novalake_config *config)
{
	/* CNVi */
	if (fw_config_probe(FW_CONFIG(WIFI_INTERFACE, WIFI_INTERFACE_CNVI))) {
		config->cnvi_wifi_core = true;
		config->cnvi_bt_core = true;
	}
}
