/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <soc/gpio.h>
#include <types.h>
#include <vendorcode/google/chromeos/chromeos.h>

/* Pad configuration in ramstage for Pujjolo */
static const struct pad_config override_gpio_table[] = {
	/* E17 : WWAN_RST_L */
	PAD_CFG_GPO_LOCK(GPP_E17, 1, LOCK_CONFIG),
	/* A7  : SLP_S0_GATE */
	PAD_CFG_GPO(GPP_A7, 1, DEEP),
	/* A8  : WWAN_RF_DISABLE_ODL */
	PAD_CFG_GPO(GPP_A8, 1, DEEP),
	/* A11 : EN_SPK_PA ==> NC */
	PAD_NC(GPP_A11, NONE),
	/* A12 : NC# ==> SOC_PEN_DETECT_ODL */
	PAD_CFG_GPI_SCI_HIGH(GPP_A12, NONE, PLTRST, EDGE_SINGLE),
	/* A13 : GPP_A13 ==> GSC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),
	/* A18 : DDSP_HPDB ==> DDI2_HPD */
	/* A19 : NC */
	PAD_NC(GPP_A19, NONE),
	PAD_CFG_NF(GPP_A18, NONE, DEEP, NF1),
	/* A20  : HDMI_HPD */
	PAD_NC(GPP_A20, NONE),
	/* A21 : USB_C1_AUX_DC_P */
	PAD_CFG_NF(GPP_A21, NONE, DEEP, NF6),
	/* A22 : USB_C1_AUX_DC_N */
	PAD_CFG_NF(GPP_A22, NONE, DEEP, NF6),
	/* A23 :  NC */
	PAD_NC(GPP_A23, NONE),
	/* B2  : NC */
	PAD_NC(GPP_B2, NONE),
	/* B5  : SOC_I2C_SUB_SDA */
	PAD_CFG_NF(GPP_B5, NONE, DEEP, NF2),
	/* B6  : SOC_I2C_SUB_SCL */
	PAD_CFG_NF(GPP_B6, NONE, DEEP, NF2),
	/* D6  : SRCCLKREQ1# ==> WWAN_EN */
	PAD_CFG_GPO(GPP_D6, 1, DEEP),
	/* B7  : GPP_B7 ==> NC */
	PAD_NC_LOCK(GPP_B7, NONE, LOCK_CONFIG),
	/* B8  : WWAN_SAR_DETECT_2_ODL */
	PAD_NC_LOCK(GPP_B8, NONE, LOCK_CONFIG),
	/* B11 : SOC_PMC_PD0_INT_ODL */
	PAD_CFG_NF(GPP_B11, NONE, DEEP, NF1),
	/* B16  : GPP_B16 ==> I2C_5_SDA */
	PAD_CFG_NF_LOCK(GPP_B16, NONE, NF2, LOCK_CONFIG),
	/* B17  : GPP_B17 ==> I2C_5_SCL */
	PAD_CFG_NF_LOCK(GPP_B17, NONE, NF2, LOCK_CONFIG),
	/* C1  : SMBDATA ==> USI_RST_L */
	PAD_CFG_GPO(GPP_C1, 1, DEEP),
	/* C4  : TCHSCR_REPORT_EN */
	PAD_CFG_GPO(GPP_C4, 0, DEEP),
	/* C6  : I2C_SOC_PMC_PD_SCL */
	PAD_CFG_NF(GPP_C6, NONE, DEEP, NF1),
	/* C7  : I2C_SOC_PMC_PD_SDA */
	PAD_CFG_NF(GPP_C7, NONE, DEEP, NF1),
	/* D1  : SEN_MODE2_EC_ISH_INT_ODL */
	PAD_CFG_NF(GPP_D1, NONE, DEEP, NF1),
	/* D0  : ISH_GP0 ==> PCH_FP_BOOT0 */
	PAD_CFG_GPO_LOCK(GPP_D0, 0, LOCK_CONFIG),
	/* E7  : NC ==> FP_RST_ODL */
	PAD_CFG_GPO_LOCK(GPP_E7, 1, LOCK_CONFIG),
	/* D2 : NC ==> EN_FP_PWR */
	PAD_CFG_GPO_LOCK(GPP_D2, 1, LOCK_CONFIG),
	/* D3  : WCAM_RST_L ==> NC */
	PAD_CFG_GPO_LOCK(GPP_D3, 0, LOCK_CONFIG),
	/* D5  : SRCCLKREQ2# ==> WWAN_CLKREQ_ODL */
	//PAD_CFG_NF(GPP_D5, NONE, DEEP, NF1),
	/* D13 : UART0_ISH_RX_DBG_TX */
	PAD_CFG_NF(GPP_D13, NONE, DEEP, NF1),
	/* D14 : UART0_ISH_TX_DBG_RX */
	PAD_CFG_NF(GPP_D14, NONE, DEEP, NF1),
	/* D15 : ISH_UART0_RTS# ==> EN_PP2800_WCAM_X */
	PAD_CFG_GPO_LOCK(GPP_D15, 1, LOCK_CONFIG),
	/* D16 : ISH_UART0_CTS# ==> EN_PP1800_PP1200_WCAM_X */
	PAD_CFG_GPO_LOCK(GPP_D16, 1, LOCK_CONFIG),
	/* D17  : NC ==> UART_AP_RX_FP_TX */
	PAD_CFG_NF(GPP_D17, NONE, DEEP, NF1),
	/* D18  : NC ==> UART_AP_TX_FP_RX */
	PAD_CFG_NF(GPP_D18, NONE, DEEP, NF1),
	/* D19 : NC */
	PAD_NC(GPP_D19, NONE),
	/* E0 : NC ==> SOC_PEN_DETECT_R_ODL */
	PAD_CFG_GPI_INT(GPP_E0, NONE, PLTRST, EDGE_BOTH),
	/* E4  : WLAN_WWAN_COEX_1 temp out high*/
	PAD_CFG_GPO(GPP_E4, 0, DEEP),
	/* E5  : WLAN_WWAN_COEX_2 temp out high */
	PAD_CFG_GPO(GPP_E5, 0, DEEP),
	/* E11  : TCHSCR_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_E11, NONE, PLTRST, LEVEL, INVERT),
	/* E13 : SD_WAKE_N */
	PAD_CFG_GPI_LOCK(GPP_E13, NONE, LOCK_CONFIG),
	/* E20 : DDP2_CTRLCLK ==> HDMI_DDC_SCL */
	PAD_NC(GPP_E20, NONE),
	/* E21 : DDP2_CTRLDATA ==> HDMI_DDC_SDA_STRAP */
	PAD_NC(GPP_E21, NONE),
	/* E22 : USB_C0_AUX_DC_P */
	PAD_CFG_NF(GPP_E22, NONE, DEEP, NF6),
	/* E23 : USB_C0_AUX_DC_N */
	PAD_CFG_NF(GPP_E23, NONE, DEEP, NF6),
	/* F11 : NC ==> GSPI_PCH_CLK_FPMCU */
	PAD_CFG_NF(GPP_F11, NONE, DEEP, NF4),
	/* F12 : WWAN_RST_L ==> GSPI_PCH_DO_FPMCU_DI_R */
	PAD_CFG_NF(GPP_F12, NONE, DEEP, NF4),
	/* F13 : SOC_PEN_DETECT_R_ODL ==> GSPI_PCH_DI_FPMCU_DO */
	PAD_CFG_NF(GPP_F13, NONE, DEEP, NF4),
	/* F15 : SOC_PEN_DETECT_ODL ==> FPMCU_INT_L */
	PAD_CFG_GPI_IRQ_WAKE(GPP_F15, NONE, PWROK, LEVEL, INVERT),
	/* F16  : NC ==> GSPI_PCH_CS_FPMCU_R_L */
	PAD_CFG_NF(GPP_F16, NONE, DEEP, NF4),
	/* H8 : I2C_1_SDA */
	PAD_CFG_NF(GPP_H8, NONE, DEEP, NF1),
	/* H9 : I2C_1_SCL */
	PAD_CFG_NF(GPP_H9, NONE, DEEP, NF1),
	/* H15  : PD0_SOC_DBG_L */
	PAD_CFG_NF(GPP_H15, NONE, DEEP, NF1),
	//PAD_CFG_GPI_LOCK(GPP_H15, NONE, LOCK_CONFIG),
	/* H17  : PD1_SOC_DBG_L*/
	//PAD_CFG_GPI_LOCK(GPP_H17, NONE, LOCK_CONFIG),
	PAD_CFG_NF(GPP_H17, NONE, DEEP, NF1),
	/* H18 : CPU_C10_GATE_L */
	PAD_CFG_NF(GPP_H18, NONE, DEEP, NF1),
	/* H19 : SOC_I2C_SUB_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_H19, NONE, PLTRST, LEVEL, NONE),
	/* H21  : WWAN_PERST_L */
	PAD_NC_LOCK(GPP_H21, NONE, LOCK_CONFIG),
	/* H23 : WWAN_SAR_DETECT_ODL ==> NC */
	PAD_NC_LOCK(GPP_H23, NONE, LOCK_CONFIG),
	/* R0  : HDA_HP_BCLK_R */
	PAD_CFG_NF(GPP_R0, NONE, DEEP, NF1),
	/* R1  : HDA_HP_SYNC_R */
	PAD_CFG_NF(GPP_R1, NONE, DEEP, NF1),
	/* R2 : HDA_HP_SDO_R */
	PAD_CFG_NF(GPP_R2, NONE, DEEP, NF1),
	/* R3 : HDA_HP_SDIN0_R */
	PAD_CFG_NF(GPP_R3, NONE, DEEP, NF1),
	/* R4 : DMIC_CLK_A_0A ==> DMIC_UCAM_CLK_R */
	PAD_CFG_NF(GPP_R4, NONE, DEEP, NF3),
	/* R5 : DMIC_DATA_0A ==> DMIC_UCAM_DATA */
	PAD_CFG_NF(GPP_R5, NONE, DEEP, NF3),
	/* R6 : DMIC_CLK_A_1A ==> DMIC_WCAM_CLK_R */
	PAD_CFG_NF(GPP_R6, NONE, DEEP, NF3),
	/* R7 : DMIC_DATA_1A ==> DMIC_WCAM_DATA */
	PAD_CFG_NF(GPP_R7, NONE, DEEP, NF3),
	/* S0  : NC */
	PAD_NC(GPP_S0, NONE),
	/* S1  : NC */
	PAD_NC(GPP_S1, NONE),
	/* S2 :  NC */
	PAD_NC(GPP_S2, NONE),

};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* A13 : GPP_A13 ==> GSC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),
	/*
	* WWAN_EN is asserted in ramstage to meet the 500 ms warm reset toff
	* requirement. WWAN_EN must be asserted before WWAN_RST_L is released
	* (with min delay 0 ms), so this works as long as the pin used for
	* WWAN_EN comes before the pin used for WWAN_RST_L.
	*/
	/* D6  : SRCCLKREQ1# ==> WWAN_EN */
	PAD_CFG_GPO(GPP_D6, 0, DEEP),
	/* E12 : THC0_SPI1_IO1 ==> SOC_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_E12, NONE, DEEP),
	/*
	 * E7 ==> FP_RST_ODL
	 * FP_RST_ODL comes out of reset as hi-z and does not have an external pull-down.
	 * To ensure proper power sequencing for the FPMCU device, reset signal is driven low
	 * early on in bootblock, followed by enabling of power. Reset signal is deasserted
	 * later on in ramstage. Since reset signal is asserted in bootblock, it results in
	 * FPMCU not working after a S3 resume. This is a known issue.
	 */
	 PAD_CFG_GPO(GPP_E7, 0, DEEP),
	 /* D2  : ISH_GP2 ==> EN_FP_PWR */
	 PAD_CFG_GPO(GPP_D2, 1, DEEP),
	/* E17 : WWAN_RST_L */
	PAD_CFG_GPO(GPP_E17, 0, DEEP),
	/* H4  : I2C0_SDA ==> SOC_I2C_GSC_SDA */
	PAD_CFG_NF(GPP_H4, NONE, DEEP, NF1),
	/* H5  : I2C0_SCL ==> SOC_I2C_GSC_SCL */
	PAD_CFG_NF(GPP_H5, NONE, DEEP, NF1),
	/* H10 : UART0_RXD ==> UART_SOC_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* H11 : UART0_TXD ==> UART_SOC_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
};

/* Pad configuration in romstage for Sundance */
static const struct pad_config romstage_gpio_table[] = {
	/* E17 : WWAN_RST_L */
	PAD_CFG_GPO(GPP_E17, 1, DEEP),
	/* Enable touchscreen, hold in reset */
	/* C0  : SMBCLK ==> EN_PP3300_TCHSCR */
	PAD_CFG_GPO(GPP_C0, 1, DEEP),
	/* C1  : SMBDATA ==> USI_RST_L */
	PAD_CFG_TERM_GPO(GPP_C1, 0, UP_20K, DEEP),
	/*
	 * E7 ==> FP_RST_ODL
	 * FP_RST_ODL comes out of reset as hi-z and does not have an external pull-down.
	 * To ensure proper power sequencing for the FPMCU device, reset signal is driven low
	 * early on in bootblock, followed by enabling of power. Reset signal is deasserted
	 * later on in ramstage. Since reset signal is asserted in bootblock, it results in
	 * FPMCU not working after a S3 resume. This is a known issue.
	 */
	 PAD_CFG_GPO(GPP_E7, 0, DEEP),
	 /* D2  : ISH_GP2 ==> EN_FP_PWR */
	 PAD_CFG_GPO(GPP_D2, 0, DEEP),
};

const struct pad_config *variant_gpio_override_table(size_t *num)
{
	*num = ARRAY_SIZE(override_gpio_table);
	return override_gpio_table;
}

const struct pad_config *variant_gpio_table(size_t *num)
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

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AH(GPIO_PCH_WP, CROS_GPIO_DEVICE_NAME),
};
DECLARE_CROS_GPIOS(cros_gpios);
