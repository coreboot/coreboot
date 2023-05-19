/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <fw_config.h>
#include <baseboard/variants.h>
#include <baseboard/gpio.h>
#include <sar.h>
#include <delay.h>

#define FM350_RST_DEALY_MS 20
#define FM350_PERST_DEALY_MS 30

static const struct pad_config fm350_rst_pad[] = {
	/* F12 : WWAN_RST_L */
	PAD_CFG_GPO_LOCK(GPP_F12, 1, LOCK_CONFIG),
};
static const struct pad_config fm350_perst_pad[] = {
	/* H21 : WWAN_PERST */
	PAD_CFG_GPO(GPP_H21, 1, DEEP),
};

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
	return get_wifi_sar_fw_config_filename(FW_CONFIG_FIELD(WIFI_SAR_ID));
}

void variant_init(void)
{
	if (fw_config_probe(FW_CONFIG(WWAN_5G, WWAN_5G_PRESENT))) {
		/*
		 * FM350 power on seuqence:
		 * De-assert WWAN_EN -> 20ms -> de-assert WWAN_RST -> 30ms ->
		 * de-assert WWAN_PERST
		 * WWAN_EN is de-asserted in ramstage gpio configuration, de-assert
		 * WWAN_RST and WWAN_PERST here.
		 */
		mdelay(FM350_RST_DEALY_MS);
		gpio_configure_pads(fm350_rst_pad, ARRAY_SIZE(fm350_rst_pad));
		mdelay(FM350_PERST_DEALY_MS);
		gpio_configure_pads(fm350_perst_pad, ARRAY_SIZE(fm350_perst_pad));
	}
}
