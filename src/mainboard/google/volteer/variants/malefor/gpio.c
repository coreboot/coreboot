/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>

/* Pad configuration in ramstage */
/* Leave eSPI pins untouched from default settings */
static const struct pad_config gpio_table[] = {
	/* A0 thru A6 come configured out of reset, do not touch */
	/* A0  : ESPI_IO0 ==> ESPI_IO_0 */
	/* A1  : ESPI_IO1 ==> ESPI_IO_1 */
	/* A2  : ESPI_IO2 ==> ESPI_IO_2 */
	/* A3  : ESPI_IO3 ==> ESPI_IO_3 */
	/* A4  : ESPI_CS# ==> ESPI_CS_L */
	/* A5  : ESPI_CLK ==> ESPI_CLK */
	/* A6  : ESPI_RESET# ==> NC(TP764) */
	/* A7  : I2S2_SCLK ==> EN_PP3300_TRACKPAD */
	PAD_CFG_GPO(GPP_A7, 1, DEEP),
	/* A8  : I2S2_SFRM ==> EN_PP3300_TOUCHSCREEN */
	PAD_CFG_GPO(GPP_A8, 1, DEEP),
	/* A10 : I2S2_RXD ==> EN_SPKR_PA */
	PAD_CFG_GPO(GPP_A10, 1, DEEP),
	/* A13 : PMC_I2C_SCL ==> BT_DISABLE_L */
	PAD_CFG_GPO(GPP_A13, 1, DEEP),
	/* A16 : USB_OC3# ==> USB_C0_OC_ODL */
	PAD_CFG_NF(GPP_A16, NONE, DEEP, NF1),
	/* A18 : DDSP_HPDB ==> HDMI_HPD */
	PAD_CFG_NF(GPP_A18, NONE, DEEP, NF1),
	/* A19 : DDSP_HPD1 ==> USB_C0_DP_HPD */
	PAD_CFG_NF(GPP_A19, NONE, DEEP, NF1),
	/* A20 : DDSP_HPD2 ==> USB_C1_DP_HPD */
	PAD_CFG_NF(GPP_A20, NONE, DEEP, NF1),
	/* A22 : DDPC_CTRLDATA ==> EN_HDMI_PWR */
	PAD_CFG_GPO(GPP_A22, 1, DEEP),
	/* A23 : I2S1_SCLK ==> I2S1_SPKR_SCLK */
	PAD_CFG_NF(GPP_A23, NONE, DEEP, NF1),

	/* B2  : VRALERT# ==> NOT USED */
	PAD_NC(GPP_B2, NONE),
	/* B3  : CPU_GP2 ==> PEN_DET_ODL */
	PAD_CFG_GPI(GPP_B3, NONE, DEEP),
	/* B9  : I2C5_SDA ==> PCH_I2C5_TRACKPAD_SDA */
	PAD_CFG_NF(GPP_B9, NONE, DEEP, NF1),
	/* B10 : I2C5_SCL ==> PCH_I2C5_TRACKPAD_SCL */
	PAD_CFG_NF(GPP_B10, NONE, DEEP, NF1),
	/* B19 : GSPI1_CS0# ==> PCH_GSPI1_FPMCU_CS_L */
	PAD_CFG_NF(GPP_B19, NONE, DEEP, NF1),
	/* B20 : GSPI1_CLK ==> PCH_GSPI1_FPMCU_CLK */
	PAD_CFG_NF(GPP_B20, NONE, DEEP, NF1),
	/* B21 : GSPI1_MISO ==> PCH_GSPI1_FPMCU_MISO */
	PAD_CFG_NF(GPP_B21, NONE, DEEP, NF1),
	/* B22 : GSPI1_MOSI ==> PCH_GSPI1_GPMCU_MOSI */
	PAD_CFG_NF(GPP_B22, NONE, DEEP, NF1),

	/* C0  : SMBCLK ==> EN_PP3300_WLAN */
	PAD_CFG_GPO(GPP_C0, 1, DEEP),
	/* C7  : SML1DATA ==> EN_USI_CHARGE */
	PAD_CFG_GPO(GPP_C7, 1, DEEP),
	/* C10 : UART0_RTS# ==> USI_RST_L */
	PAD_CFG_GPO(GPP_C10, 1, DEEP),
	/* C11 : UART0_CTS# ==> NOT USED */
	PAD_NC(GPP_C11, NONE),
	/* C16 : I2C0_SDA ==> PCH_I2C0_1V8_AUDIO_SDA */
	PAD_CFG_NF(GPP_C16, NONE, DEEP, NF1),
	/* C17 : I2C0_SCL ==> PCH_I2C0_1V8_AUDIO_SCL */
	PAD_CFG_NF(GPP_C17, NONE, DEEP, NF1),
	/* C18 : I2C1_SDA ==> PCH_I2C1_TOUCH_USI_SDA */
	PAD_CFG_NF(GPP_C18, NONE, DEEP, NF1),
	/* C19 : I2C1_SCL ==> PCH_I2C1_TOUCH_USI_SCL */
	PAD_CFG_NF(GPP_C19, NONE, DEEP, NF1),
	/* C20 : UART2_RXD ==> FPMCU_INT_L */
	PAD_CFG_GPI_INT(GPP_C20, NONE, PLTRST, LEVEL),
	/* C22 : UART2_RTS# ==> PCH_FPMCU_BOOT0 */
	PAD_CFG_GPO(GPP_C22, 0, DEEP),

	/* D5  : SRCCLKREQ0$ ==> SSD_CLKREQ_ODL */
	PAD_CFG_NF(GPP_D5, NONE, DEEP, NF1),
	/* D6  : SRCCLKREQ1# ==> WLAN_CLKREQ_ODL */
	PAD_CFG_NF(GPP_D6, NONE, DEEP, NF1),
	/* D8  : SRCCLKREQ3# ==> SD_CLKREQ_ODL */
	PAD_CFG_NF(GPP_D8, NONE, DEEP, NF1),
	/* D16 : ISH_UART0_CTS# ==> EN_PP3300_SD */
	PAD_CFG_GPO(GPP_D16, 1, DEEP),
	/* D17 : ISH_GP4 ==> EN_CVF_PWR */
	PAD_CFG_GPO(GPP_D17, 1, DEEP),

	/* E1  : SPI1_IO2 ==> PEN_DET_ODL */
	PAD_CFG_GPI_SCI_LOW(GPP_E1, NONE, DEEP, EDGE_SINGLE),
	/* E2  : SPI1_IO3 ==> WLAN_PCIE_WAKE_ODL */
	PAD_CFG_GPI(GPP_E2, NONE, DEEP),
	/* E3  : CPU_GP0 ==> USI_REPORT_EN */
	PAD_CFG_GPO(GPP_E3, 1, DEEP),
	/* E4  : SATA_DEVSLP0 ==> M2_SSD_PE_WAKE_ODL */
	PAD_CFG_GPI(GPP_E4, NONE, DEEP),
	/* E7  : CPU_GP1 ==> USI_INT */
	PAD_CFG_GPI_APIC(GPP_E7, NONE, PLTRST, LEVEL, NONE),
	/* E11 : SPI1_CLK ==> SD_PE_WAKE_ODL */
	PAD_CFG_GPI(GPP_E11, NONE, DEEP),
	/* E12 : SPI1_MISO_IO1 ==> PEN_ALERT_ODL */
	PAD_CFG_GPI(GPP_E12, NONE, DEEP),
	/* E15 : ISH_GP6 ==> TRACKPAD_INT_ODL */
	PAD_CFG_GPI_IRQ_WAKE(GPP_E15, NONE, DEEP, LEVEL, INVERT),

	/* F6  : CNV_PA_BLANKING ==> NC */
	PAD_NC(GPP_F6, NONE),
	/* F8  : I2S_MCLK2_INOUT ==> HP_INT_L */
	PAD_CFG_GPI_INT(GPP_F8, NONE, PLTRST, EDGE_BOTH),
	/* F13 : GSXDOUT ==> WiFi_DISABLE_L */
	PAD_CFG_GPO(GPP_F13, 1, DEEP),
	/* F19 : SRCCLKREQ6# ==> WLAN_INT_L */
	PAD_CFG_GPI_SCI_LOW(GPP_F19, NONE, DEEP, EDGE_SINGLE),

	/* H3  : SX_EXIT_HOLDOFF# ==> SD_PERST_L */
	PAD_CFG_GPO(GPP_H3, 1, DEEP),
	/* H8  : I2C4_SDA ==> NC */
	PAD_NC(GPP_H8, NONE),
	/* H9  : I2C4_SCL ==> NC */
	PAD_NC(GPP_H9, NONE),
	/* H11 : SRCCLKREQ5# ==> WLAN_PERST_L */
	PAD_CFG_GPO(GPP_H11, 1, DEEP),
	/* H16 : DDPB_CTRLCLK ==> DDPB_HDMI_CTRLCLK */
	PAD_CFG_NF(GPP_H16, NONE, DEEP, NF1),
	/* H17 : DDPB_CTRLDATA ==> DDPB_HDMI_CTRLDATA */
	PAD_CFG_NF(GPP_H17, NONE, DEEP, NF1),
	/* H19 : TIME_SYNC0 ==> USER_PRES_FP_ODL */
	PAD_CFG_GPI(GPP_H19, NONE, DEEP),

	/* R0 : HDA_BCLK ==> I2S0_HP_SCLK */
	PAD_CFG_NF(GPP_R0, NONE, DEEP, NF2),
	/* R1 : HDA_SYNC ==> I2S0_HP_SFRM */
	PAD_CFG_NF(GPP_R1, NONE, DEEP, NF2),
	/* R2 : HDA_SDO ==> I2S0_PCH_TX_HP_RX_STRAP */
	PAD_CFG_NF(GPP_R2, DN_20K, DEEP, NF2),
	/* R3 : HDA_SDIO ==> I2S0_PCH_RX_HP_TX */
	PAD_CFG_NF(GPP_R3, NONE, DEEP, NF2),
	/* R5 : HDA_SDI1 ==> I2S1_PCH_RX_SPKR_TX */
	PAD_CFG_NF(GPP_R5, NONE, DEEP, NF2),
	/* R6 : I2S1_TXD ==> I2S1_PCH_RX_SPKR_RX */
	PAD_CFG_NF(GPP_R6, NONE, DEEP, NF2),
	/* R7 : I2S1_SFRM ==> I2S1_SPKR_SFRM */
	PAD_CFG_NF(GPP_R7, NONE, DEEP, NF2),

	/* S6 : SNDW3_CLK ==> DMIC_CLK0 */
	PAD_CFG_NF(GPP_S6, NONE, DEEP, NF2),
	/* S7 : SNDW3_DATA ==> DMIC_DATA0 */
	PAD_CFG_NF(GPP_S7, NONE, DEEP, NF2),
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* C8 : UART0 RX */
	PAD_CFG_NF(GPP_C8, NONE, DEEP, NF1),
	/* C9 : UART0 TX */
	PAD_CFG_NF(GPP_C9, NONE, DEEP, NF1),

	/* A12 : SATAXPCIE1 ==> M2_SSD_PEDET */
	PAD_CFG_NF(GPP_A12, NONE, DEEP, NF1),
	/* A13 : PMC_I2C_SCL ==> BT_DISABLE_L */
	/* assert reset on reboot */
	PAD_CFG_GPO(GPP_A13, 0, DEEP),
	/* A17 : DDSP_HPDC ==> MEM_CH_SEL */
	PAD_CFG_GPI(GPP_A17, NONE, DEEP),

	/* B11 : PMCALERT# ==> PCH_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_B11, NONE, DEEP),

	/* B15 : GSPI0_CS0# ==> PCH_GSPI0_H1_TPM_CS_L */
	PAD_CFG_NF(GPP_B15, NONE, DEEP, NF1),

	/* B16 : GSPI0_CLK ==> PCH_GSPI0_H1_TPM_CLK */
	PAD_CFG_NF(GPP_B16, NONE, DEEP, NF1),

	/* B17 : GSPI0_MISO ==> PCH_GSPIO_H1_TPM_MISO */
	PAD_CFG_NF(GPP_B17, NONE, DEEP, NF1),

	/* B18 : GSPI0_MOSI ==> PCH_GSPI0_H1_TPM_MOSI_STRAP */
	PAD_CFG_NF(GPP_B18, NONE, DEEP, NF1),

	/* C0  : SMBCLK ==> EN_PP3300_WLAN */
	PAD_CFG_GPO(GPP_C0, 1, DEEP),

	/* C21 : UART2_TXD ==> H1_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_C21, NONE, PLTRST, LEVEL, INVERT),

	/* C22 : UART2_RTS# ==> PCH_FPMCU_BOOT0 */
	PAD_CFG_GPO(GPP_C22, 0, DEEP),

	/* D16 : ISH_UART0_CTS# ==> EN_PP3300_SD */
	PAD_CFG_GPO(GPP_D16, 1, DEEP),

	/* H11 : SRCCLKREQ5# ==> WLAN_PERST_L */
	PAD_CFG_GPO(GPP_H11, 1, DEEP),
};

const struct pad_config *variant_override_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

/* GPIO settings before entering S5 */
static const struct pad_config s5_sleep_gpio_table[] = {
	PAD_CFG_GPO(GPP_C23, 0, DEEP), /* FPMCU_RST_ODL */
	PAD_CFG_GPO(GPP_A21, 0, DEEP), /* EN_FP_PWR */
};

const struct pad_config *variant_sleep_gpio_table(u8 slp_typ, size_t *num)
{
	if (slp_typ == ACPI_S5) {
		*num = ARRAY_SIZE(s5_sleep_gpio_table);
		return s5_sleep_gpio_table;
	}
	*num = 0;
	return NULL;
}

/* GPIOs needed to be set in romstage. */
static const struct pad_config romstage_gpio_table[] = {
	/* Enable touchscreen, hold in reset */
	/* A8  : I2S2_SFRM ==> EN_PP3300_TOUCHSCREEN */
	PAD_CFG_GPO(GPP_A8, 1, DEEP),
	/* C10 : UART0_RTS# ==> USI_RST_L */
	PAD_CFG_GPO(GPP_C10, 0, DEEP),
};

const struct pad_config *variant_romstage_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(romstage_gpio_table);
	return romstage_gpio_table;
}
