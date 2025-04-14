/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <baseboard/variants.h>
#include <fw_config.h>
#include <sar.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	return get_wifi_sar_fw_config_filename(FW_CONFIG_FIELD(WIFI));
}

void variant_update_soc_chip_config(struct soc_intel_pantherlake_config *config)
{
	config->cnvi_wifi_core = false;
	config->cnvi_bt_core = false;
	/* CNVi */
	if (fw_config_probe(FW_CONFIG(WIFI, WIFI_CNVI_6)) ||
			fw_config_probe(FW_CONFIG(WIFI, WIFI_CNVI_7))) {
		config->cnvi_wifi_core = true;
		config->cnvi_bt_core = true;

		if (fw_config_probe(FW_CONFIG(AUDIO, AUDIO_ALC721_SNDW))) {
			printk(BIOS_INFO, "BT audio offload configured.\n");
			config->cnvi_bt_audio_offload = true;
		}
	}
}
