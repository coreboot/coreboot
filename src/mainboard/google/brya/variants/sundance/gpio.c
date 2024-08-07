/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <soc/gpio.h>
#include <fw_config.h>

/* Pad configuration in ramstage for Sundance */
static const struct pad_config override_gpio_table[] = {
	/* A8  : WWAN_RF_DISABLE_ODL */
	PAD_CFG_GPO(GPP_A8, 1, DEEP),
	/* A20  : NC */
	PAD_NC_LOCK(GPP_A20, NONE, LOCK_CONFIG),
	/* B5  : NC */
	PAD_NC_LOCK(GPP_B5, NONE, LOCK_CONFIG),
	/* B6  : NC */
	PAD_NC_LOCK(GPP_B6, NONE, LOCK_CONFIG),
	/* C1  : SMBDATA ==> USI_RST_L */
	PAD_CFG_TERM_GPO(GPP_C1, 1, UP_20K, DEEP),
	/* D3  : WCAM_RST_L ==> NC */
	PAD_NC_LOCK(GPP_D3, NONE, LOCK_CONFIG),
	/* D6  : SRCCLKREQ1# ==> WWAN_EN */
	PAD_CFG_GPO(GPP_D6, 1, DEEP),
	/* D8  : NC */
	PAD_NC_LOCK(GPP_D8, NONE, LOCK_CONFIG),
	/* D15  : EN_PP2800_WCAM_X ==> NC */
	PAD_NC_LOCK(GPP_D15, NONE, LOCK_CONFIG),
	/* D16  : EN_PP1800_PP1200_WCAM_X ==> NC */
	PAD_NC_LOCK(GPP_D16, NONE, LOCK_CONFIG),
	/* D17 : NC ==> SD_WAKE_N */
	PAD_CFG_GPI_LOCK(GPP_D17, NONE, LOCK_CONFIG),
	/* E20  : NC */
	PAD_NC_LOCK(GPP_E20, NONE, LOCK_CONFIG),
	/* E21  : NC */
	PAD_NC_LOCK(GPP_E21, NONE, LOCK_CONFIG),
	/* F12 : WWAN_RST_L */
	PAD_CFG_GPO_LOCK(GPP_F12, 1, LOCK_CONFIG),
	/* F13  : NC */
	PAD_NC_LOCK(GPP_F13, NONE, LOCK_CONFIG),
	/* F15  : NC */
	PAD_NC_LOCK(GPP_F15, NONE, LOCK_CONFIG),
	/* H12  : NC */
	PAD_NC_LOCK(GPP_H12, NONE, LOCK_CONFIG),
	/* H13  : NC */
	PAD_NC_LOCK(GPP_H13, NONE, LOCK_CONFIG),
	/* H19  : NC */
	PAD_NC_LOCK(GPP_H19, NONE, LOCK_CONFIG),
	/* H22  : WCAM_MCLK_R ==> NC */
	PAD_NC(GPP_H22, NONE),
	/* H23 : WWAN_SAR_DETECT_ODL ==> NC */
	PAD_NC_LOCK(GPP_H23, NONE, LOCK_CONFIG),
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* A13 : GPP_A13 ==> GSC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),
	/*
	* WWAN_EN is asserted in ramstage to meet the 500 ms warm reset toff
	* requirement. WWAN_EN must be asserted before WWAN_RST_L is released
	* (with min delay 0 ms), so this works as long as the pin used for
	* WWAN_EN comes before the pin used for WWAN_RST_L.
	*/
	/* D6  : SRCCLKREQ1# ==> WWAN_EN */
	PAD_CFG_GPO(GPP_D6, 0, DEEP),
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

/* Pad configuration in romstage for Sundance */
static const struct pad_config romstage_gpio_table[] = {
	/* Enable touchscreen, hold in reset */
	/* C0  : SMBCLK ==> EN_PP3300_TCHSCR */
	PAD_CFG_GPO(GPP_C0, 1, DEEP),
	/* C1  : SMBDATA ==> USI_RST_L */
	PAD_CFG_TERM_GPO(GPP_C1, 0, UP_20K, DEEP),
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
