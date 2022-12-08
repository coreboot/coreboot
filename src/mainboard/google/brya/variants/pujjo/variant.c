/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <fw_config.h>
#include <baseboard/variants.h>
#include <sar.h>

void variant_update_soc_chip_config(struct soc_intel_alderlake_config *config)
{
	// Configure external V1P05/Vnn/VnnSx Rails for Pujjo, Pujjoflex
	if (fw_config_probe(FW_CONFIG(EXT_VR, EXT_VR_PRESENT))) {
		config->ext_fivr_settings.configure_ext_fivr = 1;

		config->ext_fivr_settings.v1p05_enable_bitmap =
				FIVR_ENABLE_ALL_SX & ~FIVR_ENABLE_S0;

		config->ext_fivr_settings.vnn_enable_bitmap =
				FIVR_ENABLE_ALL_SX;

		config->ext_fivr_settings.vnn_sx_enable_bitmap =
				FIVR_ENABLE_ALL_SX;

		config->ext_fivr_settings.v1p05_supported_voltage_bitmap =
				FIVR_VOLTAGE_NORMAL;

		config->ext_fivr_settings.vnn_supported_voltage_bitmap =
				FIVR_VOLTAGE_MIN_ACTIVE;

		config->ext_fivr_settings.v1p05_voltage_mv = 1050;
		config->ext_fivr_settings.vnn_voltage_mv = 780;
		config->ext_fivr_settings.vnn_sx_voltage_mv = 1050;
		config->ext_fivr_settings.v1p05_icc_max_ma = 500;
		config->ext_fivr_settings.vnn_icc_max_ma = 500;
	}
}

const char *get_wifi_sar_cbfs_filename(void)
{
	if (fw_config_probe(FW_CONFIG(WIFI_SAR_ID, WIFI_SAR_TABLE_0))) {
		printk(BIOS_INFO, "Use wifi_sar_0.hex.\n");
		return "wifi_sar_0.hex";
	}

	else if (fw_config_probe(FW_CONFIG(WIFI_SAR_ID, WIFI_SAR_TABLE_1))) {
		printk(BIOS_INFO, "Use wifi_sar_1.hex.\n");
		return "wifi_sar_1.hex";
	}

	else if (fw_config_probe(FW_CONFIG(WIFI_SAR_ID, WIFI_SAR_TABLE_2))) {
		printk(BIOS_INFO, "Use wifi_sar_2.hex.\n");
		return "wifi_sar_2.hex";
	}

	printk(BIOS_INFO, "Intel Wi-Fi SAR not used, return NULL!\n");
	return NULL;
}
