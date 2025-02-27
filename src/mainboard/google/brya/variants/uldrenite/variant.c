/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <delay.h>
#include <fw_config.h>
#include <sar.h>

#define RW350R_RST_DELAY_MS 20
#define RW350R_PERST_DELAY_MS 30

const char *get_wifi_sar_cbfs_filename(void)
{
	return "wifi_sar_0.hex";
}

static const struct pad_config rw350r_en_pad[] = {
	/* H23 : LTE_PWR_OFF_EN */
	PAD_CFG_GPO(GPP_H23, 1, DEEP),
};
static const struct pad_config rw350r_rst_pad[] = {
	/* F12 : WWAN_RST_L */
	PAD_CFG_GPO_LOCK(GPP_F12, 1, LOCK_CONFIG),
};
static const struct pad_config rw350r_perst_pad[] = {
	/* F13 : PLTRST_WWAN# */
	PAD_CFG_GPO(GPP_F13, 1, DEEP),
};

static const struct pad_config touchscreen_disable_pads[] = {
	/* A21  : TCHSCR_REPORT_EN */
	PAD_NC(GPP_A21, NONE),
	/* B16  : SOC_I2C_TCHSCR_SDA */
	PAD_NC(GPP_B16, NONE),
	/* B17  : SOC_I2C_TCHSCR_SCL */
	PAD_NC(GPP_B17, NONE),
	/* D15  : TCHSCR_RST_L */
	PAD_NC(GPP_D15, NONE),
	/* D16  : TCHSCR_INT_ODL */
	PAD_NC(GPP_D16, NONE),
	/* E17  : EN_PP3300_TCHSCR_X */
	PAD_NC(GPP_E17, NONE),
};

static const struct pad_config lte_disable_pads[] = {
	/* A8  : WWAN_RF_DISABLE_ODL */
	PAD_NC(GPP_A8, NONE),
	/* F12 : WWAN_RST_L */
	PAD_NC_LOCK(GPP_F12, NONE, LOCK_CONFIG),
	/* F13 : PLTRST_WWAN# */
	PAD_NC(GPP_F13, NONE),
	/* H23  : WWAN_EN */
	PAD_NC(GPP_H23, NONE),
};

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	if (fw_config_probe(FW_CONFIG(TOUCHSCREEN, TOUCHSCREEN_NONE))) {
		printk(BIOS_INFO, "Disable touchscreen GPIO pins.\n");
		gpio_padbased_override(padbased_table, touchscreen_disable_pads,
				ARRAY_SIZE(touchscreen_disable_pads));
	}
	if (fw_config_probe(FW_CONFIG(DB_CELLULAR, CELLULAR_ABSENT))) {
		printk(BIOS_INFO, "Disable Cellular GPIO pins.\n");
		gpio_padbased_override(padbased_table, lte_disable_pads,
				ARRAY_SIZE(lte_disable_pads));
	}
}

void variant_init(void)
{
	if (fw_config_probe(FW_CONFIG(DB_CELLULAR, CELLULAR_ABSENT)))
		return;
	/*
	 * RW350R power on seuqence:
	 * De-assert WWAN_EN -> 20ms -> de-assert WWAN_RST -> 30ms ->
	 * de-assert WWAN_PERST
	 */
	gpio_configure_pads(rw350r_en_pad, ARRAY_SIZE(rw350r_en_pad));
	mdelay(RW350R_RST_DELAY_MS);
	gpio_configure_pads(rw350r_rst_pad, ARRAY_SIZE(rw350r_rst_pad));
	mdelay(RW350R_PERST_DELAY_MS);
	gpio_configure_pads(rw350r_perst_pad, ARRAY_SIZE(rw350r_perst_pad));
}

void variant_update_soc_chip_config(struct soc_intel_alderlake_config *config)
{
	if (fw_config_probe(FW_CONFIG(EXT_VR, EXT_VR_PRESENT))) {
		/* # Configure external V1P05/Vnn/VnnSx Rails */
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
		printk(BIOS_INFO, "Configured External FIVR\n");
	}
}
