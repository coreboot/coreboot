/* SPDX-License-Identifier: GPL-2.0-only */

#include <sar.h>
#include <chip.h>
#include <fw_config.h>
#include <baseboard/variants.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	return "wifi_sar_0.hex";
}

void variant_update_soc_chip_config(struct soc_intel_alderlake_config *config)
{
	if (fw_config_probe(FW_CONFIG(DB_LTE, LTE_USB))) {
		config->ext_fivr_settings.configure_ext_fivr = 1;
		config->ext_fivr_settings.v1p05_enable_bitmap = FIVR_ENABLE_ALL_SX;
		config->ext_fivr_settings.vnn_enable_bitmap = FIVR_ENABLE_ALL_SX;
		config->ext_fivr_settings.v1p05_supported_voltage_bitmap = FIVR_VOLTAGE_NORMAL |
								FIVR_VOLTAGE_MIN_ACTIVE |
								FIVR_VOLTAGE_MIN_RETENTION;
		config->ext_fivr_settings.vnn_supported_voltage_bitmap = FIVR_VOLTAGE_NORMAL |
								FIVR_VOLTAGE_MIN_ACTIVE |
								FIVR_VOLTAGE_MIN_RETENTION;
		config->ext_fivr_settings.v1p05_icc_max_ma = 500;
		config->ext_fivr_settings.vnn_sx_voltage_mv = 1250;
	}
}
