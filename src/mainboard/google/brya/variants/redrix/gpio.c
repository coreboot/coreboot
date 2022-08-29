/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <soc/gpio.h>

/* Pad configuration in ramstage */
static const struct pad_config override_gpio_table[] = {
	/* A17 : DISP_MISCC ==> NC */
	PAD_NC(GPP_A17, NONE),
	/* A19 : DDSP_HPD1 ==> NC */
	PAD_NC(GPP_A19, NONE),
	/* A20 : DDSP_HPD2 ==> NC */
	PAD_NC(GPP_A20, NONE),
	/* A21 : DDPC_CTRCLK ==> NC */
	PAD_NC(GPP_A21, NONE),
	/* A22 : DDPC_CTRLDATA ==> NC */
	PAD_NC(GPP_A22, NONE),

	/* B3  : PROC_GP2 ==> NC */
	PAD_NC_LOCK(GPP_B3, NONE, LOCK_CONFIG),
	/* B15  : TIME_SYNC0 ==> NC */
	PAD_NC_LOCK(GPP_B15, NONE, LOCK_CONFIG),

	/* C3 : SML0CLK ==> NC */
	PAD_NC(GPP_C3, NONE),
	/* C4 : SML0DATA ==> NC */
	PAD_NC(GPP_C4, NONE),

	/* D7  : SRCCLKREQ2# ==> NC */
	PAD_NC(GPP_D7, NONE),
	/* D13 : ISH_UART0_RXD ==> NC */
	PAD_NC_LOCK(GPP_D13, NONE, LOCK_CONFIG),

	/* D16 : ISH_UART0_CTS# ==> EN_NVM_SENSOR_PWR */
	PAD_CFG_GPO(GPP_D16, 1, DEEP),

	/* E3  : PROC_GP0 ==> NC */
	PAD_NC(GPP_E3, NONE),
	/* E7  : PROC_GP1 ==> NC */
	PAD_NC(GPP_E7, NONE),
	/* E16 : RSVD_TP ==> WWAN_RST_L */
	PAD_CFG_GPO(GPP_E16, 1, DEEP),
	/* E20 : DDP2_CTRLCLK ==> NC */
	PAD_NC(GPP_E20, NONE),
	/* E22 : DDPA_CTRLCLK ==> NC */
	PAD_NC(GPP_E22, NONE),
	/* E23 : DDPA_CTRLDATA ==> NC */
	PAD_NC(GPP_E23, NONE),
	/* F20 : EXT_PWR_GATE# ==> NC */
	PAD_NC(GPP_F20, NONE),

	/* H3  : SX_EXIT_HOLDOFF# ==> NC */
	PAD_NC_LOCK(GPP_H3, NONE, LOCK_CONFIG),
	/* H6  : I2C1_SDA ==> PCH_I2C_TPM_SDA */
	PAD_CFG_NF_LOCK(GPP_H6, NONE, NF1, LOCK_CONFIG),
	/* H7  : I2C1_SCL ==> PCH_I2C_TPM_SCL */
	PAD_CFG_NF_LOCK(GPP_H7, NONE, NF1, LOCK_CONFIG),
	/* H20 : IMGCLKOUT1 ==> NC */
	PAD_NC(GPP_H20, NONE),
	/* H21 : IMGCLKOUT2 ==> Privacy screen */
	PAD_CFG_GPO(GPP_H21, 0, DEEP),

	/* R6 : I2S_PCH_TX_SPKR_RX ==> NC */
	PAD_NC(GPP_R6, NONE),
	/* R7 : I2S_PCH_RX_SPKR_TX ==> NC */
	PAD_NC(GPP_R7, NONE),

	/* S4 : SNDW2_CLK ==> NC */
	PAD_NC(GPP_S4, NONE),
	/* S5 : SNDW2_DATA ==> NC */
	PAD_NC(GPP_S5, NONE),
	/* S6 : SNDW3_CLK ==> NC */
	PAD_NC(GPP_S6, NONE),
	/* S7 : SNDW3_DATA ==> NC */
	PAD_NC(GPP_S7, NONE),
	/*
	 * E0 : SATAXPCIE0 ==> WWAN_PERST_L
	 * Drive high here, so that PERST_L is sequenced after RST_L
	 */
	PAD_CFG_GPO(GPP_E0, 1, DEEP),
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* A12 : SATAXPCIE1 ==> EN_PP3300_WWAN */
	PAD_CFG_GPO(GPP_A12, 1, DEEP),
	/* A13 : PMC_I2C_SCL ==> GSC_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),
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
	/* E0  : SATAXPCIE0 ==> WWAN_PERST_L (updated in ramstage) */
	PAD_CFG_GPO(GPP_E0, 0, DEEP),
	/* E13 : THC0_SPI1_IO2 ==> MEM_CH_SEL */
	PAD_CFG_GPI(GPP_E13, NONE, DEEP),
	/* E15 : RSVD_TP ==> PCH_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_E15, NONE, DEEP),
	/* E16 : RSVD_TP ==> WWAN_RST_L (updated in ramstage) */
	PAD_CFG_GPO(GPP_E16, 0, DEEP),
	/* F18 : THC1_SPI2_INT# ==> EC_IN_RW_OD */
	PAD_CFG_GPI(GPP_F18, NONE, DEEP),
	/* F21 : EXT_PWR_GATE2# ==> WWAN_FCPO_L (updated in romstage) */
	PAD_CFG_GPO(GPP_F21, 0, DEEP),
	/* H10 : UART0_RXD ==> UART_PCH_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* H11 : UART0_TXD ==> UART_PCH_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
	/*
	 * enable EN_PP3300_SSD in bootblock, then PERST# is asserted, and
	 * then deassert PERST# in romstage
	 */
	/* H13 : I2C7_SCL ==> EN_PP3300_SD */
	PAD_CFG_GPO(GPP_H13, 1, DEEP),
	/* B4  : PROC_GP3 ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 0, DEEP),
};

static const struct pad_config romstage_gpio_table[] = {
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
