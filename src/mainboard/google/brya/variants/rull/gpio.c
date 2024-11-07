/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <soc/gpio.h>

/* Pad configuration in ramstage */
static const struct pad_config override_gpio_table[] = {
	/* A7  : NC ==> LTE_Present */
	PAD_CFG_GPI(GPP_A7, NONE, DEEP),
	/* A8  : GPP_A8 ==> NC */
	PAD_NC_LOCK(GPP_A8, NONE, LOCK_CONFIG),
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

	/* B5  : I2C2_SDA ==> NA */
	PAD_NC_LOCK(GPP_B5, NONE, LOCK_CONFIG),
	/* B6  : I2C2_SCL ==> NA */
	PAD_NC_LOCK(GPP_B6, NONE, LOCK_CONFIG),
	/* B11 : NC ==> EN_PP3300_WLAN_X*/
	PAD_CFG_GPO(GPP_B11, 1, DEEP),

	/* D11 : EN_PP1800_WCAM_X ==> EN_PP3300_SSD_X */
	PAD_CFG_GPO(GPP_D11, 1, DEEP),
	/* B4  : SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 1, DEEP),

	/* D3 : ISH_GP3 ==> NA */
	PAD_NC_LOCK(GPP_D3, NONE, LOCK_CONFIG),
	/* D6 : WWAN_PWR_ENABLE ==> PCIE_REFCLK_SSD1_REQ_N */
	PAD_CFG_NF(GPP_D6, NONE, DEEP, NF1),
	/* D8  : SRCCLKREQ3# ==> NC */
	PAD_NC_LOCK(GPP_D8, NONE, LOCK_CONFIG),

	/* D13 : EN_PP1800_WCAM_X ==> NA */
	PAD_NC_LOCK(GPP_D13, NONE, LOCK_CONFIG),
	/* D15 : EN_PP2800_WCAM_X ==> NA */
	PAD_NC_LOCK(GPP_D15, NONE, LOCK_CONFIG),
    /* D16 : EN_PP1800_PP1200_WCAM_X ==> NA */
	PAD_NC_LOCK(GPP_D16, NONE, LOCK_CONFIG),

	/* E4  : NA ==> SSD/EMMC/UFS ID */
	PAD_CFG_GPI_LOCK(GPP_E4, NONE, LOCK_CONFIG),
	/* E5  : NA ==> SSD/EMMC/UFS ID */
	PAD_CFG_GPI_LOCK(GPP_E5, NONE, LOCK_CONFIG),
	/* E20 : DDP2_CTRLCLK ==> NC */
	PAD_NC_LOCK(GPP_E20, NONE, LOCK_CONFIG),
	/* E21 : DDP2_CTRLDATA ==> GPP_E21_STRAP */
	PAD_NC_LOCK(GPP_E21, NONE, LOCK_CONFIG),

	/* F11 : WWAN_PWR_ON ==> NC */
	PAD_NC_LOCK(GPP_F11, NONE, LOCK_CONFIG),
	/* F12 : WWAN_RST_L ==> NA */
	PAD_NC_LOCK(GPP_F12, NONE, LOCK_CONFIG),
	/* F13 : SOC_PEN_DETECT_R_ODL ==> NA */
	PAD_NC_LOCK(GPP_F13, NONE, LOCK_CONFIG),
	/* F15 : SOC_PEN_DETECT_ODL ==> NA */
	PAD_NC_LOCK(GPP_F15, NONE, LOCK_CONFIG),
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
    /* H22 : WCAM_MCLK_R ==> NA */
	PAD_NC_LOCK(GPP_H22, NONE, LOCK_CONFIG),
	/* H23 : WWAN_SAR_DETECT_ODL ==> NA */
	PAD_NC_LOCK(GPP_H23, NONE, LOCK_CONFIG),
	/* H20 : IMGCLKOUT1 ==> WLAN_PERST_L */
	PAD_CFG_GPO(GPP_H20, 1, DEEP),

	/* R6 : DMIC_CLK_A_1A ==> DMIC_WCAM_CLK_R */
	PAD_CFG_NF(GPP_R6, NONE, DEEP, NF3),
	/* R7 : DMIC_DATA_1A ==> DMIC_WCAM_DATA */
	PAD_CFG_NF(GPP_R7, NONE, DEEP, NF3),

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
	/* B4  : I2C2_SDA ==> SSD1_RST_L */
	PAD_CFG_GPO(GPP_B4, 0, DEEP),
	/* D11 : EN_PP1800_WCAM_X ==> EN_PP3300_SSD_X */
	PAD_CFG_GPO(GPP_D11, 1, DEEP),

	/* A13 : GPP_A13 ==> GSC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),

    /* C0  : SMBCLK ==> EN_PP3300_TCHSCR_X */
	PAD_CFG_GPO(GPP_C0, 1, DEEP),
	/* C1  : SMBDATA ==> TCHSCR_RST_L */
	PAD_CFG_GPO(GPP_C1, 1, DEEP),

	/* E12 : THC0_SPI1_IO1 ==> SOC_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_E12, NONE, DEEP),

	/* H4  : I2C0_SDA ==> SOC_I2C_GSC_SDA */
	PAD_CFG_NF(GPP_H4, NONE, DEEP, NF1),
	/* H5  : I2C0_SCL ==> SOC_I2C_GSC_SCL */
	PAD_CFG_NF(GPP_H5, NONE, DEEP, NF1),

	/* H10 : UART0_RXD ==> UART_SOC_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* H11 : UART0_TXD ==> UART_SOC_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
    /* H20 : IMGCLKOUT1 ==> WLAN_PERST_L */
	PAD_CFG_GPO(GPP_H20, 0, DEEP),
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
