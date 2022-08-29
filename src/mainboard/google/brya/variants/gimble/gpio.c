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
	/* A14 : USB_OC1# ==> NC */
	PAD_NC(GPP_A14, NONE),
	/* A15 : USB_OC2# ==> NC */
	PAD_NC(GPP_A15, NONE),
	/* A18 : DDSP_HPDB ==> NC */
	PAD_NC(GPP_A18, NONE),
	/* A21 : DDPC_CTRCLK ==> NC */
	PAD_NC(GPP_A21, NONE),
	/* A22 : DDPC_CTRLDATA ==> NC */
	PAD_NC(GPP_A22, NONE),

	/* B3  : PROC_GP2 ==> NC */
	PAD_NC_LOCK(GPP_B3, NONE, LOCK_CONFIG),
	/* B5  : ISH_I2C0_SDA ==> NC */
	PAD_NC_LOCK(GPP_B5, NONE, LOCK_CONFIG),
	/* B6  : ISH_I2C0_SCL ==> NC */
	PAD_NC_LOCK(GPP_B6, NONE, LOCK_CONFIG),

	/* C3 : SML0CLK ==> NC */
	PAD_NC(GPP_C3, NONE),
	/* C4 : SML0DATA ==> NC */
	PAD_NC(GPP_C4, NONE),

	/* D3  : ISH_GP3 ==> NC */
	PAD_NC_LOCK(GPP_D3, NONE, LOCK_CONFIG),
	/* D5  : SRCCLKREQ0# ==> NC */
	PAD_NC(GPP_D5, NONE),
	/* D9  : ISH_SPI_CS# ==> NC */
	PAD_NC_LOCK(GPP_D9, NONE, LOCK_CONFIG),
	/* D15 : ISH_UART0_RTS# ==> NC */
	PAD_NC_LOCK(GPP_D15, NONE, LOCK_CONFIG),
	/* D16 : ISH_UART0_CTS# ==> EN_WCAM_PWR */
	PAD_CFG_GPO_LOCK(GPP_D16, 1, LOCK_CONFIG),
	/* D17 : UART1_RXD ==> NC */
	PAD_NC_LOCK(GPP_D17, NONE, LOCK_CONFIG),

	/* E0  : SATAXPCIE0 ==> NC */
	PAD_NC(GPP_E0, NONE),
	/* E3  : PROC_GP0 ==> NC */
	PAD_NC(GPP_E3, NONE),
	/* E4  : SATA_DEVSLP0 ==> NC */
	PAD_NC(GPP_E4, NONE),
	/* E7  : PROC_GP1 ==> NC */
	PAD_NC(GPP_E7, NONE),
	/* E10 : THC0_SPI1_CS# ==> NC */
	PAD_NC_LOCK(GPP_E10, NONE, LOCK_CONFIG),
	/* E16 : RSVD_TP ==> NC */
	PAD_NC(GPP_E16, NONE),
	/* E17 : THC0_SPI1_INT# ==> NC */
	PAD_NC_LOCK(GPP_E17, NONE, LOCK_CONFIG),
	/* E18 : DDP1_CTRLCLK ==> NC */
	PAD_NC(GPP_E18, NONE),
	/* E20 : DDP2_CTRLCLK ==> NC */
	PAD_NC(GPP_E20, NONE),

	/* F6  : CNV_PA_BLANKING ==> NC */
	PAD_NC(GPP_F6, NONE),
	/* F19 : SRCCLKREQ6# ==> NC */
	PAD_NC(GPP_F19, NONE),
	/* F20 : EXT_PWR_GATE# ==> NC */
	PAD_NC(GPP_F20, NONE),
	/* F21 : EXT_PWR_GATE2# ==> NC */
	PAD_NC(GPP_F21, NONE),

	/* H6  : I2C1_SDA ==> PCH_I2C_TPM_SDA_P2 */
	PAD_CFG_NF_LOCK(GPP_H6, NONE, NF1, LOCK_CONFIG),
	/* H7  : I2C1_SCL ==> PCH_I2C_TPM_SCL_P2 */
	PAD_CFG_NF_LOCK(GPP_H7, NONE, NF1, LOCK_CONFIG),
	/* H8  : I2C4_SDA ==> NC */
	PAD_NC(GPP_H8, NONE),
	/* H9  : I2C4_SCL ==> NC */
	PAD_NC(GPP_H9, NONE),
	/* H13 : I2C7_SCL ==> EN_PP3300_SD */
	PAD_CFG_GPO_LOCK(GPP_H13, 1, LOCK_CONFIG),
	/* H15 : DDPB_CTRLCLK ==> NC */
	PAD_NC(GPP_H15, NONE),
	/* H17 : DDPB_CTRLDATA ==> NC*/
	PAD_NC(GPP_H17, NONE),
	/* H19 : SRCCLKREQ4# ==> NC */
	PAD_NC(GPP_H19, NONE),
	/* H21 : IMGCLKOUT2 ==> NC */
	PAD_NC(GPP_H21, NONE),
	/* H22 : IMGCLKOUT3 ==> NC */
	PAD_NC(GPP_H22, NONE),
	/* H23 : SRCCLKREQ5# ==> NC */
	PAD_NC(GPP_H23, NONE),

	/* S4 : SNDW2_CLK ==> NC */
	PAD_NC(GPP_S4, NONE),
	/* S5 : SNDW2_DATA ==> NC */
	PAD_NC(GPP_S5, NONE),
	/* S6 : SNDW3_CLK ==> NC */
	PAD_NC(GPP_S6, NONE),
	/* S7 : SNDW3_DATA ==> NC */
	PAD_NC(GPP_S7, NONE),

	/* GPD11: LANPHYC ==> NC */
	PAD_NC(GPD11, NONE),
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* A13 : PMC_I2C_SCL ==> GSC_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),

	/* B4  : PROC_GP3 ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 0, DEEP),
	/* H6  : I2C1_SDA ==> PCH_I2C_TPM_SDA_P2 */
	PAD_CFG_NF(GPP_H6, NONE, DEEP, NF1),
	/* H7  : I2C1_SCL ==> PCH_I2C_TPM_SCL_P2 */
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

	/* E0  : SATAXPCIE0 ==> NC */
	PAD_NC(GPP_E0, NONE),
	/* E13 : THC0_SPI1_IO2 ==> MEM_CH_SEL */
	PAD_CFG_GPI(GPP_E13, NONE, DEEP),
	/* E15 : RSVD_TP ==> PCH_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_E15, NONE, DEEP),
	/* E16 : RSVD_TP ==> NC */
	PAD_NC(GPP_E16, NONE),
	/* F18 : THC1_SPI2_INT# ==> EC_IN_RW_OD */
	PAD_CFG_GPI(GPP_F18, NONE, DEEP),
	/* H10 : UART0_RXD ==> UART_PCH_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* H11 : UART0_TXD ==> UART_PCH_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
	/* H13 : I2C7_SCL ==> EN_PP3300_SD */
	PAD_CFG_GPO(GPP_H13, 1, DEEP),
};

static const struct pad_config romstage_gpio_table[] = {
	/* B4  : PROC_GP3 ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 1, DEEP),

	/* D1  : ISH_GP1 ==> FP_RST_ODL */
	PAD_CFG_GPO(GPP_D1, 0, DEEP),
	/* D2  : ISH_GP2 ==> EN_FP_PWR */
	PAD_CFG_GPO(GPP_D2, 0, DEEP),
	/* D18 : UART1_TXD ==> SD_PE_RST_L */
	PAD_CFG_GPO(GPP_D18, 1, DEEP),
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
