/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <soc/gpio.h>

/* Pad configuration in ramstage */
static const struct pad_config override_gpio_table[] = {
	/* A7  : SRCCLK_OE7# ==> PEN_DET_ODL */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_A7, NONE, DEEP),
	/* A8  : SRCCLKREQ7# ==> PEN_DET_ODL */
	PAD_CFG_GPI_SCI_HIGH(GPP_A8, NONE, DEEP, EDGE_SINGLE),
	/* B3  : PROC_GP2 ==> NC */
	PAD_NC_LOCK(GPP_B3, NONE, LOCK_CONFIG),
	/* B5  : ISH_I2C0_SDA ==> NC */
	PAD_NC_LOCK(GPP_B5, NONE, LOCK_CONFIG),
	/* B6  : ISH_I2C0_SCL ==> NC */
	PAD_NC_LOCK(GPP_B6, NONE, LOCK_CONFIG),
	/* B6  : TIME_SYNC0 ==> NC */
	PAD_NC_LOCK(GPP_B15, NONE, LOCK_CONFIG),
	/* C3 : SML0CLK ==> NC */
	PAD_NC(GPP_C3, NONE),
	/* C4 : SML0DATA ==> EN_PP5000_PEN */
	PAD_CFG_GPO(GPP_C4, 1, DEEP),
	/* D0  : ISH_GP0 ==> NC */
	PAD_NC_LOCK(GPP_D0, NONE, LOCK_CONFIG),
	/* D1  : ISH_GP1 ==> NC */
	PAD_NC_LOCK(GPP_D1, NONE, LOCK_CONFIG),
	/* D2  : ISH_GP2 ==> NC */
	PAD_NC_LOCK(GPP_D2, NONE, LOCK_CONFIG),
	/* D3  : ISH_GP3 ==> NC */
	PAD_NC_LOCK(GPP_D3, NONE, LOCK_CONFIG),
	/* D5  : SRCCLKREQ0# ==> NC */
	PAD_NC(GPP_D5, NONE),
	/* D15 : ISH_UART0_RTS# ==> NC */
	PAD_NC_LOCK(GPP_D15, NONE, LOCK_CONFIG),
	/* D16 : ISH_UART0_CTS# ==> NC */
	PAD_NC_LOCK(GPP_D16, NONE, LOCK_CONFIG),
	/* E0  : SATAXPCIE0 ==> NC */
	PAD_NC(GPP_E0, NONE),
	/* E3  : PROC_GP0 ==> NC */
	PAD_NC(GPP_E3, NONE),
	/* E7  : PROC_GP1 ==> NC */
	PAD_NC(GPP_E7, NONE),
	/* E9  : USB_OC0# ==> NC */
	PAD_NC_LOCK(GPP_E9, NONE, LOCK_CONFIG),
	/* E10 : THC0_SPI1_CS# ==> NC */
	PAD_NC_LOCK(GPP_E10, NONE, LOCK_CONFIG),
	/* E16 : RSVD_TP ==> NC */
	PAD_NC(GPP_E16, NONE),
	/* E17 : THC0_SPI1_INT# ==> NC */
	PAD_NC_LOCK(GPP_E17, NONE, LOCK_CONFIG),
	/* E18 : DDP1_CTRLCLK ==> NC */
	PAD_NC(GPP_E18, NONE),
	/* E19 : DDP1_CTRLDATA ==> GPP_E19_STRAP */
	PAD_NC(GPP_E19, NONE),
	/* E22 : DDPA_CTRLCLK ==> NC */
	PAD_NC(GPP_E22, NONE),
	/* E23 : DDPA_CTRLDATA ==> NC */
	PAD_NC(GPP_E23, NONE),
	/* F6  : CNV_PA_BLANKING ==> NC */
	PAD_NC(GPP_F6, NONE),
	/* F11 : THC1_SPI2_CLK ==> NC */
	PAD_NC_LOCK(GPP_F11, NONE, LOCK_CONFIG),
	/* F12 : GSXDOUT ==> NC */
	PAD_NC_LOCK(GPP_F12, NONE, LOCK_CONFIG),
	/* F13 : GSXDOUT ==> NC */
	PAD_NC_LOCK(GPP_F13, NONE, LOCK_CONFIG),
	/* F15 : GSXSRESET# ==> NC */
	PAD_NC_LOCK(GPP_F15, NONE, LOCK_CONFIG),
	/* F16 : GSXCLK ==> NC */
	PAD_NC_LOCK(GPP_F16, NONE, LOCK_CONFIG),
	/* F20 : EXT_PWR_GATE# ==> NC */
	PAD_NC(GPP_F20, NONE),
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
	/* H19 : SRCCLKREQ4# ==> NC */
	PAD_NC(GPP_H19, NONE),
	/* H21 : IMGCLKOUT2 ==>  NC */
	PAD_NC(GPP_H21, NONE),
	/* H22 : IMGCLKOUT3 ==> NC */
	PAD_NC(GPP_H22, NONE),
	/* H23 : SRCCLKREQ5# ==> NC */
	PAD_NC(GPP_H23, NONE),
	/* R4 : HDA_RST# ==> DMIC_CLK0 */
	PAD_CFG_NF(GPP_R4, NONE, DEEP, NF3),
	/* R5 : HDA_SDI1 ==> DMIC_DATA0 */
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

};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* A13 : PMC_I2C_SCL ==> GSC_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),
	/* B4  : PROC_GP3 ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 0, DEEP),
	/* H6  : I2C1_SDA ==> PCH_I2C_TPM_SDA */
	PAD_CFG_NF(GPP_H6, NONE, DEEP, NF1),
	/* H7  : I2C1_SCL ==> PCH_I2C_TPM_SCL */
	PAD_CFG_NF(GPP_H7, NONE, DEEP, NF1),
	/* D11 : ISH_SPI_MISO ==> EN_PP3300_SSD */
	PAD_CFG_GPO(GPP_D11, 1, DEEP),
	/* D18 : UART1_TXD ==> SD_PE_RST_L */
	PAD_CFG_GPO(GPP_D18, 0, PLTRST),
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
	PAD_CFG_GPO(GPP_H13, 1, PLTRST),
};

static const struct pad_config romstage_gpio_table[] = {
	/* B4  : PROC_GP3 ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 1, DEEP),
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
