/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <soc/gpio.h>

/* Pad configuration in ramstage */
static const struct pad_config override_gpio_table[] = {
	/* A11 : PMC_I2C_SDA ==> NC */
	PAD_NC(GPP_A11, NONE),

	/* B2  : VRALERT# ==> RGB_RST_ODL */
	PAD_CFG_GPO_LOCK(GPP_B2, 1, LOCK_CONFIG),
	/* B15  : TIME_SYNC0 ==> NC */
	PAD_NC_LOCK(GPP_B15, NONE, LOCK_CONFIG),

	/* C3 : SML0CLK ==> NC */
	PAD_NC(GPP_C3, NONE),

	/* D3  : ISH_GP3 ==> EN_PP3300_SSD */
	PAD_CFG_GPO_LOCK(GPP_D3, 1, LOCK_CONFIG),
	/* D11 : ISH_SPI_MISO ==> USB_C3_LSX_TX */
	PAD_CFG_NF_LOCK(GPP_D11, NONE, NF4, LOCK_CONFIG),
	/* D12 : ISH_SPI_MOSI ==> USB_C3_LSX_RX_STRAP */
	PAD_CFG_NF_LOCK(GPP_D12, NONE, NF4, LOCK_CONFIG),

	/* E3  : PROC_GP0 ==> MEM_STRAP_0 */
	PAD_CFG_GPI(GPP_E3, NONE, DEEP),
	/* E5  : SATA_DEVSLP1 ==> MEM_CH_SEL */
	PAD_CFG_GPI(GPP_E5, NONE, DEEP),
	/* E7  : PROC_GP1 ==> MEM_STRAP_3 */
	PAD_CFG_GPI(GPP_E7, NONE, DEEP),
	/* E10 : THC0_SPI1_CS# ==> UWB_GSPI0_CS */
	PAD_CFG_NF_LOCK(GPP_E10, NONE, NF2, LOCK_CONFIG),
	/* E11 : THC0_SPI1_CLK ==> UWB_CLK */
	PAD_CFG_NF_LOCK(GPP_E11, NONE, NF2, LOCK_CONFIG),
	/* E12 : THC0_SPI1_IO1 ==> UWB_GSPI0_DI */
	PAD_CFG_NF_LOCK(GPP_E12, NONE, NF3, LOCK_CONFIG),
	/* E13 : THC0_SPI1_IO2 ==> UWB_GSPI0_DO */
	PAD_CFG_NF_LOCK(GPP_E13, NONE, NF3, LOCK_CONFIG),
	/* E16 : RSVD_TP ==> WWAN_RST_L */
	PAD_CFG_GPO(GPP_E16, 1, DEEP),
	/* E22 : DDPA_CTRLCLK ==> WWAN_CONFIG0 */
	PAD_CFG_GPI(GPP_E22, NONE, DEEP),
	/* E23 : DDPA_CTRLDATA ==> USB_C3_OC_ODL */
	PAD_CFG_NF(GPP_E23, NONE, DEEP, NF1),
	/*
	 * E0 : SATAXPCIE0 ==> WWAN_PERST_L
	 * Drive high here, so that PERST_L is sequenced after RST_L
	 */
	PAD_CFG_GPO(GPP_E0, 1, DEEP),
	/* F19 : NC */
	PAD_NC(GPP_F19, NONE),

	/* H6  : I2C1_SDA ==> PCH_I2C_TPM_SDA */
	PAD_CFG_NF_LOCK(GPP_H6, NONE, NF1, LOCK_CONFIG),
	/* H7  : IC1_SCL ==> PCH_I2C_TPM_SCL */
	PAD_CFG_NF_LOCK(GPP_H7, NONE, NF1, LOCK_CONFIG),
	/* H12 : Reserved I2C7_SDAL for AMP */
	PAD_NC(GPP_H12, NONE),
	/* H13 : Reserved I2C7_SCL for AMP */
	PAD_NC(GPP_H13, NONE),
	/* H15 : DDPB_CTRLCLK ==> USB_C3_AUX_DC_P */
	PAD_CFG_NF(GPP_H15, NONE, DEEP, NF6),
	/* H17 : DDPB_CTRLDATA ==> USB_C3_AUX_DC_N */
	PAD_CFG_NF(GPP_H17, NONE, DEEP, NF6),
	/* H21 : IMGCLKOUT2 ==>  UCAM_MCLK_R */
	PAD_CFG_NF(GPP_H21, NONE, DEEP, NF1),

	/* R4 : HDA_RST# ==> DMIC_CLK0_R */
	PAD_CFG_NF(GPP_R4, NONE, DEEP, NF3),
	/* R5 : HDA_SDI1 ==> DMIC_DATA0_R */
	PAD_CFG_NF(GPP_R5, NONE, DEEP, NF3),
	/* R6 : NC */
	PAD_NC(GPP_R6, NONE),
	/* R7 : NC */
	PAD_NC(GPP_R7, NONE),

	/* S0 : SNDW0_CLK ==> I2S_SPKR_SCLK_R */
	PAD_CFG_NF(GPP_S0, NONE, DEEP, NF4),
	/* S1 : SNDW0_DATA ==> I2S_SPKR_SFRM_R */
	PAD_CFG_NF(GPP_S1, NONE, DEEP, NF4),
	/* S2 : SNDW1_CLK ==> DMIC_CLK0_R */
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

	/* GPD11: LANPHYC ==> WWAN_CONFIG1 */
	PAD_NC(GPD11, NONE),
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* A12 : SATAXPCIE1 ==> EN_PP3300_WWAN */
	PAD_CFG_GPO(GPP_A12, 1, DEEP),
	/* A13 : PMC_I2C_SCL ==> GSC_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),
	/* B4  : PROC_GP3 ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 0, DEEP),
	/* H6  : I2C1_SDA ==> PCH_I2C_TPM_SDA */
	PAD_CFG_NF(GPP_H6, NONE, DEEP, NF1),
	/* H7  : IC1_SCL ==> PCH_I2C_TPM_SCL */
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
	/* D3  : ISH_GP3 ==> EN_PP3300_SSD */
	PAD_CFG_GPO(GPP_D3, 1, DEEP),
	/* D11 : ISH_SPI_MISO ==> USB_C0_LSX_SOC_TX */
	PAD_CFG_NF(GPP_D11, NONE, DEEP, NF4),
	/* E0  : SATAXPCIE0 ==> WWAN_PERST_L (updated in ramstage) */
	PAD_CFG_GPO(GPP_E0, 0, DEEP),
	/* E3  : PROC_GP0 ==> MEM_STRAP_0 */
	PAD_CFG_GPI(GPP_E3, NONE, DEEP),
	/* E5  : SATA_DEVSLP1 ==> MEM_CH_SEL */
	PAD_CFG_GPI(GPP_E5, NONE, DEEP),
	/* E7  : PROC_GP1 ==> MEM_STRAP_3 */
	PAD_CFG_GPI(GPP_E7, NONE, DEEP),

	/* E15 : RSVD_TP ==> PCH_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_E15, NONE, DEEP),
	/* E16 : RSVD_TP ==> WWAN_RST_L  */
	PAD_CFG_GPO(GPP_E16, 0, DEEP),
	/* F18 : THC1_SPI2_INT# ==> EC_IN_RW_OD */
	PAD_CFG_GPI(GPP_F18, NONE, DEEP),
	/* F21 : EXT_PWR_GATE2# ==> WWAN_FCPO_L (updated in romstage) */
	PAD_CFG_GPO(GPP_F21, 0, DEEP),
	/* H10 : UART0_RXD ==> UART_PCH_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* H11 : UART0_TXD ==> UART_PCH_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),

	/*Add virtual GPIOs for CPU PCIe RP*/
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

const struct pad_config *variant_romstage_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(romstage_gpio_table);
	return romstage_gpio_table;
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
