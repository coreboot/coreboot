/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <soc/gpio.h>

/* Pad configuration in ramstage */
static const struct pad_config override_gpio_table[] = {
	/* A6  : ESPI_ALERT1# ==> NC */
	PAD_NC(GPP_A6, NONE),
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
	PAD_NC(GPP_B3, NONE),
	/* B15  : PROC_GP3 ==> AUD_RST_L */
	PAD_CFG_GPO(GPP_B15, 1, PWROK),

	/* C3  : GPP_C3 ==> SML0_SMBCLK */
	PAD_CFG_NF(GPP_C3, NONE, DEEP, NF1),
	/* C4  : GPP_C4 ==> SML0_SMBDATA */
	PAD_CFG_NF(GPP_C4, NONE, DEEP, NF1),

	/* D3  : ISH_GP3 ==> NC */
	PAD_NC(GPP_D3, NONE),
	/* D5  : SRCCLKREQ0# ==> SSD_CLKREQ_ODL */
	PAD_CFG_NF(GPP_D5, NONE, DEEP, NF1),
	/* D13 : ISH_UART0_RXD ==> NC */
	PAD_NC(GPP_D13, NONE),
	/* D14 : ISH_UART0_TXD ==> NC */
	PAD_NC(GPP_D14, NONE),
	/* D15 : ISH_UART0_RTS# ==> NC */
	PAD_NC(GPP_D15, NONE),
	/* D16 : ISH_UART0_CTS# ==> EN_WCAM_PWR */
	PAD_CFG_GPO_LOCK(GPP_D16, 1, LOCK_CONFIG),
	/* D19 : I2S_MCLK1_OUT ==> NC */
	PAD_NC(GPP_D19, NONE),

	/* E3  : PROC_GP0 ==> NC */
	PAD_NC(GPP_E3, NONE),
	/* E5  : SATA_DEVSLP1 ==> NC */
	PAD_NC(GPP_E5, NONE),
	/* E7  : PROC_GP1 ==> NC */
	PAD_NC(GPP_E7, NONE),
	/* E20 : DDP2_CTRLCLK ==> NC */
	PAD_NC(GPP_E20, NONE),
	/* E21 : DDP2_CTRLDATA ==> NC */
	PAD_NC(GPP_E21, NONE),
	/* E22 : DDPA_CTRLCLK ==> NC */
	PAD_NC(GPP_E22, NONE),
	/* E23 : DDPA_CTRLDATA ==> NC */
	PAD_NC(GPP_E23, NONE),

	/* F19 : SRCCLKREQ6# ==> NC */
	PAD_NC(GPP_F19, NONE),
	/* F20 : EXT_PWR_GATE# ==> NC */
	PAD_NC(GPP_F20, NONE),

	/* H21 : IMGCLKOUT2 ==> VPRO_STRAP */
	PAD_CFG_GPI(GPP_H21, NONE, DEEP),
	/* H22 : IMGCLKOUT3 ==> NC */
	PAD_NC(GPP_H22, NONE),

	/* R4 : HDA_RST# ==> DMIC_CLK0_R */
	PAD_CFG_NF(GPP_R4, NONE, DEEP, NF3),
	/* R5 : HDA_SDI1 ==> DMIC_DATA0_R */
	PAD_CFG_NF(GPP_R5, NONE, DEEP, NF3),
	/* R6 : I2S2_TXD ==> NC */
	PAD_NC(GPP_R6, NONE),
	/* R7 : I2S2_RXD ==> NC */
	PAD_NC(GPP_R7, NONE),

	/* S0 : SNDW0_CLK ==> I2S_SPKR_SCLK_R */
	PAD_CFG_NF(GPP_S0, NONE, DEEP, NF4),
	/* S1 : SNDW0_DATA ==> I2S_SPKR_SFRM_R */
	PAD_CFG_NF(GPP_S1, NONE, DEEP, NF4),
	/* S2 : SNDW1_CLK ==> I2S_PCH_TX_SPKR_RX_R */
	PAD_CFG_NF(GPP_S2, NONE, DEEP, NF4),
	/* S3 : SNDW1_DATA ==> I2S_PCH_RX_SPKR_TX */
	PAD_CFG_NF(GPP_S3, NONE, DEEP, NF4),
	/* S4 : SNDW2_CLK ==> NC */
	PAD_NC(GPP_S4, NONE),
	/* S5 : SNDW2_DATA ==> NC */
	PAD_NC(GPP_S5, NONE),
	/* S6 : SNDW3_CLK ==> NC */
	PAD_NC(GPP_S6, NONE),
	/* S7 : SNDW3_DATA ==> NC */
	PAD_NC(GPP_S7, NONE),

	/* T2 : GPP_T2 ==> eMMC_CFG */
	PAD_CFG_GPI(GPP_T2, NONE, DEEP),

	/* GPD11: LANPHYC ==> NC */
	PAD_NC(GPD11, NONE),
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* A12 : SATAXPCIE1 ==> EN_PP3300_WWAN */
	PAD_CFG_GPO(GPP_A12, 1, DEEP),
	/* A13 : PMC_I2C_SCL ==> GSC_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),
	/* B3  : PROC_GP2 ==> eMMC_PERST_L */
	PAD_CFG_GPO(GPP_B3, 0, DEEP),
	/* B4  : PROC_GP3 ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 0, DEEP),
	/* H6  : I2C1_SDA ==> PCH_I2C_TPM_SDA */
	PAD_CFG_NF(GPP_H6, NONE, DEEP, NF1),
	/* H7  : I2C1_SCL ==> PCH_I2C_TPM_SCL */
	PAD_CFG_NF(GPP_H7, NONE, DEEP, NF1),
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
	/* D18 : UART1_TXD ==> SD_PE_RST_L */
	PAD_CFG_GPO(GPP_D18, 0, PLTRST),
	/* E0  : SATAXPCIE0 ==> WWAN_PERST_L (updated in ramstage) */
	PAD_CFG_GPO(GPP_E0, 0, DEEP),
	/* E13 : THC0_SPI1_IO2 ==> MEM_CH_SEL */
	PAD_CFG_GPI(GPP_E13, NONE, DEEP),
	/* E16 : RSVD_TP ==> WWAN_RST_L (updated in ramstage) */
	PAD_CFG_GPO(GPP_E16, 0, DEEP),
	/* E15 : RSVD_TP ==> PCH_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_E15, NONE, DEEP),
	/* F18 : THC1_SPI2_INT# ==> EC_IN_RW_OD */
	PAD_CFG_GPI(GPP_F18, NONE, DEEP),
	/* F21 : EXT_PWR_GATE2# ==> WWAN_FCPO_L (updated in romstage) */
	PAD_CFG_GPO(GPP_F21, 0, DEEP),
	/* H10 : UART0_RXD ==> UART_PCH_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* H11 : UART0_TXD ==> UART_PCH_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
	/* H13 : I2C7_SCL ==> EN_PP3300_SD */
	PAD_CFG_GPO(GPP_H13, 1, PLTRST),

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
	/* F21 : EXT_PWR_GATE2# ==> WWAN_FCPO_L (set here for correct power sequencing) */
	PAD_CFG_GPO(GPP_F21, 1, DEEP),
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
