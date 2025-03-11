/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <soc/gpio.h>

/* Pad configuration in ramstage */
static const struct pad_config override_gpio_table[] = {
	/* A14 : USB_C0_AUX_DC_P */
	PAD_CFG_NF(GPP_A14, NONE, DEEP, NF6),
	/* A15 : USB_C0_AUX_DC_N */
	PAD_CFG_NF(GPP_A15, NONE, DEEP, NF6),

	/* B11 : PMCALERT# ==> EN_PP3300_WLAN */
	PAD_CFG_GPO(GPP_B11, 1, DEEP),

	/* D6  : WWAN_EN ==> NC */
	PAD_NC(GPP_D6, NONE),
	/* D7  : WLAN_CLKREQ_ODL ==> NC */
	PAD_NC(GPP_D7, NONE),
	/* D8  : SD_CLKREQ_ODL ==> NC */
	PAD_NC(GPP_D8, NONE),

	/* E7 : NC ==> GPP_E7_STRAP */
	PAD_CFG_GPI_LOCK(GPP_E7, NONE, LOCK_CONFIG),
	/* E14 : EDP_HPD ==> NC */
	PAD_NC(GPP_E14, NONE),
	/* E20 : HDMI_DDC_SCL ==> NC */
	PAD_NC(GPP_E20, NONE),
	/* E21 : DDP2_CTRLDATA ==> HDMI_DDC_SDA_STRAP */
	PAD_CFG_NF(GPP_E21, NONE, DEEP, NF1),
	/* E22 : DDPA_CTRLCLK ==> LCD_RST_N - used for MIPI Power seq. */
	PAD_CFG_NF(GPP_E22, NONE, DEEP, NF1),
	/* E23 : NC */
	PAD_NC(GPP_E23, NONE),

	/* F6  : NC */
	PAD_NC(GPP_F6, NONE),
	/* F12 : GSXDOUT ==> EMR_INT_ODL */
	PAD_CFG_GPI_INT(GPP_F12, NONE, PLTRST, LEVEL),
	/* F13 : SOC_PEN_DETECT_R_ODL ==> NC */
	PAD_NC(GPP_F13, NONE),
	/* F15 : SOC_PEN_DETECT_OEL ==> NC */
	PAD_NC(GPP_F15, NONE),
	/* F16 : GSXCLK ==> EMR_RESET_L */
	PAD_CFG_GPO(GPP_F16, 0, DEEP),

	/* H3  : NC */
	PAD_NC(GPP_H3, NONE),
	/* H8  : I2C4_SDA ==> SOC_I2C_EMR_SDA */
	PAD_CFG_NF(GPP_H8, NONE, DEEP, NF1),
	/* H9  : I2C4_SCL ==> SOC_I2C_EMR_SCL */
	PAD_CFG_NF(GPP_H9, NONE, DEEP, NF1),
	/* H12 : SD_PERST_L ==> NC */
	PAD_NC(GPP_H12, NONE),
	/* H19 : NC */
	PAD_NC(GPP_H19, NONE),
	/* H20 : NC */
	PAD_NC(GPP_H20, NONE),

	/* Configure the virtual CNVi Bluetooth I2S GPIO pads */
	/* BT_I2S_BCLK */
	PAD_CFG_NF(GPP_VGPIO_30, NONE, DEEP, NF3),
	/* BT_I2S_SYNC */
	PAD_CFG_NF(GPP_VGPIO_31, NONE, DEEP, NF3),
	/* BT_I2S_SDO */
	PAD_CFG_NF(GPP_VGPIO_32, NONE, DEEP, NF3),
	/* BT_I2S_SDI */
	PAD_CFG_NF(GPP_VGPIO_33, NONE, DEEP, NF3),
	/* SSP2_SCLK */
	PAD_CFG_NF(GPP_VGPIO_34, NONE, DEEP, NF1),
	/* SSP2_SFRM */
	PAD_CFG_NF(GPP_VGPIO_35, NONE, DEEP, NF1),
	/* SSP_TXD */
	PAD_CFG_NF(GPP_VGPIO_36, NONE, DEEP, NF1),
	/* SSP_RXD */
	PAD_CFG_NF(GPP_VGPIO_37, NONE, DEEP, NF1),
};

/* Early pad configuration in bootblock for pirrha */
static const struct pad_config early_gpio_table[] = {
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
	/* H13 : UART0_CTS# ==> EN_PP3300_SD_X */
	PAD_CFG_GPO(GPP_H13, 1, DEEP),
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
