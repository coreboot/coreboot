/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <variant/gpio.h>
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
	/* A7  : I2S2_SCLK ==> I2S1_SCLK */
	PAD_CFG_NF(GPP_A7, NONE, DEEP, NF1),
	/* A8  : I2S2_SFRM ==> EN_PP3300_TOUCHSCREEN */
	PAD_CFG_GPO(GPP_A8, 1, DEEP),
	/* A9  : I2S2_TXD ==> I2S1_TXD */
	PAD_CFG_NF(GPP_A9, NONE, DEEP, NF1),
	/* A10 : I2S2_RXD ==> I2S1_RXD */
	PAD_CFG_NF(GPP_A10, NONE, DEEP, NF1),
	/* A13 : PMC_I2C_SCL ==> BT_DISABLE_L */
	PAD_CFG_GPO(GPP_A13, 1, DEEP),
	/* A14 : DDSP_HPD3 ==> USB_C2_DP_HPD */
	PAD_CFG_NF(GPP_A14, NONE, DEEP, NF2),
	/* A16 : USB_OC3# ==> USB_C0_OC_OD# */
	PAD_CFG_NF(GPP_A16, NONE, DEEP, NF1),
	/* A18 : DDSP_HPDB ==> NC */
	PAD_NC(GPP_A18, NONE),
	/* A19 : DDSP_HPD1 ==> USB_C0_DP_HPD */
	PAD_CFG_NF(GPP_A19, NONE, DEEP, NF1),
	/* A20 : DDSP_HPD2 ==> USB_C1_DP_HPD */
	PAD_CFG_NF(GPP_A20, NONE, DEEP, NF1),
	/* A22 : DDPC_CTRLDATA ==> NC */
	PAD_NC(GPP_A22, NONE),
	/* A23 : I2S1_SCLK ==> HP_INT_L */
	PAD_CFG_GPI_INT(GPP_A23, NONE, PLTRST, EDGE_BOTH),

	/* B2  : VRALERT# ==> NC */
	PAD_NC(GPP_B2, NONE),
	/* B3  : CPU_GP2 ==> NC */
	PAD_NC(GPP_B3, NONE),
	/* B5  : ISH_I2C0_CVF_SDA ==> NC */
	PAD_NC(GPP_B5, NONE),
	/* B6  : ISH_I2C0_CVF_SCL ==> NC */
	PAD_NC(GPP_B6, NONE),
	/* B7  : ISH_12C1_SDA ==> I2C_SENSOR_SDA */
	PAD_CFG_NF(GPP_B7, NONE, DEEP, NF1),
	/* B8  : ISH_I2C1_SCL ==> I2C_SENSOR_SCL */
	PAD_CFG_NF(GPP_B8, NONE, DEEP, NF1),
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

	/* C1  : SMBDATA ==> FPMCU_BOOT1 */
	PAD_CFG_GPO(GPP_C1, 0, DEEP),
	/* C7  : SML1DATA ==> NC */
	PAD_NC(GPP_C7, NONE),
	/* C10 : UART0_RTS# ==> USI_RST_L */
	PAD_CFG_GPO(GPP_C10, 1, DEEP),
	/* C11 : UART0_CTS# ==> NC */
	PAD_NC(GPP_C11, NONE),
	/* C13 : UART1_TXD ==> NC */
	PAD_NC(GPP_C13, NONE),
	/* C20 : UART2_RXD ==> FPMCU_INT_L */
	PAD_CFG_GPI_INT(GPP_C20, NONE, PLTRST, LEVEL),
	/* C22 : UART2_RTS# ==> PCH_FPMCU_BOOT0 */
	PAD_CFG_GPO(GPP_C22, 0, DEEP),

	/* D7  : SRCCLKREQ2# ==> NC */
	PAD_NC(GPP_D7, NONE),
	/* D8  : SRCCLKREQ3# ==> SD_CLKREQ_ODL */
	PAD_CFG_NF(GPP_D8, NONE, DEEP, NF1),
	/* D9  : ISH_SPI_CS# ==> TBT_LSX2_TXD */
	PAD_CFG_NF(GPP_D9, NONE, DEEP, NF4),
	/* D10 : ISH_SPI_CLK ==> TBT_LSX2_RXD */
	PAD_CFG_NF(GPP_D10, NONE, DEEP, NF4),
	/* D11 : ISH_SPI_MISO ==> NC */
	PAD_NC(GPP_D11, NONE),
	/* D12 : ISH_SPI_MOSI ==> NC */
	PAD_NC(GPP_D12, NONE),
	/* D16 : ISH_UART0_CTS# ==> EN_PP3300_SD */
	PAD_CFG_GPO(GPP_D16, 1, DEEP),

	/* E0  : SATAXPCIE0 ==> NC */
	PAD_NC(GPP_E0, NONE),
	/* E1  : SPI1_IO2 ==> NC */
	PAD_NC(GPP_E1, NONE),
	/* E2  : SPI1_IO3 ==> NC */
	PAD_NC(GPP_E2, NONE),
	/* E5  : SATA_DEVSLP1 ==> NC */
	PAD_NC(GPP_E5, NONE),
	/* E10 : SPI1_CS# ==> NC */
	PAD_NC(GPP_E10, NONE),
	/* E11 : SPI1_CLK ==> SD_PE_WAKE_ODL */
	PAD_CFG_GPI(GPP_E11, NONE, DEEP),
	/* E12 : SPI1_MISO_IO1 ==> NC */
	PAD_NC(GPP_E12, NONE),
	/* E13 : SPI1_MOSI_IO0 ==> NC */
	PAD_NC(GPP_E13, NONE),
	/* E15 : ISH_GP6 ==> TRACKPAD_INT_ODL */
	PAD_CFG_GPI_IRQ_WAKE(GPP_E15, NONE, DEEP, LEVEL, INVERT),
	/* E16 : ISH_GP7 ==> SD_PRSNT# */
	PAD_CFG_GPI(GPP_E16, NONE, DEEP),
	/* E17 : THC0_SPI1_INT# ==> NC */
	PAD_NC(GPP_E17, NONE),
	/* E22 : DDPA_CTRLCLK ==> NC */
	PAD_NC(GPP_E22, NONE),
	/* E23 : DDPA_CTRLDATA ==> NC */
	PAD_NC(GPP_E23, NONE),

	/* F8  : I2S_MCLK2_INOUT ==> NC */
	PAD_NC(GPP_F8, NONE),
	/* F10 : GPPF10_STRAP */
	PAD_NC(GPP_F10, NONE),
	/* F11 : THC1_SPI2_CLK ==> NC */
	PAD_NC(GPP_F11, NONE),
	/* F12 : GSXDOUT ==> EN_PP3300_TRACKPAD */
	PAD_CFG_GPO(GPP_F12, 1, DEEP),
	/* F13 : GSXDOUT ==> WiFi_DISABLE_L */
	PAD_CFG_GPO(GPP_F13, 1, DEEP),
	/* F14 : GSXDIN ==> NC */
	PAD_NC(GPP_F14, NONE),
	/* F15 : GSXSRESET# ==> NC */
	PAD_NC(GPP_F15, NONE),
	/* F16 : GSXCLK ==> EN_PP3300_TOUCHSCREEN */
	PAD_CFG_GPO(GPP_F16, 1, DEEP),
	/* F17 : WWAN_RF_DISABLE_ODL ==> EC_IN_RW_OD */
	PAD_CFG_GPI(GPP_F17, NONE, DEEP),
	/* F18 : THC1_SPI2_INT# ==> EN_SPKR_PA */
	PAD_CFG_GPO(GPP_F18, 1, DEEP),
	/* F19 : SRCCLKREQ6# ==> NC */
	PAD_NC(GPP_F19, NONE),

	/* H3  : SX_EXIT_HOLDOFF# ==> SD_PERST_L */
	PAD_CFG_GPO(GPP_H3, 1, DEEP),
	/* H6  : I2C3_SDA */
	PAD_CFG_NF(GPP_H6, NONE, DEEP, NF1),
	/* H7  : I2C3_SCL */
	PAD_CFG_NF(GPP_H7, NONE, DEEP, NF1),
	/* H12 : M2_SKT2_CFG0 ==> NC */
	PAD_NC(GPP_H12, NONE),
	/* H13 : M2_SKT2_CFG1 # ==> SPKR_INT# */
	PAD_CFG_GPI(GPP_H13, NONE, DEEP),
	/* H14 : M2_SKT2_CFG2 # ==> NC */
	PAD_NC(GPP_H14, NONE),
	/* H15 : M2_SKT2_CFG3 # ==> NC */
	PAD_NC(GPP_H15, NONE),
	/* H16 : DDPB_CTRLCLK ==> NC */
	PAD_NC(GPP_H16, NONE),
	/* H17 : DDPB_CTRLDATA ==> NC */
	PAD_NC(GPP_H17, NONE),
	/* H19 : TIME_SYNC0 ==> USER_PRES_FP_ODL */
	PAD_CFG_GPI(GPP_H19, NONE, DEEP),
	/* H23 : IMGCLKOUT4 ==> NC */
	PAD_NC(GPP_H23, NONE),

	/* R4 : HDA_RST# ==> NC */
	PAD_NC(GPP_R4, NONE),
	/* R5 : HDA_SDI1 ==> NC */
	PAD_NC(GPP_R5, NONE),
	/* R6 : I2S1_TXD ==> NC */
	PAD_NC(GPP_R6, NONE),
	/* R7 : I2S1_SFRM ==> NC */
	PAD_NC(GPP_R7, NONE),

	/* S4 : SNDW2_CLK ==> SOC_DMIC_CLK1 */
	PAD_CFG_NF(GPP_S4, NONE, DEEP, NF2),
	/* S5 : SNDW2_DATA ==> SOC_DMIC_DATA1 */
	PAD_CFG_NF(GPP_S5, NONE, DEEP, NF2),

	/* GPD11: LANPHYC ==> NC */
	PAD_NC(GPD11, NONE),
};

const struct pad_config *variant_override_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}

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

	/* F17 : WWAN_RF_DISABLE_ODL ==> EC_IN_RW_OD */
	PAD_CFG_GPI(GPP_F17, NONE, DEEP),

	/* H11 : SRCCLKREQ5# ==> WLAN_PERST_L */
	PAD_CFG_GPO(GPP_H11, 1, DEEP),
};

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
