/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <chip.h>
#include <device/device.h>
#include <fw_config.h>
#include <sar.h>

void variant_update_soc_chip_config(struct soc_intel_alderlake_config *config)
{
	if (fw_config_probe(FW_CONFIG(WIFI_BT, WIFI_BT_CNVI)) || (!fw_config_is_provisioned())) {
		printk(BIOS_INFO, "CNVi bluetooth enabled by fw_config\n");
		config->cnvi_bt_core = true;
	}
}

const char *get_wifi_sar_cbfs_filename(void)
{
	return get_wifi_sar_fw_config_filename(FW_CONFIG_FIELD(WIFI_BT));
}

const char *variant_get_auxfw_version_file(void)
{
	return NULL;
}
