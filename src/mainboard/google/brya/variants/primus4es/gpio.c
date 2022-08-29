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
	/* A14 : USB_OC1# ==> NC */
	PAD_NC(GPP_A14, NONE),
	/* A15 : USB_OC2# ==> NC */
	PAD_NC(GPP_A15, NONE),
	/* A21 : DDPC_CTRCLK ==> NC */
	PAD_NC(GPP_A21, NONE),
	/* A22 : DDPC_CTRLDATA ==> NC */
	PAD_NC(GPP_A22, NONE),

	/* B2  : VRALERT# ==> NC */
	PAD_NC(GPP_B2, NONE),
	/* B3  : PROC_GP2 ==> eMMC_PERST_L */
	PAD_CFG_GPO(GPP_B3, 1, DEEP),
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

	/* D3  : ISH_GP3 ==> NC */
	PAD_NC_LOCK(GPP_D3, NONE, LOCK_CONFIG),
	/* D5  : SRCCLKREQ0# ==> SSD_CLKREQ_ODL */
	PAD_CFG_NF(GPP_D5, NONE, DEEP, NF1),
	/* D6  : SRCCLKREQ1# ==> NC */
	PAD_NC(GPP_D6, NONE),
	/* D13 : ISH_UART0_RXD ==> NC */
	PAD_NC_LOCK(GPP_D13, NONE, LOCK_CONFIG),
	/* D14 : ISH_UART0_TXD ==> USB_A1_RT_RST_ODL */
	PAD_CFG_GPO_LOCK(GPP_D14, 1, LOCK_CONFIG),
	/* D18 : UART1_TXD ==> SD_PE_RST_L */
	PAD_CFG_GPO_LOCK(GPP_D18, 1, LOCK_CONFIG),

	/* E3  : PROC_GP0 ==> NC */
	PAD_NC(GPP_E3, NONE),
	/* E7  : PROC_GP1 ==> NC */
	PAD_NC(GPP_E7, NONE),
	/* E20 : USB_C1_LSX_SOC_TX ==> EN_PP3300_eMMC */
	PAD_CFG_GPO(GPP_E20, 1, DEEP),
	/* E21 : DDP2_CTRLDATA ==> NC */
	PAD_NC(GPP_E21, NONE),

	/* F19 : SRCCLKREQ6# ==> EMMC_CLKREQ_ODL */
	PAD_CFG_NF(GPP_F19, NONE, DEEP, NF1),
	/* F20 : EXT_PWR_GATE# ==> NC */
	PAD_NC(GPP_F20, NONE),

	/* H19 : SRCCLKREQ4# ==> NC */
	PAD_NC(GPP_H19, NONE),
	/* H21 : IMGCLKOUT2 ==> NC */
	PAD_NC(GPP_H21, NONE),
	/* H22 : IMGCLKOUT3 ==> NC */
	PAD_NC(GPP_H22, NONE),
	/* H23 : SRCCLKREQ5# ==> NC */
	PAD_NC(GPP_H23, NONE),

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
	/* A12 : SATAXPCIE1 ==> EN_PPVAR_WWAN */
	PAD_CFG_GPO(GPP_A12, 1, DEEP),
	/* A13 : PMC_I2C_SCL ==> GSC_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),
	/* B3  : PROC_GP2 ==> eMMC_PERST_L */
	PAD_CFG_GPO(GPP_B3, 0, DEEP),
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
	/* D18 : UART1_TXD ==> SD_PE_RST_L */
	PAD_CFG_GPO(GPP_D18, 0, PLTRST),
	/* E0  : SATAXPCIE0 ==> WWAN_PERST_L (updated in ramstage)*/
	PAD_CFG_GPO(GPP_E0, 0, DEEP),
	/* E13 : THC0_SPI1_IO2 ==> MEM_CH_SEL */
	PAD_CFG_GPI(GPP_E13, NONE, DEEP),
	/* E16 : RSVD_TP ==> WWAN_RST_L (updated in ramstage)*/
	PAD_CFG_GPO(GPP_E16, 0, DEEP),
	/* E15 : RSVD_TP ==> PCH_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_E15, NONE, DEEP),
	/* E20 : USB_C1_LSX_SOC_TX ==> EN_PP3300_eMMC */
	PAD_CFG_GPO(GPP_E20, 1, DEEP),
	/* F21 : EXT_PWR_GATE2# ==> WWAN_FCPO_L (updated in romstage) */
	PAD_CFG_GPO(GPP_F21, 0, DEEP),
	/* F18 : THC1_SPI2_INT# ==> EC_IN_RW_OD */
	PAD_CFG_GPI(GPP_F18, NONE, DEEP),
	/* H10 : UART0_RXD ==> UART_PCH_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* H11 : UART0_TXD ==> UART_PCH_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
	/* H13 : I2C7_SCL ==> EN_PP3300_SD */
	PAD_CFG_GPO(GPP_H13, 1, PLTRST),
};

static const struct pad_config romstage_gpio_table[] = {
	/* A12 : SATAXPCIE1 ==> EN_PPVAR_WWAN (set here for correct power sequencing) */
	PAD_CFG_GPO(GPP_A12, 1, DEEP),

	/* B4  : PROC_GP3 ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 1, DEEP),

	/* D1  : ISH_GP1 ==> FP_RST_ODL */
	PAD_CFG_GPO(GPP_D1, 0, DEEP),
	/* D2  : ISH_GP2 ==> EN_FP_PWR */
	PAD_CFG_GPO(GPP_D2, 0, DEEP),

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
