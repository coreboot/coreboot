/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <boardid.h>
#include <soc/gpio.h>

/* Pad configuration in ramstage */
static const struct pad_config override_gpio_table[] = {
	/* A5  : ESPI_ALERT0# ==> NC */
	PAD_NC(GPP_A5, NONE),
	/* A6  : ESPI_ALERT1# ==> NC */
	PAD_NC(GPP_A6, NONE),
	/* A7  : SRCCLK_OE7# ==> NC */
	PAD_NC(GPP_A7, NONE),
	/* A8  : WWAN_RF_DISABLE_ODL ==> NC */
	PAD_NC(GPP_A8, NONE),
	/* A9  : ESPI_CLK ==> ESPI_CLK */
	PAD_CFG_NF(GPP_A9, NONE, DEEP, NF1),
	/* A10 : ESPI_RESET# ==> ESPI_PCH_RST_EC_L */
	PAD_CFG_NF(GPP_A10, NONE, DEEP, NF1),
	/* A12 : EN_PP3300_WWAN ==> NC */
	PAD_NC(GPP_A12, NONE),
	/* A19 : USB_C2_AUX_DC_P ==> NC */
	PAD_NC(GPP_A19, NONE),
	/* A20 : USB_C2_AUX_DC_N ==> NC */
	PAD_NC(GPP_A20, NONE),
	/* A21 : USB_C1_AUX_DC_P ==> NC */
	PAD_NC(GPP_A21, NONE),
	/* A22 : USB_C1_AUX_DC_N ==> NC */
	PAD_NC(GPP_A22, NONE),
	/* B2  : GPP_B2(TP97) ==> GPP_B2(TP1712) */
	PAD_NC(GPP_B2, NONE),
	/* B3  : PROC_GP2 ==> NC */
	PAD_NC(GPP_B3, NONE),
	/* B5  : PCH_I2C_MISC_SDA ==> NC */
	PAD_NC(GPP_B5, NONE),
	/* B6  : PCH_I2C_MISC_SCL ==> NC */
	PAD_NC(GPP_B6, NONE),
	/* B7  : PCH_I2C_TCHSCR_SDA ==> PCH_I2C_TCHSCR_SDA */
	PAD_CFG_NF_LOCK(GPP_B7, NONE, NF2, LOCK_CONFIG),
	/* B8  : PCH_I2C_TCHSCR_SCL ==> PCH_I2C_TCHSCR_SCL */
	PAD_CFG_NF_LOCK(GPP_B8, NONE, NF2, LOCK_CONFIG),
	/* B15 : FP_USER_PRES_FP_L ==> NC */
	PAD_NC(GPP_B15, NONE),
	/* B23 : SML1ALERT# ==> PCHHOT_ODL_STRAP */
	PAD_CFG_NF_LOCK(GPP_B23, DN_20K, NF2, LOCK_CONFIG),
	/* C3  : EN_UCAM_PWR ==> EN_UCAM_PWR(TP1103) */
	PAD_NC(GPP_C3, NONE),
	/* C4  : EN_UCAM_SENR_PWR ==> EN_UCAM_SENR_PWR(TP1104) */
	PAD_NC(GPP_C4, NONE),
	/* D0  : ISH_GP0 ==> NC */
	PAD_NC(GPP_D0, NONE),
	/* D3  : ISH_GP3 ==> NC */
	PAD_NC(GPP_D3, NONE),
	/* D5  : WWAN_DPR_SAR_ODL ==> NC */
	PAD_NC(GPP_D5, NONE),
	/* D8  : SD_CLKREQ_ODL ==> NC */
	PAD_NC(GPP_D8, NONE),
	/* D9  : USB_C2_LSX_TX ==> NC */
	PAD_NC(GPP_D9, NONE),
	/* D10 : ISH_SPI_CLK ==> NC */
	PAD_NC(GPP_D10, NONE),
	/* D11 : ISH_SPI_MISO ==> EN_PP3300_SSD */
	PAD_CFG_GPO(GPP_D11, 1, DEEP),
	/* D13 : ISH_UART0_RXD ==> NC */
	PAD_NC(GPP_D13, NONE),
	/* D14 : ISH_UART0_TXD ==> NC */
	PAD_NC(GPP_D14, NONE),
	/* D15 : ISH_UART0_RTS# ==> NC */
	PAD_NC(GPP_D15, NONE),
	/* D16 : ISH_UART0_CTS# ==> NC */
	PAD_NC(GPP_D16, NONE),
	/* D17 : UART1_RXD ==> NC */
	PAD_NC(GPP_D17, NONE),
	/* D18 : UART1_TXD ==> NC */
	PAD_NC(GPP_D18, NONE),
	/* E0 : WWAN_PERST_L ==> NC */
	PAD_NC(GPP_E0, NONE),
	/* E3  : PROC_GP0 ==> NC */
	PAD_NC(GPP_E3, NONE),
	/* E4  : SATA_DEVSLP0 ==> NC */
	PAD_NC(GPP_E4, NONE),
	/* E5  : SATA_DEVSLP1 ==> NC */
	PAD_NC(GPP_E5, NONE),
	/* E7  : PROC_GP1 ==> NC */
	PAD_NC(GPP_E7, NONE),
	/* E10 : WWAN_CONFIG0 ==> NC */
	PAD_NC(GPP_E10, NONE),
	/* E16 : WWAN_RST_L ==> NC */
	PAD_NC(GPP_E16, NONE),
	/* E17 : WWAN_CONFIG2 ==> SSD_STRAP */
	PAD_CFG_GPI(GPP_E17, NONE, DEEP),
	/* E18 : USB_C0_LSX_TX ==> NC */
	PAD_NC(GPP_E18, NONE),
	/* E19 : DDP1_CTRLDATA ==> NC */
	PAD_NC(GPP_E19, NONE),
	/* E20 : USB_C1_LSX_TX ==> NC */
	PAD_NC(GPP_E20, NONE),
	/* E21 : DDP2_CTRLDATA ==> NC */
	PAD_NC(GPP_E21, NONE),
	/* F6  : WWAN_WLAN_COEX3 ==> NC */
	PAD_NC(GPP_F6, NONE),
	/* F19 : GPP_F19(TP93) ==> NC */
	PAD_NC(GPP_F19, NONE),
	/* F20 : UCAM_RST_L ==> NC */
	PAD_NC(GPP_F20, NONE),
	/* F21 : WWAN_FCPO_L ==> NC */
	PAD_NC(GPP_F21, NONE),
	/* F23 : NC */
	PAD_CFG_NF_LOCK(GPP_F23, NONE, NF1, LOCK_CONFIG),
	/* H8  : WWAN_WLAN_COEX1 ==> PCB_ID0(NC) */
	PAD_CFG_GPI_LOCK(GPP_H8, NONE, LOCK_CONFIG),
	/* H9  : WWAN_WLAN_COEX2 ==> PCB_ID1(NC) */
	PAD_CFG_GPI_LOCK(GPP_H9, NONE, LOCK_CONFIG),
	/* H12 : I2C7_SDA ==> NC */
	PAD_NC(GPP_H12, NONE),
	/* H19 : SRCCLKREQ4# ==> NC */
	PAD_NC(GPP_H19, NONE),
	/* H21 : UCAM_MCLK ==>  NC */
	PAD_NC(GPP_H21, NONE),
	/* H22 : WCAM_MCLK ==> NC */
	PAD_NC(GPP_H22, NONE),
	/* H23 : WWAN_CLKREQ_ODL ==> NC */
	PAD_NC(GPP_H23, NONE),
	/* R4 : HDA_RST# ==> NC */
	PAD_NC(GPP_R4, NONE),
	/* R5 : HDA_SDI1 ==> NC */
	PAD_NC(GPP_R5, NONE),
	/* R6 : I2S2_TXD ==> I2S_PCH_TX_SPKR_RX_R */
	PAD_CFG_NF(GPP_R6, NONE, DEEP, NF3),
	/* R7 : I2S2_RXD ==> I2S_PCH_RX_SPKR_TX */
	PAD_CFG_NF(GPP_R7, NONE, DEEP, NF3),
	/* S0 : SNDW0_CLK ==> SDW_HP_CLK_R */
	PAD_CFG_NF(GPP_S0, NONE, DEEP, NF4),
	/* S1 : SNDW0_DATA ==> SDW_HP_DATA_R */
	PAD_CFG_NF(GPP_S1, NONE, DEEP, NF4),
	/* S2 : SNDW1_CLK ==> DMIC_CLK0_R */
	PAD_CFG_NF(GPP_S2, NONE, DEEP, NF4),
	/* S3 : SNDW1_DATA ==> NC */
	PAD_NC(GPP_S3, NONE),
	/* S4 : SDW_SPKR_CLK ==> NC */
	PAD_NC(GPP_S4, NONE),
	/* S5 : SDW_SPKR_DATA ==> NC */
	PAD_NC(GPP_S5, NONE),
	/* S6 : DMIC_CLK1_R ==> NC */
	PAD_NC(GPP_S6, NONE),
	/* S7 : DMIC_DATA1_R ==> NC */
	PAD_NC(GPP_S7, NONE),
	/* GPD11: LANPHYC ==> NC */
	PAD_NC(GPD11, NONE),
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* A13 : GSC_PCH_INT_ODL ==> GSC_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),
	/* B4  : SSD_PERST_L ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 0, DEEP),
	/* B7  : PCH_I2C_TCHSCR_SDA ==> PCH_I2C_TCHSCR_SDA */
	PAD_CFG_NF(GPP_B7, NONE, DEEP, NF2),
	/* B8  : PCH_I2C_TCHSCR_SCL ==> PCH_I2C_TCHSCR_SCL */
	PAD_CFG_NF(GPP_B8, NONE, DEEP, NF2),

	/*
	 * D1  : FP_RST_ODL ==> FP_RST_ODL
	 * FP_RST_ODL comes out of reset as hi-z and does not have an external pull-down.
	 * To ensure proper power sequencing for the FPMCU device, reset signal is driven low
	 * early on in bootblock, followed by enabling of power. Reset signal is deasserted
	 * later on in ramstage. Since reset signal is asserted in bootblock, it results in
	 * FPMCU not working after a S3 resume. This is a known issue.
	 */
	PAD_CFG_GPO(GPP_D1, 0, DEEP),
	/* D2  : EN_FP_PWR ==> EN_FP_PWR */
	PAD_CFG_GPO(GPP_D2, 1, DEEP),
	/* D11 : ISH_SPI_MISO ==> EN_PP3300_SSD */
	PAD_CFG_GPO(GPP_D11, 1, DEEP),
	/* E0  : WWAN_PERST_L ==> NC */
	PAD_NC(GPP_E0, NONE),
	/* E13 : MEM_CH_SEL ==> MEM_CH_SEL */
	PAD_CFG_GPI(GPP_E13, NONE, DEEP),
	/* E15 : PCH_WP_OD ==> PCH_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_E15, NONE, DEEP),
	/* E16 : WWAN_RST_L ==> NC */
	PAD_NC(GPP_E16, NONE),
	/* F18 : EC_IN_RW_OD ==> EC_IN_RW_OD */
	PAD_CFG_GPI(GPP_F18, NONE, DEEP),
	/* H6  : PCH_I2C_TPM_SDA ==> PCH_I2C_TPM_SDA */
	PAD_CFG_NF(GPP_H6, NONE, DEEP, NF1),
	/* H7  : PCH_I2C_TPM_SCL ==> PCH_I2C_TPM_SCL */
	PAD_CFG_NF(GPP_H7, NONE, DEEP, NF1),
	/* H10 : UART_PCH_RX_DBG_TX ==> UART_PCH_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* H11 : UART_PCH_TX_DBG_RX ==> UART_PCH_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
	/* H13 : EN_PP3300_SD ==> EN_PP3300_SD(TP1201) */
	PAD_NC(GPP_H13, UP_20K),
};

static const struct pad_config romstage_gpio_table[] = {
	/* B4  : SSD_PERST_L ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 1, DEEP),
	/* D1  : FP_RST_ODL ==> FP_RST_ODL */
	PAD_CFG_GPO(GPP_D1, 0, DEEP),
	/* D2  : EN_FP_PWR ==> EN_FP_PWR */
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
