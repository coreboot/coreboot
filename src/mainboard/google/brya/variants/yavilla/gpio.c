/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <soc/gpio.h>

/* Pad configuration in ramstage */
static const struct pad_config override_gpio_table[] = {
	/* A8  : WWAN_RF_DISABLE_ODL */
	PAD_CFG_GPO(GPP_A8, 1, DEEP),
	/* A18 : NC ==> HDMI_HPD_SRC */
	PAD_CFG_NF(GPP_A18, NONE, DEEP, NF1),
	/* A21 : GPP_A21 ==> USB_C1_AUX_DC_P */
	PAD_CFG_GPO(GPP_A21, 0, DEEP),
	/* A22 : GPP_A22 ==> USB_C1_AUX_DC_N */
	PAD_CFG_GPO(GPP_A22, 1, DEEP),

	/* D6  : WWAN_EN */
	PAD_CFG_GPO(GPP_D6, 1, DEEP),
	/* D8  : SD_CLKREQ_ODL ==> NC  */
	PAD_NC(GPP_D8, NONE),

	/* F6  : CNV_PA_BLANKING ==> NC */
	PAD_NC(GPP_F6, NONE),
	/* F12 : WWAN_RST_ODL */
	PAD_CFG_GPO_LOCK(GPP_F12, 1, LOCK_CONFIG),
	/* F23 : V1P05EXT_CTRL ==> NC */
	PAD_NC(GPP_F23, NONE),

	/* H8  : CNV_MFUART2_RXD ==> NC */
	PAD_NC(GPP_H8, NONE),
	/* H9  : CNV_MFUART2_TXD ==> NC */
	PAD_NC(GPP_H9, NONE),
	/* H12 : SD_PERST_L ==> NC */
	PAD_NC(GPP_H12, NONE),
	/* H13 : EN_PP3300_SD_X ==> NC */
	PAD_NC(GPP_H13, NONE),
	/* H15 : HDMI_SRC_SCL */
	PAD_CFG_NF(GPP_H15, NONE, DEEP, NF1),
	/* H17 : HDMI_SRC_SDA */
	PAD_CFG_NF(GPP_H17, NONE, DEEP, NF1),
	/* H19 : SRCCLKREQ4# ==> NC */
	PAD_NC(GPP_H19, NONE),
	/* H23 : WWAN_SAR_DETECT_ODL */
	PAD_CFG_GPO(GPP_H23, 1, DEEP),

};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* D6  : WWAN_EN */
	PAD_CFG_GPO(GPP_D6, 0, DEEP),
	/* H20 : IMGCLKOUT1 ==> WLAN_PERST_L */
	PAD_CFG_GPO(GPP_H20, 0, DEEP),
	/* A13 : GPP_A13 ==> GSC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),
	/* E12 : THC0_SPI1_IO1 ==> SOC_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_E12, NONE, DEEP),
	/* F18 : THC1_SPI2_INT# ==> EC_IN_RW_OD */
	PAD_CFG_GPI(GPP_F18, NONE, DEEP),
	/* H4  : I2C0_SDA ==> SOC_I2C_GSC_SDA */
	PAD_CFG_NF(GPP_H4, NONE, DEEP, NF1),
	/* H5  : I2C0_SCL ==> SOC_I2C_GSC_SCL */
	PAD_CFG_NF(GPP_H5, NONE, DEEP, NF1),
	/* H10 : UART0_RXD ==> UART_SOC_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* H11 : UART0_TXD ==> UART_SOC_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
	/* B11 : PMCALERT# ==> EN_PP3300_WLAN_X */
	PAD_CFG_GPO(GPP_B11, 1, DEEP),
	/* F12 : WWAN_RST_ODL */
	PAD_CFG_GPO(GPP_F12, 0, DEEP),
};

static const struct pad_config romstage_gpio_table[] = {
	/* Enable touchscreen, hold in reset */
	/* C0  : SMBCLK ==> EN_PP3300_TCHSCR */
	PAD_CFG_GPO(GPP_C0, 1, DEEP),
	/* C1  : SMBDATA ==> USI_RST_L */
	PAD_CFG_GPO(GPP_C1, 0, DEEP),

	/* H20 : IMGCLKOUT1 ==> WLAN_PERST_L */
	PAD_CFG_GPO(GPP_H20, 1, DEEP),
};

const struct pad_config *variant_gpio_override_table(size_t *num)
{
	*num = ARRAY_SIZE(override_gpio_table);
	return override_gpio_table;
}

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

const struct pad_config *variant_romstage_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(romstage_gpio_table);
	return romstage_gpio_table;
}
