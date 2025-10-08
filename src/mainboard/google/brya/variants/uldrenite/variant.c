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
	/* E16 : WWAN_PCIE_WAKE_ODL */
	PAD_NC(GPP_E16, NONE),
	/* F12 : WWAN_RST_L */
	PAD_NC_LOCK(GPP_F12, NONE, LOCK_CONFIG),
	/* F13 : PLTRST_WWAN# */
	PAD_NC(GPP_F13, NONE),
	/* H23  : WWAN_EN */
	PAD_NC(GPP_H23, NONE),
};

static const struct pad_config ish_uart0_disable_pads[] = {
	/* A16 : ISH_GP5 ==> NC */
	PAD_NC_LOCK(GPP_A16, NONE, LOCK_CONFIG),
	/* B5  : GPP_B5 ==> NC */
	PAD_NC(GPP_B5, NONE),
	/* B6  : GPP_B6 ==> NC */
	PAD_NC(GPP_B6, NONE),
	/* D1  : ISH_GP1 ==> NC */
	PAD_NC(GPP_D1, NONE),
	/* D13  : UART0_ISH_RXD ==> NC */
	PAD_NC(GPP_D13, NONE),
	/* D14  : UART0_ISH_TXD ==> NC */
	PAD_NC(GPP_D14, NONE),
	/* E9  : SOC_ACC2_INT ==> NC */
	PAD_NC_LOCK(GPP_E9, NONE, LOCK_CONFIG),
};

static const struct pad_config switch_ish_uart1_pads[] = {
	/* D13  : UART0_ISH_RXD ==> NC  */
	PAD_NC(GPP_D13, NONE),
	/* D14  : UART0_ISH_TXD ==> NC */
	PAD_NC(GPP_D14, NONE),
	/* D17 : NC ==> UART1_ISH_RDX */
	PAD_CFG_NF(GPP_D17, NONE, DEEP, NF2),
	/* D18 : NC ==> UART1_ISH_TDX */
	PAD_CFG_NF(GPP_D18, NONE, DEEP, NF2),
};

static const struct pad_config ish_uart1_disable_pads[] = {
	/* A16 : ISH_GP5 ==> NC */
	PAD_NC_LOCK(GPP_A16, NONE, LOCK_CONFIG),
	/* B5  : GPP_B5 ==> NC */
	PAD_NC(GPP_B5, NONE),
	/* B6  : GPP_B6 ==> NC */
	PAD_NC(GPP_B6, NONE),
	/* D1  : ISH_GP1 ==> NC */
	PAD_NC(GPP_D1, NONE),
	/* D17 : UART1_ISH_RDX ==> NC */
	PAD_NC(GPP_D17, NONE),
	/* D18 : UART1_ISH_TDX ==> NC*/
	PAD_NC(GPP_D18, NONE),
	/* E9  : SOC_ACC2_INT ==> NC */
	PAD_NC_LOCK(GPP_E9, NONE, LOCK_CONFIG),
};

static const struct pad_config emmc_disable_pads[] = {
	/* I7  : EMMC_CMD */
	PAD_NC(GPP_I7, NONE),
	/* I8  : EMMC_D0 */
	PAD_NC(GPP_I8, NONE),
	/* I9  : EMMC_D1 */
	PAD_NC(GPP_I9, NONE),
	/* I10 : EMMC_D2 */
	PAD_NC(GPP_I10, NONE),
	/* I11 : EMMC_D3 */
	PAD_NC(GPP_I11, NONE),
	/* I12 : EMMC_D4 */
	PAD_NC(GPP_I12, NONE),
	/* I13 : EMMC_D5 */
	PAD_NC(GPP_I13, NONE),
	/* I14 : EMMC_D6 */
	PAD_NC(GPP_I14, NONE),
	/* I15 : EMMC_D7 */
	PAD_NC(GPP_I15, NONE),
	/* I16 : EMMC_RCLK */
	PAD_NC(GPP_I16, NONE),
	/* I17 : EMMC_CLK */
	PAD_NC(GPP_I17, NONE),
	/* I18 : EMMC_RST_L */
	PAD_NC(GPP_I18, NONE),
};

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	uint32_t board_version = board_id();

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
	if (fw_config_probe(FW_CONFIG(STORAGE, STORAGE_UFS))) {
		printk(BIOS_INFO, "Disable eMMC GPIO pins.\n");
		gpio_padbased_override(padbased_table, emmc_disable_pads,
				ARRAY_SIZE(emmc_disable_pads));
	}

	/* b/415605630: Support different ISH UART mappings according the board id */
	if (fw_config_probe(FW_CONFIG(SERIES, SERIES_ULDRENITE)) && board_version < 2) {
		/* Override ISH UART0 to ISH UART1 */
		gpio_padbased_override(padbased_table, switch_ish_uart1_pads,
			ARRAY_SIZE(switch_ish_uart1_pads));

		if (!fw_config_probe(FW_CONFIG(ISH, ISH_ENABLE))) {
			printk(BIOS_INFO, "Disable ISH GPIO pins is based on ISH UART1.\n");
			gpio_padbased_override(padbased_table, ish_uart1_disable_pads,
				ARRAY_SIZE(ish_uart1_disable_pads));
		}
	} else {
		if (!fw_config_probe(FW_CONFIG(ISH, ISH_ENABLE))) {
			printk(BIOS_INFO, "Disable ISH GPIO pins is based on ISH UART0.\n");
			gpio_padbased_override(padbased_table, ish_uart0_disable_pads,
				ARRAY_SIZE(ish_uart0_disable_pads));
		}
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
	if (fw_config_probe(FW_CONFIG(SERIES, SERIES_ULDRENITE)) && board_version != 1)
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
