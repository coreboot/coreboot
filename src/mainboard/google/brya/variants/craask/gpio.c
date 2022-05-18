/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <soc/gpio.h>

/* Pad configuration in ramstage for craask */
static const struct pad_config override_gpio_table[] = {
	/* D6  : WWAN_EN */
	PAD_CFG_GPO(GPP_D6, 1, DEEP),
	/* D7  : WLAN_CLKREQ_ODL */
	PAD_NC(GPP_D7, NONE),
	/* F12 : WWAN_RST_L */
	PAD_CFG_GPO(GPP_F12, 1, DEEP),
	/* H3  : WLAN_PCIE_WAKE_ODL */
	PAD_NC(GPP_H3, NONE),
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
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
	/* H13 : UART0_CTS# ==> EN_PP3300_SD_X */
	PAD_CFG_GPO(GPP_H13, 1, DEEP),
};

static const struct pad_config romstage_gpio_table[] = {
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
