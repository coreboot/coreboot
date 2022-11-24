/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <soc/gpio.h>

/* Pad configuration in ramstage for craask */
static const struct pad_config override_gpio_table[] = {
	/* A8  : WWAN_RF_DISABLE_ODL */
	PAD_CFG_GPO(GPP_A8, 1, DEEP),
	/* B4  : SSD_PERST_L */
	PAD_CFG_GPO_LOCK(GPP_B4, 1, LOCK_CONFIG),
	/* D6  : WWAN_EN */
	PAD_CFG_GPO(GPP_D6, 1, DEEP),
	/* D7  : WLAN_CLKREQ_ODL */
	PAD_NC(GPP_D7, NONE),
	/* D11 : EN_PP3300_SSD */
	PAD_CFG_GPO_LOCK(GPP_D11, 1, LOCK_CONFIG),
	/* E17 : SSD_PLN_L */
	PAD_CFG_GPO_LOCK(GPP_E17, 1, LOCK_CONFIG),
	/* F12 : WWAN_RST_L */
	PAD_CFG_GPO_LOCK(GPP_F12, 1, LOCK_CONFIG),
	/* H3  : WLAN_PCIE_WAKE_ODL */
	PAD_NC_LOCK(GPP_H3, NONE, LOCK_CONFIG),
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
	/* B4  : SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 0, DEEP),
	/* F12 : GSXDOUT ==> WWAN_RST_L */
	PAD_CFG_GPO(GPP_F12, 0, DEEP),
	/* H12 : UART0_RTS# ==> SD_PERST_L */
	PAD_CFG_GPO(GPP_H12, 0, DEEP),
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
	/* D11 : EN_PP3300_SSD */
	PAD_CFG_GPO(GPP_D11, 1, DEEP),
	/* H13 : UART0_CTS# ==> EN_PP3300_SD_X */
	PAD_CFG_GPO(GPP_H13, 1, DEEP),
};

static const struct pad_config romstage_gpio_table[] = {
	/* B4  : SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 1, DEEP),
	/* H12 : UART0_RTS# ==> SD_PERST_L */
	PAD_CFG_GPO(GPP_H12, 1, DEEP),
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
