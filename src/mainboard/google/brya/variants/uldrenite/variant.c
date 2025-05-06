/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <boardid.h>
#include <console/console.h>
#include <delay.h>
#include <fw_config.h>
#include <sar.h>
#include <soc/bootblock.h>
#include <stdlib.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	return "wifi_sar_0.hex";
}

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

static const struct pad_config ish_disable_pads[] = {
	/* A16 : ISH_GP5 ==> NC */
	PAD_NC_LOCK(GPP_A16, NONE, LOCK_CONFIG),
	/* B5  : GPP_B5 ==> NC */
	PAD_NC(GPP_B5, NONE),
	/* B6  : GPP_B6 ==> NC */
	PAD_NC(GPP_B6, NONE),
	/* D1  : ISH_GP1 ==> NC */
	PAD_NC(GPP_D1, NONE),
	/* E9  : SOC_ACC2_INT ==> NC */
	PAD_NC_LOCK(GPP_E9, NONE, LOCK_CONFIG),
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
	if (!fw_config_probe(FW_CONFIG(ISH, ISH_ENABLE))) {
		printk(BIOS_INFO, "Disable ISH GPIO pins.\n");
		gpio_padbased_override(padbased_table, ish_disable_pads,
				ARRAY_SIZE(ish_disable_pads));
	}
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

void variant_update_descriptor(void)
{
	uint32_t board_version = board_id();

	/* b/404126972: Only this phase has M/B with both FIVR and MBVR. */
	if (board_version != 1)
		return;

	/* VccanaVrLocation = "VCCANA is CPU FIVR" */
	struct descriptor_byte fivr_bytes[] = {
		{ 0xc33,  0x32 }
	};

	/* VccanaVrLocation = "VCCANA is Platform VR" */
	struct descriptor_byte mbvr_bytes[] = {
		{ 0xc33,  0x72 }
	};

	if (fw_config_probe(FW_CONFIG(EXT_VR, EXT_VR_PRESENT))) {
		printk(BIOS_INFO, "Configuring descriptor for MBVR\n");
		configure_descriptor(mbvr_bytes, ARRAY_SIZE(mbvr_bytes));
	} else {
		printk(BIOS_INFO, "Configuring descriptor for FIVR\n");
		configure_descriptor(fivr_bytes, ARRAY_SIZE(fivr_bytes));
	}
}

void variant_configure_pads(void)
{
	const struct pad_config *base_pads;
	struct pad_config *padbased_table;
	size_t base_num;

	padbased_table = new_padbased_table();
	base_pads = variant_gpio_table(&base_num);
	gpio_padbased_override(padbased_table, base_pads, base_num);
	fw_config_gpio_padbased_override(padbased_table);
	gpio_configure_pads_with_padbased(padbased_table);
	free(padbased_table);
}
