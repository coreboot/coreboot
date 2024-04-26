/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * This header block is used to supply information to arbitrage, a
 * google-internal tool. Updating it incorrectly will lead to issues,
 * so please don't update it unless a change is specifically required.
 * BaseID: E3110FFB1FCDA587
 * Overrides: None
 */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <soc/gpio.h>

/* Pad configuration in ramstage */
static const struct pad_config override_gpio_table[] = {
	/* GPP_A18 : [NF1: DDSP_HPDB NF4: DISP_MISCB NF6: USB_C_GPP_A18] ==> NC */
	PAD_NC(GPP_A18, NONE),
	/* GPP_A19 : [NF1: DDSP_HPD1 NF4: DISP_MISC1 NF6: USB_C_GPP_A19] ==> NC */
	PAD_NC(GPP_A19, NONE),
	/* GPP_A20 : [NF1: DDSP_HPD2 NF4: DISP_MISC2 NF6: USB_C_GPP_A20] ==> NC */
	PAD_NC(GPP_A20, NONE),

	/* GPP_C0 : [NF1: SMBCLK NF6: USB_C_GPP_C0] ==> SMBCLK */
	PAD_CFG_NF(GPP_C0, NONE, DEEP, NF1),
	/* GPP_C1 : [NF1: SMBDATA NF6: USB_C_GPP_C1] ==> SMBDATA */
	PAD_CFG_NF(GPP_C1, NONE, DEEP, NF1),

	/* GPP_D9 : [NF1: ISH_SPI_CS# NF2: DDP3_CTRLCLK NF4: TBT_LSX2_TXD NF5: BSSB_LS2_RX
	 * NF6: USB_C_GPP_D9 NF7: GSPI2_CS0#] ==> NC */
	PAD_NC(GPP_D9, NONE),
	/* GPP_D10 : [NF1: ISH_SPI_CLK NF2: DDP3_CTRLDATA NF4: TBT_LSX2_RXD NF5: BSSB_LS2_TX
	 * NF6: USB_C_GPP_D10 NF7: GSPI2_CLK] ==> NC */
	PAD_NC(GPP_D10, NONE),

	/* GPP_E4 : [NF1: DEVSLP0 NF6: USB_C_GPP_E4 NF7: SRCCLK_OE9#] ==> NC */
	PAD_NC(GPP_E4, NONE),

	/* GPP_E10 : THC0_SPI1_CS_L/GSPI0_CS0_L ==> NC */
	PAD_NC(GPP_E10, NONE),
	/* GPP_E12 : THC0_SPI1_IO1/I2C0A_SDA/GSPI0_MISO ==> NC */
	PAD_NC(GPP_E12, NONE),
	/* GPP_E13 : THC0_SPI1_IO0/I2C0A_SCL/GSPI0_MOSI ==> NC */
	PAD_NC(GPP_E13, NONE),
	/* GPP_E15 : SRCCLK_OE8_L ==> NC */
	PAD_NC(GPP_E15, NONE),

	/* GPP_H15 : [NF1: DDPB_CTRLCLK NF3: PCIE_LINK_DOWN NF6: USB_C_GPP_H15] ==> NC */
	PAD_NC(GPP_H15, NONE),
	/* GPP_H17 : [NF1: DDPB_CTRLDATA NF6: USB_C_GPP_H17] ==> NC */
	PAD_NC(GPP_H17, NONE),

	/* GPP_S0 : SNDW0_CLL/I2S1_SCLK ==> NC */
	PAD_NC(GPP_S0, NONE),
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* GPP_D11 : [] ==> EN_PP3300_SSD (NC) */
	PAD_NC(GPP_D11, NONE),
	/* GPP_E2  : THC0_SPI1_IO3 ==> GSC_PCH_INT_ODL */
	PAD_CFG_GPI_APIC_LOCK(GPP_E2, NONE, LEVEL, INVERT, LOCK_CONFIG),
	/* GPP_E8 : GPP_E8 ==> PCH_WP_OD */
	PAD_CFG_GPI_LOCK(GPP_E8, NONE, LOCK_CONFIG),
	/* GPP_F9 : [NF1: BOOTMPC NF6: USB_C_GPP_F9] ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_F9, 0, DEEP),
	/* F21 : EXT_PWR_GATE2# ==> NC */
	PAD_NC(GPP_F21, NONE),
	/* GPP_H8 : [NF1: I2C4_SDA NF2: CNV_MFUART2_RXD NF6: USB_C_GPP_H8] ==> PCH_I2C_GSC_SDA */
	PAD_CFG_NF(GPP_H8, NONE, DEEP, NF1),
	/* GPP_H9 : [NF1: I2C4_SCL NF2: CNV_MFUART2_TXD] ==> PCH_I2C_GSC_SCL */
	PAD_CFG_NF(GPP_H9, NONE, DEEP, NF1),
	/* H10 : UART0_RXD ==> UART_PCH_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, PLTRST, NF2),
	/* H11 : UART0_TXD ==> UART_PCH_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, PLTRST, NF2),
	/* GPP_S0 : SNDW0_CLL/I2S1_SCLK ==> NC */
	PAD_NC(GPP_S0, NONE),

	/* CPU PCIe VGPIO for PEG60 */
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_48, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_49, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_50, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_51, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_52, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_53, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_54, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_55, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_56, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_57, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_58, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_59, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_60, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_61, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_62, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_63, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_76, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_77, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_78, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_79, NONE, PLTRST, NF1),
};

static const struct pad_config romstage_gpio_table[] = {
	/* GPP_E10 : THC0_SPI1_CS_L/GSPI0_CS0_L ==> NC */
	PAD_NC(GPP_E10, NONE),
	/* GPP_E12 : THC0_SPI1_IO1/I2C0A_SDA/GSPI0_MISO ==> NC */
	PAD_NC(GPP_E12, NONE),
	/* GPP_E13 : THC0_SPI1_IO0/I2C0A_SCL/GSPI0_MOSI ==> NC */
	PAD_NC(GPP_E13, NONE),
	/* GPP_E15 : SRCCLK_OE8_L ==> NC */
	PAD_NC(GPP_E15, NONE),
	/* GPP_F7 : [NF6: USB_C_GPP_F7] ==> EN_PP3300_TCHSCR */
	PAD_CFG_GPO(GPP_F7, 1, PLTRST),
	/* GPP_F9 : [NF1: BOOTMPC NF6: USB_C_GPP_F9] ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_F9, 1, DEEP),
	/* GPP_F17 : [NF3: THC1_SPI2_RST# NF6: USB_C_GPP_F17] ==> TCHSCR_RST_L */
	PAD_CFG_GPO(GPP_F17, 0, DEEP),
	/* GPP_S0 : SNDW0_CLL/I2S1_SCLK ==> NC */
	PAD_NC(GPP_S0, NONE),
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
