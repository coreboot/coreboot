/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <soc/gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>

/* Pad configuration in ramstage */
static const struct pad_config override_gpio_table[] = {
	/* A7  : SRCCLK_OE7# ==> PEN_DET_ODL */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_A7, NONE, DEEP),
	/* A8  : SRCCLKREQ7# ==> PEN_DET_ODL */
	PAD_CFG_GPI_SCI_LOW(GPP_A8, NONE, DEEP, EDGE_SINGLE),
	/* B3  : PROC_GP2 ==> NC */
	PAD_NC(GPP_B3, NONE),
	/* B5  : ISH_I2C0_SDA ==> NC */
	PAD_NC(GPP_B5, NONE),
	/* B6  : ISH_I2C0_SCL ==> NC */
	PAD_NC(GPP_B6, NONE),
	/* B6  : TIME_SYNC0 ==> NC */
	PAD_NC(GPP_B15, NONE),
	/* C3 : SML0CLK ==> NC */
	PAD_NC(GPP_C3, NONE),
	/* C4 : SML0DATA ==> EN_PP5000_PEN */
	PAD_CFG_GPO(GPP_C4, 1, DEEP),
	/* D0  : ISH_GP0 ==> NC */
	PAD_NC(GPP_D0, NONE),
	/* D1  : ISH_GP1 ==> NC */
	PAD_NC(GPP_D1, NONE),
	/* D2  : ISH_GP2 ==> NC */
	PAD_NC(GPP_D2, NONE),
	/* D3  : ISH_GP3 ==> NC */
	PAD_NC(GPP_D3, NONE),
	/* D5  : SRCCLKREQ0# ==> NC */
	PAD_NC(GPP_D5, NONE),
	/* D15 : ISH_UART0_RTS# ==> NC */
	PAD_NC(GPP_D15, NONE),
	/* D16 : ISH_UART0_CTS# ==> NC */
	PAD_NC(GPP_D16, NONE),
	/* E0  : SATAXPCIE0 ==> NC */
	PAD_NC(GPP_E0, NONE),
	/* E3  : PROC_GP0 ==> NC */
	PAD_NC(GPP_E3, NONE),
	/* E7  : PROC_GP1 ==> NC */
	PAD_NC(GPP_E7, NONE),
	/* E9  : USB_OC0# ==> NC */
	PAD_NC(GPP_E9, NONE),
	/* E10 : THC0_SPI1_CS# ==> NC */
	PAD_NC(GPP_E10, NONE),
	/* E16 : RSVD_TP ==> NC */
	PAD_NC(GPP_E16, NONE),
	/* E17 : THC0_SPI1_INT# ==> NC */
	PAD_NC(GPP_E17, NONE),
	/* E18 : DDP1_CTRLCLK ==> NC */
	PAD_NC(GPP_E18, NONE),
	/* E22 : DDPA_CTRLCLK ==> NC */
	PAD_NC(GPP_E22, NONE),
	/* E23 : DDPA_CTRLDATA ==> NC */
	PAD_NC(GPP_E23, NONE),
	/* F6  : CNV_PA_BLANKING ==> NC */
	PAD_NC(GPP_F6, NONE),
	/* F11 : THC1_SPI2_CLK ==> NC */
	PAD_NC(GPP_F11, NONE),
	/* F12 : GSXDOUT ==> NC */
	PAD_NC(GPP_F12, NONE),
	/* F13 : GSXDOUT ==> NC */
	PAD_NC(GPP_F13, NONE),
	/* F15 : GSXSRESET# ==> NC */
	PAD_NC(GPP_F15, NONE),
	/* F16 : GSXCLK ==> NC */
	PAD_NC(GPP_F16, NONE),
	/* F20 : EXT_PWR_GATE# ==> NC */
	PAD_NC(GPP_F20, NONE),
	/* F21 : EXT_PWR_GATE2# ==> NC */
	PAD_NC(GPP_F21, NONE),
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
	/* B7  : ISH_12C1_SDA ==> PCH_I2C_TPM_SDA */
	PAD_CFG_NF(GPP_B7, NONE, DEEP, NF2),
	/* B8  : ISH_12C1_SCL ==> PCH_I2C_TPM_SCL */
	PAD_CFG_NF(GPP_B8, NONE, DEEP, NF2),
	/* D18 : UART1_TXD ==> SD_PE_RST_L */
	PAD_CFG_GPO(GPP_D18, 0, PLTRST),
	/* E13 : THC0_SPI1_IO2 ==> MEM_CH_SEL */
	PAD_CFG_GPI(GPP_E13, NONE, DEEP),
	/* E15 : RSVD_TP ==> PCH_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_E15, NONE, DEEP),
	/* H10 : UART0_RXD ==> UART_PCH_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* H11 : UART0_TXD ==> UART_PCH_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
	/* H13 : I2C7_SCL ==> EN_PP3300_SD */
	PAD_CFG_GPO(GPP_H13, 1, PLTRST),
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
