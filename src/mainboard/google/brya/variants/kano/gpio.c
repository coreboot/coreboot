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
	/* A12 : SATAXPCIE1 ==>  NC  */
	PAD_NC(GPP_A12, NONE),
	/* A15 : USB_OC2# ==> NC */
	PAD_NC(GPP_A15, NONE),
	/* A19 : DDSP_HPD1 ==> NC */
	PAD_NC(GPP_A19, NONE),
	/* A20 : DDSP_HPD2 ==> NC */
	PAD_NC(GPP_A20, NONE),
	/* A21 : DDPC_CTRCLK ==> NC */
	PAD_NC(GPP_A21, NONE),
	/* A22 : DDPC_CTRLDATA ==> NC */
	PAD_NC(GPP_A22, NONE),

	/* D3  : ISH_GP3 ==> NC */
	PAD_NC_LOCK(GPP_D3, NONE, LOCK_CONFIG),
	/* D5  : SRCCLKREQ0# ==> SSD_CLKREQ_ODL */
	PAD_CFG_NF(GPP_D5, NONE, DEEP, NF1),
	/* D6  : SRCCLKREQ1# ==> APU_PEN_DETECT_ODL */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_D6, NONE, PLTRST),
	/* D7  : SRCCLKREQ2# ==> NC */
	PAD_NC(GPP_D7, NONE),
	/* D8  : SRCCLKREQ3# ==> NC */
	PAD_NC(GPP_D8, NONE),
	/* D16 : ISH_UART0_CTS# ==> PEN_PWR_EN */
	PAD_CFG_GPO_LOCK(GPP_D16, 1, LOCK_CONFIG),
	/* D17  : UART1_RXD ==> APU_PEN_DETECT_ODL */
	PAD_CFG_GPI_SCI_LOCK(GPP_D17, NONE, EDGE_SINGLE, NONE, LOCK_CONFIG),
	/* D18 : UART1_TXD ==> NC */
	PAD_NC_LOCK(GPP_D18, NONE, LOCK_CONFIG),

	/* E0  : SATAXPCIE0 ==> NC */
	PAD_NC(GPP_E0, NONE),
	/* E3  : PROC_GP0 ==> SAR1_INT_L */
	PAD_CFG_GPI_APIC(GPP_E3, NONE, PLTRST, LEVEL, NONE),
	/* E7  : PROC_GP1 ==> NC */
	PAD_NC(GPP_E7, NONE),
	/* E10 : THC0_SPI1_CS# ==> NC */
	PAD_NC_LOCK(GPP_E10, NONE, LOCK_CONFIG),
	/* E17 : THC0_SPI1_INT# ==> NC */
	PAD_NC_LOCK(GPP_E17, NONE, LOCK_CONFIG),
	/* E22 : DDPA_CTRLCLK ==> NC */
	PAD_NC(GPP_E22, NONE),
	/* E23 : DDPA_CTRLDATA ==> NC */
	PAD_NC(GPP_E23, NONE),

	/* F6  : CNV_PA_BLANKING ==> NC */
	PAD_NC(GPP_F6, NONE),
	/* F21 : EXT_PWR_GATE2# ==> NC */
	PAD_NC(GPP_F21, NONE),

	/* H6  : I2C1_SDA ==> PCH_I2C_TPM_SDA */
	PAD_CFG_NF_LOCK(GPP_H6, NONE, NF1, LOCK_CONFIG),
	/* H7  : I2C1_SCL ==> PCH_I2C_TPM_SCL */
	PAD_CFG_NF_LOCK(GPP_H7, NONE, NF1, LOCK_CONFIG),
	/* H8  : I2C4_SDA ==> NC */
	PAD_NC(GPP_H8, NONE),
	/* H9  : I2C4_SCL ==> NC */
	PAD_NC(GPP_H9, NONE),
	/* H12 : I2C7_SDA ==> NC */
	PAD_NC_LOCK(GPP_H12, NONE, LOCK_CONFIG),
	/* H13 : I2C7_SCL ==> NC */
	PAD_NC_LOCK(GPP_H13, NONE, LOCK_CONFIG),
	/* H19 : SRCCLKREQ4# ==> NC */
	PAD_NC(GPP_H19, NONE),
	/* H20 : IMGCLKOUT1 ==> NC */
	PAD_NC(GPP_H20, NONE),
	/* H21 : IMGCLKOUT2 ==> NC */
	PAD_NC(GPP_H21, NONE),
	/* H22 : IMGCLKOUT3 ==> NC */
	PAD_NC(GPP_H22, NONE),
	/* H23 : SRCCLKREQ5# ==> NC */
	PAD_NC(GPP_H23, NONE),

	/* R4 : HDA_RST# ==> DMIC_CLK0_R */
	PAD_CFG_NF(GPP_R4, NONE, DEEP, NF3),
	/* R5 : HDA_SDI1 ==> DMIC_DATA0_R */
	PAD_CFG_NF(GPP_R5, NONE, DEEP, NF3),
	/* R6 : I2S2_TXD ==> DMIC_CLK1_R */
	PAD_CFG_NF(GPP_R6, NONE, DEEP, NF3),
	/* R7 : I2S2_RXD ==> DMIC_DATA1_R */
	PAD_CFG_NF(GPP_R7, NONE, DEEP, NF3),

	/* S0 : SNDW0_CLK ==> I2S1_SPKR_SCLK_R */
	PAD_CFG_NF(GPP_S0, NONE, DEEP, NF4),
	/* S1 : SNDW0_DATA ==> I2S1_SPKR_SFRM_R */
	PAD_CFG_NF(GPP_S1, NONE, DEEP, NF4),
	/* S2 : SNDW1_CLK ==> I2S1_PCH_TX_SPKR_RX_R */
	PAD_CFG_NF(GPP_S2, NONE, DEEP, NF4),
	/* S3 : SNDW1_DATA ==> I2S1_PCH_RX_SPKR_TX */
	PAD_CFG_NF(GPP_S3, NONE, DEEP, NF4),

	/* GPD11: LANPHYC ==> NC */
	PAD_NC(GPD11, NONE),
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* A13 : PMC_I2C_SCL ==> GSC_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),
	/* B4  : PROC_GP3 ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 0, DEEP),
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
	/* E0  : SATAXPCIE0 ==> NC */
	PAD_NC(GPP_E0, NONE),
	/* E13 : THC0_SPI1_IO2 ==> MEM_CH_SEL */
	PAD_CFG_GPI(GPP_E13, NONE, DEEP),
	/* E15 : RSVD_TP ==> PCH_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_E15, NONE, DEEP),
	/* F18 : THC1_SPI2_INT# ==> EC_IN_RW_OD */
	PAD_CFG_GPI(GPP_F18, NONE, DEEP),
	/* H6  : I2C1_SDA ==> PCH_I2C_TPM_SDA */
	PAD_CFG_NF(GPP_H6, NONE, DEEP, NF1),
	/* H7  : I2C1_SCL ==> PCH_I2C_TPM_SCL */
	PAD_CFG_NF(GPP_H7, NONE, DEEP, NF1),
	/* H10 : UART0_RXD ==> UART_PCH_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* H11 : UART0_TXD ==> UART_PCH_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),

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
	/* B4  : PROC_GP3 ==> SSD_PERST_L */
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
