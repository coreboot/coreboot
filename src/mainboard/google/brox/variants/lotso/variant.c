/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <baseboard/variants.h>
#include <chip.h>
#include <fw_config.h>
#include <sar.h>

void variant_update_soc_chip_config(struct soc_intel_alderlake_config *config)
{
	if (fw_config_probe(FW_CONFIG(WIFI_BT, WIFI_CNVI_WIFI6E)) || !fw_config_is_provisioned()) {
		printk(BIOS_INFO, "CNVi bluetooth enabled by fw_config\n");
		config->cnvi_bt_core = true;
		config->cnvi_bt_audio_offload = true;
	}
}

const char *get_wifi_sar_cbfs_filename(void)
{
	return get_wifi_sar_fw_config_filename(FW_CONFIG_FIELD(WIFI_BT));
}

const char *variant_get_auxfw_version_file(void)
{
	if (fw_config_probe(FW_CONFIG(RETIMER, RETIMER_BYPASS)))
		return "rts5453_retimer_bypass.hash";
	else if (fw_config_probe(FW_CONFIG(RETIMER, RETIMER_JHL8040)))
		return "rts5453_retimer_jhl8040.hash";
	return NULL;
}
