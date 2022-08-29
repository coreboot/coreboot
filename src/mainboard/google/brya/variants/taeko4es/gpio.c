/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <soc/gpio.h>

/* Pad configuration in ramstage */
static const struct pad_config override_gpio_table[] = {
	/* A6  : ESPI_ALERT1# ==> NC */
	PAD_NC(GPP_A6, NONE),
	/* A7  : SRCCLK_OE7# ==> NC */
	PAD_NC(GPP_A7, NONE),
	/* A8  : SRCCLKREQ7# ==> NC */
	PAD_NC(GPP_A8, NONE),
	/* A12 : SATAXPCIE1 ==> NC */
	PAD_NC(GPP_A12, NONE),
	/* A14 : USB_OC1# ==> NC */
	PAD_NC(GPP_A14, NONE),
	/* A15 : USB_OC2# ==> NC */
	PAD_NC(GPP_A15, NONE),
	/* A18 : DDSP_HPDB ==> NC */
	PAD_NC(GPP_A18, NONE),
	/* A19 : DDSP_HPD1 ==> NC */
	PAD_NC(GPP_A19, NONE),
	/* A20 : DDSP_HPD2 ==> NC */
	PAD_NC(GPP_A20, NONE),
	/* A21 : DDPC_CTRCLK ==> NC */
	PAD_NC(GPP_A21, NONE),
	/* A22 : DDPC_CTRLDATA ==> NC */
	PAD_NC(GPP_A22, NONE),

	/* B2  : VRALERT# ==> NC */
	PAD_NC(GPP_B2, NONE),
	/* B3  : PROC_GP2 ==> NC */
	PAD_NC_LOCK(GPP_B3, NONE, LOCK_CONFIG),
	/* B7  : ISH_12C1_SDA ==> PCH_I2C_TPM_SDA */
	PAD_CFG_NF_LOCK(GPP_B7, NONE, NF2, LOCK_CONFIG),
	/* B8  : ISH_12C1_SCL ==> PCH_I2C_TPM_SCL */
	PAD_CFG_NF_LOCK(GPP_B8, NONE, NF2, LOCK_CONFIG),
	/* B15 : TIME_SYNC0 ==> NC */
	PAD_NC_LOCK(GPP_B15, NONE, LOCK_CONFIG),

	/* C3 : SML0CLK ==> NC */
	PAD_NC(GPP_C3, NONE),
	/* C4 : SML0DATA ==> NC */
	PAD_NC(GPP_C4, NONE),
	/* C6  : SML1CLK ==> NC */
	PAD_NC(GPP_C6, NONE),

	/* D3  : ISH_GP3 ==> NC */
	PAD_NC_LOCK(GPP_D3, NONE, LOCK_CONFIG),
	/* D5  : SRCCLKREQ0# ==> SSD_CLKREQ_ODL */
	PAD_CFG_NF(GPP_D5, NONE, DEEP, NF1),
	/* D9  : ISH_SPI_CS# ==> NC */
	PAD_NC_LOCK(GPP_D9, NONE, LOCK_CONFIG),
	/* D10 : ISH_SPI_CLK ==> NC */
	PAD_NC_LOCK(GPP_D10, NONE, LOCK_CONFIG),
	/* D13 : ISH_UART0_RXD ==> NC */
	PAD_NC_LOCK(GPP_D13, NONE, LOCK_CONFIG),
	/* D14 : ISH_UART0_TXD ==> NC */
	PAD_NC_LOCK(GPP_D14, NONE, LOCK_CONFIG),
	/* D15 : ISH_UART0_RTS# ==> NC */
	PAD_NC_LOCK(GPP_D15, NONE, LOCK_CONFIG),
	/* D16 : ISH_UART0_CTS# ==> NC */
	PAD_NC_LOCK(GPP_D16, NONE, LOCK_CONFIG),
	/* D17 : UART1_RXD ==> NC */
	PAD_NC_LOCK(GPP_D17, NONE, LOCK_CONFIG),

	/* E0  : SATAXPCIE0 ==> NC */
	PAD_NC(GPP_E0, NONE),
	/* E4  : SATA_DEVSLP0 ==> NC */
	PAD_NC(GPP_E4, NONE),
	/* E5  : SATA_DEVSLP1 ==> NC */
	PAD_NC(GPP_E5, NONE),
	/* E10 : THC0_SPI1_CS# ==> NC */
	PAD_NC_LOCK(GPP_E10, NONE, LOCK_CONFIG),
	/* E16 : RSVD_TP ==> NC */
	PAD_NC(GPP_E16, NONE),
	/* E17 : THC0_SPI1_INT# ==> NC */
	PAD_NC_LOCK(GPP_E17, NONE, LOCK_CONFIG),
	/* E18 : DDP1_CTRLCLK ==> NC */
	PAD_NC(GPP_E18, NONE),
	/* E19 : DDP1_CTRLDATA ==> NC */
	PAD_NC(GPP_E19, NONE),
	/* E20 : DDP2_CTRLCLK ==> NC */
	PAD_NC(GPP_E20, NONE),
	/* E21 : DDP2_CTRLDATA ==> NC */
	PAD_NC(GPP_E21, NONE),

	/* F6  : CNV_PA_BLANKING ==> NC */
	PAD_NC(GPP_F6, NONE),
	/* F19 : SRCCLKREQ6# ==> NC */
	PAD_NC(GPP_F19, NONE),
	/* F20 : EXT_PWR_GATE# ==> NC */
	PAD_NC(GPP_F20, NONE),
	/* F21 : EXT_PWR_GATE2# ==> NC */
	PAD_NC(GPP_F21, NONE),
	/* F22 : VNN_CTRL ==> VNN_CTRL */
	PAD_CFG_NF(GPP_F22, NONE, DEEP, NF1),
	/* F23 : BP105_CTRL ==> PP1050_CTRL */
	PAD_CFG_NF(GPP_F23, NONE, DEEP, NF1),

	/* H8  : I2C4_SDA ==> NC */
	PAD_NC(GPP_H8, NONE),
	/* H9  : I2C4_SCL ==> NC */
	PAD_NC(GPP_H9, NONE),
	/* H13 : I2C7_SCL ==> EN_PP3300_SD */
	PAD_CFG_GPO_LOCK(GPP_H13, 1, LOCK_CONFIG),
	/* H15 : DDPB_CTRLCLK ==> NC */
	PAD_NC(GPP_H15, NONE),
	/* H17 : DDPB_CTRLDATA ==> NC */
	PAD_NC(GPP_H17, NONE),
	/* H19 : SRCCLKREQ4# ==> NC */
	PAD_NC(GPP_H19, NONE),
	/* H21 : IMGCLKOUT2 ==>  NC */
	PAD_NC(GPP_H21, NONE),
	/* H22 : IMGCLKOUT3 ==> NC */
	PAD_NC(GPP_H22, NONE),
	/* H23 : SRCCLKREQ5# ==> NC */
	PAD_NC(GPP_H23, NONE),

	/* R7 : I2S2_RXD ==> NC */
	PAD_NC(GPP_R7, NONE),

	/* S0 : SNDW0_CLK ==> NC */
	PAD_NC(GPP_S0, NONE),
	/* S1 : SNDW0_DATA ==> NC */
	PAD_NC(GPP_S1, NONE),
	/* S4 : SNDW2_CLK ==> NC */
	PAD_NC(GPP_S4, NONE),
	/* S5 : SNDW2_DATA ==> NC */
	PAD_NC(GPP_S5, NONE),
	/* S6 : SNDW3_CLK ==> NC */
	PAD_NC(GPP_S6, NONE),
	/* S7 : SNDW3_DATA ==> NC */
	PAD_NC(GPP_S7, NONE),

	/* GPD11: LANPHYC ==> WWAN_CONFIG1 */
	PAD_NC(GPD11, NONE),
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* A13 : PMC_I2C_SCL ==> GSC_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),
	/* B4  : PROC_GP3 ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 0, DEEP),
	/* B7  : ISH_12C1_SDA ==> PCH_I2C_TPM_SDA */
	PAD_CFG_NF(GPP_B7, NONE, DEEP, NF2),
	/* B8  : ISH_12C1_SCL ==> PCH_I2C_TPM_SCL */
	PAD_CFG_NF(GPP_B8, NONE, DEEP, NF2),
	/*
	 * D1  : ISH_GP1 ==> FP_RST_ODL
	 * FP_RST_ODL comes out of reset as hi-z and does not have an external pull-down.
	 * To ensure proper power sequencing for the FPMCU device, reset signal is driven low
	 * early on in bootblock, followed by enabling of power. Reset signal is deasserted
	 * later on in ramstage. Since reset signal is asserted in bootblock, it results in
	 * FPMCU not working after a S3 resume. This is a known issue.
	 */
	PAD_CFG_GPO(GPP_D1, 0, DEEP),
	/* D2  : ISH_GP2 ==> EN_FP_PWR */
	PAD_CFG_GPO(GPP_D2, 1, DEEP),
	/* D11 : ISH_SPI_MISO ==> EN_PP3300_SSD */
	PAD_CFG_GPO(GPP_D11, 1, DEEP),
	/* E13 : THC0_SPI1_IO2 ==> MEM_CH_SEL */
	PAD_CFG_GPI(GPP_E13, NONE, DEEP),
	/* E15 : RSVD_TP ==> PCH_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_E15, NONE, DEEP),
	/* F18 : THC1_SPI2_INT# ==> EC_IN_RW_OD */
	PAD_CFG_GPI(GPP_F18, NONE, DEEP),
	/* H10 : UART0_RXD ==> UART_PCH_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* H11 : UART0_TXD ==> UART_PCH_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
	/* H13 : I2C7_SCL ==> EN_PP3300_SD */
	PAD_CFG_GPO(GPP_H13, 1, DEEP),
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
	/*
	 * B4  : PROC_GP3 ==> SSD_PERST_L
	 * B4 is programmed here so that it is sequenced after EN_PP3300_SSD.
	 */
	PAD_CFG_GPO(GPP_B4, 1, DEEP),

	/* D1  : ISH_GP1 ==> FP_RST_ODL */
	PAD_CFG_GPO(GPP_D1, 0, DEEP),
	/* D2  : ISH_GP2 ==> EN_FP_PWR */
	PAD_CFG_GPO(GPP_D2, 0, DEEP),
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
