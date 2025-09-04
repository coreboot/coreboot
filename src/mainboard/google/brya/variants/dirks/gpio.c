/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <intelblocks/early_graphics.h>
#include <soc/gpio.h>

/* Pad configuration in ramstage for dirks */
static const struct pad_config override_gpio_table[] = {
	/* A11 : EN_SPK_PA ==> NC */
	PAD_NC(GPP_A11, NONE),
	/* A14 : USB_OC1# ==> USB_A1_OC_ODL */
	PAD_CFG_NF(GPP_A14, NONE, DEEP, NF1),
	/* A15 : USB_OC2# ==> USB_A2_OC_ODL */
	PAD_CFG_NF(GPP_A15, NONE, DEEP, NF1),
	/* A16 : USB_OC3# ==> USB_A3_OC_ODL */
	PAD_CFG_NF(GPP_A16, NONE, DEEP, NF1),
	/* A18 : NC ==> HDMI1_HPD_SUB_ODL*/
	PAD_CFG_NF(GPP_A18, NONE, DEEP, NF1),

	/* B4 : LAN_PERST_L */
	PAD_CFG_GPO(GPP_B4, 1, PLTRST),
	/* B16 : I2C5_SDA ==> NC */
	PAD_NC(GPP_B16, NONE),
	/* B17 : I2C5_SCL ==> NC */
	PAD_NC(GPP_B17, NONE),

	/* C0  : SMBCLK ==> NC */
	PAD_NC(GPP_C0, NONE),
	/* C1  : SMBDATA ==> NC */
	PAD_NC(GPP_C1, NONE),
	/* C3  : SML0CLK ==> NC */
	PAD_NC(GPP_C3, NONE),
	/* C6  : SML1CLK ==> NC */
	PAD_NC(GPP_C6, NONE),
	/* C7  : SML1DATA ==> NC */
	PAD_NC(GPP_C7, NONE),

	/* D2  :  PWM_PP3300_BUZZER */
	PAD_CFG_GPO(GPP_D2, 1, DEEP),
	/* D3  : ISH_GP3 ==> NC */
	PAD_NC(GPP_D3, NONE),
	/* D6  : SRCCLKREQ1# ==> NC */
	PAD_NC(GPP_D6, NONE),
	/* D15  : ISH_UART0_RTS# ==> NC */
	PAD_NC(GPP_D15, NONE),
	/* D16  : ISH_UART0_CTS# ==> NC */
	PAD_NC(GPP_D16, NONE),

	/* E5 : [] ==> USB_A4_RT_RST_ODL */
	PAD_CFG_GPO(GPP_E5, 1, DEEP),
	/* E9 : USB_OC0# ==> USB_A0_OC_ODL */
	PAD_CFG_NF(GPP_E9, NONE, DEEP, NF1),
	/* E22 : DDPA_CTRLCLK ==> DDPA_CTRLCLK */
	PAD_CFG_NF(GPP_E22, NONE, DEEP, NF1),
	/* E23 : DDPA_CTRLDATA ==> DDPA_CTRLDATA */
	PAD_CFG_NF(GPP_E23, NONE, DEEP, NF1),

	/* F13 : GSXSLOAD ==> NC */
	PAD_NC(GPP_F13, NONE),
	/* F14 : GSXDIN ==> LAN_WAKE_ODL */
	PAD_CFG_GPI_SCI_LOW(GPP_F14, NONE, DEEP, EDGE_SINGLE),
	/* F15 : GSXSRESET# ==> NC */
	PAD_NC(GPP_F15, NONE),

	/* H6  : I2C1_SDA ==> NC */
	PAD_NC(GPP_H6, NONE),
	/* H7  : I2C1_SCL ==> NC */
	PAD_NC(GPP_H7, NONE),
	/* H8  : CNV_MFUART2_RXD ==> NC */
	PAD_NC(GPP_H8, NONE),
	/* H9  : CNV_MFUART2_TXD ==> NC */
	PAD_NC(GPP_H9, NONE),
	/* H12 : UART0_RTS# ==> NC */
	PAD_NC(GPP_H12, NONE),
	/* H13 : UART0_CTS# ==> NC */
	PAD_NC(GPP_H13, NONE),
	/* H15 : HDMI_SRC_DDC_SCL */
	PAD_CFG_NF(GPP_H15, NONE, DEEP, NF1),
	/* H17 : HDMI_SRC_DDC_SDA */
	PAD_CFG_NF(GPP_H17, NONE, DEEP, NF1),
	/* H21 : IMGCLKOUT2==> LAN_PE_ISOLATE_ODL */
	PAD_CFG_GPO(GPP_H21, 1, DEEP),
	/* H22 : IMGCLKOUT3 ==> NC */
	PAD_NC(GPP_H22, NONE),

	/* R4 : DMIC_CLK_A_0A ==> NC */
	PAD_NC(GPP_R4, NONE),
	/* R5 : DMIC_DATA_0A ==> NC */
	PAD_NC(GPP_R5, NONE),
	/* R6 : DMIC_CLK_A_1A ==> NC */
	PAD_NC(GPP_R6, NONE),
	/* R7 : DMIC_DATA_1A ==> NC */
	PAD_NC(GPP_R7, NONE),

	/* S0 : I2S1_SCLK ==> NC */
	PAD_NC(GPP_S0, NONE),
	/* S1 : I2S1_SFRM ==> NC */
	PAD_NC(GPP_S1, NONE),
	/* S2 : I2S1_TXD ==> NC */
	PAD_NC(GPP_S2, NONE),

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
	/* H21 : IMGCLKOUT2==> LAN_PE_ISOLATE_ODL */
	PAD_CFG_GPO(GPP_H21, 1, DEEP),
	/* GPP_B4 : [] ==> LAN_PERST_L */
	PAD_CFG_GPO(GPP_B4, 0, DEEP),
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
};

static const struct pad_config romstage_gpio_table[] = {

	/* H20 : IMGCLKOUT1 ==> WLAN_PERST_L */
	PAD_CFG_GPO(GPP_H20, 1, DEEP),
};

const struct pad_config early_graphics_gpio_table[] = {
	/* A18 : NC ==> HDMI2_HPD*/
	PAD_CFG_NF(GPP_A18, NONE, DEEP, NF1),
	/* A20 : DDSP_HPD2 ==> EC_SOC_HDMI_HPD */
	PAD_CFG_NF(GPP_A20, NONE, DEEP, NF1),

	/* E14 : DDSP_HPDA ==> HDMI1_HPD */
	PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1),
	/* E20 : DDP2_CTRLCLK ==> HDMI_DDC_SCL */
	PAD_CFG_NF(GPP_E20, NONE, DEEP, NF1),
	/* E21 : DDP2_CTRLDATA ==> HDMI_DDC_SDA_STRAP */
	PAD_CFG_NF(GPP_E21, NONE, DEEP, NF1),
	/* E22 : DDPA_CTRLCLK ==> DDPA_CTRLCLK */
	PAD_CFG_NF(GPP_E22, NONE, DEEP, NF1),
	/* E23 : DDPA_CTRLDATA ==> DDPA_CTRLDATA */
	PAD_CFG_NF(GPP_E23, NONE, DEEP, NF1),

	/* H15 : HDMI_SRC_DDC_SCL */
	PAD_CFG_NF(GPP_H15, NONE, DEEP, NF1),
	/* H17 : HDMI_SRC_DDC_SDA */
	PAD_CFG_NF(GPP_H17, NONE, DEEP, NF1),
};

const struct pad_config *variant_early_graphics_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_graphics_gpio_table);
	return early_graphics_gpio_table;
}

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
