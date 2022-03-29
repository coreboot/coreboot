/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <ec/google/chromeec/ec.h>

static const struct pad_config ssd_sku_gpio_table[] = {
	/* A0  : SAR0_INT_ODL */
	PAD_CFG_GPI_INT(GPP_A0, NONE, PLTRST, LEVEL),
	/* A6  : SAR1_INT_ODL */
	PAD_CFG_GPI_INT(GPP_A6, NONE, PLTRST, LEVEL),
	/* A8  : PEN_GARAGE_DET_L (wake) */
	PAD_CFG_GPI_SCI(GPP_A8, NONE, DEEP, EDGE_SINGLE, NONE),
	/* A10 : FPMCU_PCH_BOOT1 */
	PAD_CFG_GPO(GPP_A10, 0, DEEP),
	/* A11 : PCH_SPI_FPMCU_CS_L */
	PAD_CFG_NF(GPP_A11, NONE, DEEP, NF2),
	/* A12 : FPMCU_RST_ODL */
	PAD_CFG_GPO(GPP_A12, 0, DEEP),
	/* C15 : WWAN_DPR_SAR_ODL
	 *
	 * TODO: Driver doesn't use this pin as of now. In case driver starts
	 * using this pin, expose this pin to driver.
	 */
	PAD_CFG_GPO(GPP_C15, 1, DEEP),
	/* D9  : EN_PP3300_DX_TOUCHSCREEN */
	PAD_CFG_GPO(GPP_D9, 1, DEEP),
	/* D15 : TOUCHSCREEN_RST_L */
	PAD_CFG_GPO(GPP_D15, 1, DEEP),
	/* F3  : MEM_STRAP_3 */
	PAD_CFG_GPI(GPP_F3, NONE, PLTRST),
	/* F10 : MEM_STRAP_2 */
	PAD_CFG_GPI(GPP_F10, NONE, PLTRST),
	/* F11 : EMMC_CMD ==> NC */
	PAD_NC(GPP_F11, NONE),
	/* F12 : EMMC_DATA0 ==> NC */
	PAD_NC(GPP_F12, NONE),
	/* F13 : EMMC_DATA1 ==> NC */
	PAD_NC(GPP_F13, NONE),
	/* F14 : EMMC_DATA2 ==> NC */
	PAD_NC(GPP_F14, NONE),
	/* F15 : EMMC_DATA3 ==> NC */
	PAD_NC(GPP_F15, NONE),
	/* F16 : EMMC_DATA4 ==> NC */
	PAD_NC(GPP_F16, NONE),
	/* F17 : EMMC_DATA5 ==> NC */
	PAD_NC(GPP_F17, NONE),
	/* F18 : EMMC_DATA6 ==> NC */
	PAD_NC(GPP_F18, NONE),
	/* F19 : EMMC_DATA7 ==> NC */
	PAD_NC(GPP_F19, NONE),
	/* F20 : EMMC_RCLK ==> NC */
	PAD_NC(GPP_F20, NONE),
	/* F21 : EMMC_CLK ==> NC */
	PAD_NC(GPP_F21, NONE),
	/* F22 : EMMC_RESET# ==> NC */
	PAD_NC(GPP_F22, NONE),
	/* H3  : SPKR_PA_EN */
	PAD_CFG_GPO(GPP_H3, 0, DEEP),
	/* H19 : MEM_STRAP_0 */
	PAD_CFG_GPI(GPP_H19, NONE, PLTRST),
	/* H22 : MEM_STRAP_1 */
	PAD_CFG_GPI(GPP_H22, NONE, PLTRST),
};

static const struct pad_config emmc_sku_gpio_table[] = {
	/* A0  : SAR0_INT_ODL */
	PAD_CFG_GPI_INT(GPP_A0, NONE, PLTRST, LEVEL),
	/* A6  : SAR1_INT_ODL */
	PAD_CFG_GPI_INT(GPP_A6, NONE, PLTRST, LEVEL),
	/* A8  : PEN_GARAGE_DET_L (wake) */
	PAD_CFG_GPI_SCI(GPP_A8, NONE, DEEP, EDGE_SINGLE, NONE),
	/* A10 : FPMCU_PCH_BOOT1 */
	PAD_CFG_GPO(GPP_A10, 0, DEEP),
	/* A11 : PCH_SPI_FPMCU_CS_L */
	PAD_CFG_NF(GPP_A11, NONE, DEEP, NF2),
	/* A12 : FPMCU_RST_ODL */
	PAD_CFG_GPO(GPP_A12, 0, DEEP),
	/* D9  : EN_PP3300_DX_TOUCHSCREEN */
	PAD_CFG_GPO(GPP_D9, 1, DEEP),
	/* D15 : TOUCHSCREEN_RST_L */
	PAD_CFG_GPO(GPP_D15, 1, DEEP),
	/* E1  : M2_SSD_PEDET ==> NC */
	PAD_NC(GPP_E1, NONE),
	/* E4  : M2_SSD_PE_WAKE_ODL ==> NC */
	PAD_NC(GPP_E4, NONE),
	/* E5  : SATA_DEVSLP1 ==> NC */
	PAD_NC(GPP_E5, NONE),
	/* C15 : WWAN_DPR_SAR_ODL
	 *
	 * TODO: Driver doesn't use this pin as of now. In case driver starts
	 * using this pin, expose this pin to driver.
	 */
	PAD_CFG_GPO(GPP_C15, 1, DEEP),
	/* F3  : MEM_STRAP_3 */
	PAD_CFG_GPI(GPP_F3, NONE, PLTRST),
	/* F10 : MEM_STRAP_2 */
	PAD_CFG_GPI(GPP_F10, NONE, PLTRST),
	/* F11 : EMMC_CMD ==> EMMC_CMD */
	PAD_CFG_NF(GPP_F11, NONE, DEEP, NF1),
	/* F12 : EMMC_DATA0 ==> EMMC_DAT0 */
	PAD_CFG_NF(GPP_F12, NONE, DEEP, NF1),
	/* F13 : EMMC_DATA1 ==> EMMC_DAT1 */
	PAD_CFG_NF(GPP_F13, NONE, DEEP, NF1),
	/* F14 : EMMC_DATA2 ==> EMMC_DAT2 */
	PAD_CFG_NF(GPP_F14, NONE, DEEP, NF1),
	/* F15 : EMMC_DATA3 ==> EMMC_DAT3 */
	PAD_CFG_NF(GPP_F15, NONE, DEEP, NF1),
	/* F16 : EMMC_DATA4 ==> EMMC_DAT4 */
	PAD_CFG_NF(GPP_F16, NONE, DEEP, NF1),
	/* F17 : EMMC_DATA5 ==> EMMC_DAT5 */
	PAD_CFG_NF(GPP_F17, NONE, DEEP, NF1),
	/* F18 : EMMC_DATA6 ==> EMMC_DAT6 */
	PAD_CFG_NF(GPP_F18, NONE, DEEP, NF1),
	/* F19 : EMMC_DATA7 ==> EMMC_DAT7 */
	PAD_CFG_NF(GPP_F19, NONE, DEEP, NF1),
	/* F20 : EMMC_RCLK ==> EMMC_RCLK */
	PAD_CFG_NF(GPP_F20, NONE, DEEP, NF1),
	/* F21 : EMMC_CLK ==> EMMC_CLK */
	PAD_CFG_NF(GPP_F21, NONE, DEEP, NF1),
	/* F22 : EMMC_RESET# ==> EMMC_RST_L */
	PAD_CFG_NF(GPP_F22, NONE, DEEP, NF1),
	/* H3  : SPKR_PA_EN */
	PAD_CFG_GPO(GPP_H3, 0, DEEP),
	/* H19 : MEM_STRAP_0 */
	PAD_CFG_GPI(GPP_H19, NONE, PLTRST),
	/* H22 : MEM_STRAP_1 */
	PAD_CFG_GPI(GPP_H22, NONE, PLTRST),
};

static const struct pad_config gpio_table[] = {
	/* A0  : SAR0_INT_ODL */
	PAD_CFG_GPI_INT(GPP_A0, NONE, PLTRST, LEVEL),
	/* A6  : SAR1_INT_ODL */
	PAD_CFG_GPI_INT(GPP_A6, NONE, PLTRST, LEVEL),
	/* A8  : PEN_GARAGE_DET_L (wake) */
	PAD_CFG_GPI_SCI(GPP_A8, NONE, DEEP, EDGE_SINGLE, NONE),
	/* A10 : FPMCU_PCH_BOOT1 */
	PAD_CFG_GPO(GPP_A10, 0, DEEP),
	/* A11 : PCH_SPI_FPMCU_CS_L */
	PAD_CFG_NF(GPP_A11, NONE, DEEP, NF2),
	/* A12 : FPMCU_RST_ODL */
	PAD_CFG_GPO(GPP_A12, 0, DEEP),
	/* D9  : EN_PP3300_DX_TOUCHSCREEN */
	PAD_CFG_GPO(GPP_D9, 1, DEEP),
	/* D15 : TOUCHSCREEN_RST_L */
	PAD_CFG_GPO(GPP_D15, 1, DEEP),
	/* C15 : WWAN_DPR_SAR_ODL
	 *
	 * TODO: Driver doesn't use this pin as of now. In case driver starts
	 * using this pin, expose this pin to driver.
	 */
	PAD_CFG_GPO(GPP_C15, 1, DEEP),
	/* F3  : MEM_STRAP_3 */
	PAD_CFG_GPI(GPP_F3, NONE, PLTRST),
	/* F10 : MEM_STRAP_2 */
	PAD_CFG_GPI(GPP_F10, NONE, PLTRST),
	/* F11 : EMMC_CMD ==> EMMC_CMD */
	PAD_CFG_NF(GPP_F11, NONE, DEEP, NF1),
	/* F12 : EMMC_DATA0 ==> EMMC_DAT0 */
	PAD_CFG_NF(GPP_F12, NONE, DEEP, NF1),
	/* F13 : EMMC_DATA1 ==> EMMC_DAT1 */
	PAD_CFG_NF(GPP_F13, NONE, DEEP, NF1),
	/* F14 : EMMC_DATA2 ==> EMMC_DAT2 */
	PAD_CFG_NF(GPP_F14, NONE, DEEP, NF1),
	/* F15 : EMMC_DATA3 ==> EMMC_DAT3 */
	PAD_CFG_NF(GPP_F15, NONE, DEEP, NF1),
	/* F16 : EMMC_DATA4 ==> EMMC_DAT4 */
	PAD_CFG_NF(GPP_F16, NONE, DEEP, NF1),
	/* F17 : EMMC_DATA5 ==> EMMC_DAT5 */
	PAD_CFG_NF(GPP_F17, NONE, DEEP, NF1),
	/* F18 : EMMC_DATA6 ==> EMMC_DAT6 */
	PAD_CFG_NF(GPP_F18, NONE, DEEP, NF1),
	/* F19 : EMMC_DATA7 ==> EMMC_DAT7 */
	PAD_CFG_NF(GPP_F19, NONE, DEEP, NF1),
	/* F20 : EMMC_RCLK ==> EMMC_RCLK */
	PAD_CFG_NF(GPP_F20, NONE, DEEP, NF1),
	/* F21 : EMMC_CLK ==> EMMC_CLK */
	PAD_CFG_NF(GPP_F21, NONE, DEEP, NF1),
	/* F22 : EMMC_RESET# ==> EMMC_RST_L */
	PAD_CFG_NF(GPP_F22, NONE, DEEP, NF1),
	/* H3  : SPKR_PA_EN */
	PAD_CFG_GPO(GPP_H3, 0, DEEP),
	/* H19 : MEM_STRAP_0 */
	PAD_CFG_GPI(GPP_H19, NONE, PLTRST),
	/* H22 : MEM_STRAP_1 */
	PAD_CFG_GPI(GPP_H22, NONE, PLTRST),
};

const struct pad_config *override_gpio_table(size_t *num)
{
	uint32_t sku_id = google_chromeec_get_board_sku();
	/* For SSD SKU */
	if (sku_id == 1 || sku_id == 3 || sku_id == 23 || sku_id == 24) {
		*num = ARRAY_SIZE(ssd_sku_gpio_table);
		return ssd_sku_gpio_table;
	}
	/* For eMMC SKU */
	if (sku_id == 2 || sku_id == 4 || sku_id == 21 || sku_id == 22) {
		*num = ARRAY_SIZE(emmc_sku_gpio_table);
		return emmc_sku_gpio_table;
	}
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}

/*
 * GPIOs configured before ramstage
 * Note: the Hatch platform's romstage will configure
 * the MEM_STRAP_* (a.k.a GPIO_MEM_CONFIG_*) pins
 * as inputs before it reads them, so they are not
 * needed in this table.
 */
static const struct pad_config early_gpio_table[] = {
	/* B15 : H1_SLAVE_SPI_CS_L */
	PAD_CFG_NF(GPP_B15, NONE, DEEP, NF1),
	/* B16 : H1_SLAVE_SPI_CLK */
	PAD_CFG_NF(GPP_B16, NONE, DEEP, NF1),
	/* B17 : H1_SLAVE_SPI_MISO_R */
	PAD_CFG_NF(GPP_B17, NONE, DEEP, NF1),
	/* B18 : H1_SLAVE_SPI_MOSI_R */
	PAD_CFG_NF(GPP_B18, NONE, DEEP, NF1),
	/* C8  : UART_PCH_RX_DEBUG_TX */
	PAD_CFG_NF(GPP_C8, NONE, DEEP, NF1),
	/* C9  : UART_PCH_TX_DEBUG_RX */
	PAD_CFG_NF(GPP_C9, NONE, DEEP, NF1),
	/* C14 : BT_DISABLE_L */
	PAD_CFG_GPO(GPP_C14, 0, DEEP),
	/* PCH_WP_OD */
	PAD_CFG_GPI(GPP_C20, NONE, DEEP),
	/* C21 : H1_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_C21, NONE, PLTRST, LEVEL, INVERT),
	/* C22 : EC_IN_RW_OD */
	PAD_CFG_GPI(GPP_C22, NONE, DEEP),
	/* C23 : WLAN_PE_RST# */
	PAD_CFG_GPO(GPP_C23, 1, DEEP),
	/* E1  : M2_SSD_PEDET */
	PAD_CFG_NF(GPP_E1, NONE, DEEP, NF1),
	/* E5  : SATA_DEVSLP1 */
	PAD_CFG_NF(GPP_E5, NONE, PLTRST, NF1),
	/* F2  : MEM_CH_SEL */
	PAD_CFG_GPI(GPP_F2, NONE, PLTRST),
};

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

/* GPIOs needed to be set in romstage. */
static const struct pad_config romstage_gpio_table[] = {
	/* Enable touchscreen, hold in reset */
	/* D9  : EN_PP3300_DX_TOUCHSCREEN */
	PAD_CFG_GPO(GPP_D9, 1, DEEP),
	/* D15 : TOUCHSCREEN_RST_L */
	PAD_CFG_GPO(GPP_D15, 0, DEEP),
};

const struct pad_config *variant_romstage_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(romstage_gpio_table);
	return romstage_gpio_table;
}
