/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <soc/gpio.h>

/* Pad configuration in ramstage for craask */
static const struct pad_config override_gpio_table[] = {
	/* A8  : WWAN_RF_DISABLE_ODL */
	PAD_CFG_GPO(GPP_A8, 1, DEEP),
	/* A18 : NC ==> HDMI_HPD_SUB_ODL*/
	PAD_CFG_NF(GPP_A18, NONE, DEEP, NF1),
	/* D6  : WWAN_EN */
	PAD_CFG_GPO(GPP_D6, 1, DEEP),
	/* D8  : SRCCLKREQ3# ==> NC */
	PAD_NC(GPP_D8, NONE),
	/* F12 : WWAN_RST_L */
	PAD_CFG_GPO_LOCK(GPP_F12, 1, LOCK_CONFIG),
	/* H12 : UART0_RTS# ==> NC */
	PAD_NC_LOCK(GPP_H12, NONE, LOCK_CONFIG),
	/* H13 : UART0_CTS# ==> NC */
	PAD_NC_LOCK(GPP_H13, NONE, LOCK_CONFIG),
	/* H15 : HDMI_SRC_DDC_SCL */
	PAD_CFG_NF(GPP_H15, NONE, DEEP, NF1),
	/* H17 : HDMI_SRC_DDC_SDA */
	PAD_CFG_NF(GPP_H17, NONE, DEEP, NF1),
	/* H19 : SOC_I2C_SUB_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_H19, NONE, PLTRST, LEVEL, NONE),
	/* H23 : WWAN_SAR_DETECT_ODL */
	PAD_CFG_GPO(GPP_H23, 1, DEEP),

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

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* F12 : GSXDOUT ==> WWAN_RST_L */
	PAD_CFG_GPO(GPP_F12, 0, DEEP),
	/* H20 : IMGCLKOUT1 ==> WLAN_PERST_L */
	PAD_CFG_GPO(GPP_H20, 0, DEEP),
	/* A13 : GPP_A13 ==> GSC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),
	/* D6  : SRCCLKREQ1# ==> WWAN_EN */
	PAD_CFG_GPO(GPP_D6, 1, DEEP),
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
