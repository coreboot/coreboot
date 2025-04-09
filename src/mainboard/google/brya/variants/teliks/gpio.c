/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <soc/gpio.h>

/* Pad configuration in ramstage */
static const struct pad_config override_gpio_table[] = {
	/* A7  : NC ==> LTE_Present */
	PAD_CFG_GPI(GPP_A7, NONE, DEEP),
	/* A8  : GPP_A8 ==> WWAN_RF_DISABLE_ODL */
	PAD_CFG_GPO(GPP_A8, 1, DEEP),
	/* A11 : GPP_A11 ==> EN_SPK_PA */
	PAD_CFG_GPO(GPP_A11, 0, DEEP),
	/* A18 : NC ==> HDMI_HPD_SRC*/
	PAD_CFG_NF(GPP_A18, NONE, DEEP, NF1),

	/* A20 : DDSP_HPD2 ==> NC */
	PAD_NC_LOCK(GPP_A20, NONE, LOCK_CONFIG),
	/* A21 : GPP_A21 ==> NC */
	PAD_NC_LOCK(GPP_A21, NONE, LOCK_CONFIG),
	/* A22 : GPP_A22 ==> NC */
	PAD_NC_LOCK(GPP_A22, NONE, LOCK_CONFIG),

	/* B5  : I2C2_SDA ==> MIPI_WCAM_SDA */
	PAD_CFG_NF_LOCK(GPP_B5, NONE, NF2, LOCK_CONFIG),
	/* B6  : I2C2_SCL ==> MIPI_WCAM_SCL */
	PAD_CFG_NF_LOCK(GPP_B6, NONE, NF2, LOCK_CONFIG),

	/* B11 : NC ==> EN_PP3300_WLAN_X*/
	PAD_CFG_GPO(GPP_B11, 0, DEEP),

	/* D6 : NC ==> WWAN_PWR_ENABLE */
	PAD_CFG_GPO(GPP_D6, 1, DEEP),
	/* D8  : SRCCLKREQ3# ==> NC */
	PAD_NC_LOCK(GPP_D8, NONE, LOCK_CONFIG),
	/* D13 : NC ==> EN_PP1800_WCAM_X */
	PAD_CFG_GPO_LOCK(GPP_D13, 1, LOCK_CONFIG),

	/* E20 : DDP2_CTRLCLK ==> NC */
	PAD_NC_LOCK(GPP_E20, NONE, LOCK_CONFIG),
	/* E21 : DDP2_CTRLDATA ==> GPP_E21_STRAP */
	PAD_NC_LOCK(GPP_E21, NONE, LOCK_CONFIG),

	/* H22 : IMGCLKOUT3 ==> WCAM_MCLK_R */
	PAD_CFG_NF(GPP_H22, NONE, DEEP, NF1),
	/* H23 : WWAN_SAR_DETECT_ODL */
	PAD_CFG_GPO(GPP_H23, 1, DEEP),

	/* F11 : NC ==> WWAN_PWR_ON */
	PAD_CFG_GPO_LOCK(GPP_F11, 1, LOCK_CONFIG),
	/* F12 : GSXDOUT ==> WWAN_RST_L */
	PAD_CFG_GPO_LOCK(GPP_F12, 1, LOCK_CONFIG),
	/* F15 : GSXSRESET# ==> SOC_PEN_DETECT_ODL */
	PAD_CFG_GPI_SCI_HIGH(GPP_F15, NONE, PLTRST, EDGE_BOTH),
	/* F18 : THC1_SPI2_INT# ==> EN_PP2800_AFVDD */
	PAD_CFG_GPO(GPP_F18, 0, DEEP),
	/* F23 : V1P05_CTRL ==> NC*/
	PAD_NC_LOCK(GPP_F23, NONE, LOCK_CONFIG),


	/* H12 : UART0_RTS# ==> NC*/
	PAD_NC_LOCK(GPP_H12, NONE, LOCK_CONFIG),
	/* H13 : UART0_CTS# ==> NC */
	PAD_NC_LOCK(GPP_H13, NONE, LOCK_CONFIG),
	/* H15 : DDPB_CTRLCLK ==> HDMI_DDC_SCL */
	PAD_CFG_NF(GPP_H15, NONE, DEEP, NF1),
	/* H17 : DDPB_CTRLDATA ==> HDMI_DDC_SDA */
	PAD_CFG_NF(GPP_H17, NONE, DEEP, NF1),

	/* R6 : DMIC_CLK_A_1A ==> NC */
	PAD_NC_LOCK(GPP_R6, NONE, LOCK_CONFIG),
	/* R7 : DMIC_DATA_1A ==> NC */
	PAD_NC_LOCK(GPP_R7, NONE, LOCK_CONFIG),

	/* E19 : DDP1_CTRLDATA ==> GPP_E19_STRAP */
	PAD_CFG_GPI_LOCK(GPP_E19, DN_20K, LOCK_CONFIG),

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
	/* C0  : SMBCLK ==> EN_PP3300_TCHSCR_X */
	PAD_CFG_GPO(GPP_C0, 1, DEEP),
	/* C1  : SMBDATA ==> TCHSCR_RST_L */
	PAD_CFG_GPO(GPP_C1, 1, DEEP),

	/* H20 : IMGCLKOUT1 ==> WLAN_PERST_L */
	PAD_CFG_GPO(GPP_H20, 0, DEEP),
	/* A13 : GPP_A13 ==> GSC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),
	/* E12 : THC0_SPI1_IO1 ==> SOC_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_E12, NONE, DEEP),

	/* F11 : NC ==> WWAN_PWR_ON */
	PAD_CFG_GPO(GPP_F11, 1, DEEP),
	/* F12 : GSXDOUT ==> WWAN_RST_L */
	PAD_CFG_GPO(GPP_F12, 0, DEEP),
	/* D6 : NC ==> WWAN_PWR_ENABLE */
	PAD_CFG_GPO(GPP_D6, 1, DEEP),

	/* H4  : I2C0_SDA ==> SOC_I2C_GSC_SDA */
	PAD_CFG_NF(GPP_H4, NONE, DEEP, NF1),
	/* H5  : I2C0_SCL ==> SOC_I2C_GSC_SCL */
	PAD_CFG_NF(GPP_H5, NONE, DEEP, NF1),

	/* H10 : UART0_RXD ==> UART_SOC_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* H11 : UART0_TXD ==> UART_SOC_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
	/* E19 : DDP1_CTRLDATA ==> GPP_E19_STRAP */
	PAD_CFG_GPI_LOCK(GPP_E19, DN_20K, LOCK_CONFIG),
};

static const struct pad_config romstage_gpio_table[] = {
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
