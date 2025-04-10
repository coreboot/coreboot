/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <soc/gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>

/* Pad configuration in ramstage */
static const struct pad_config override_gpio_table[] = {
	/* GPP_D03 : [] ==> EN_PP3300_SD */
	PAD_CFG_GPO_LOCK(GPP_A17, 1, LOCK_CONFIG),
	/* GPP_B14 : [NF1: SPKR NF2: TIME_SYNC1 NF4: SATA_LED# NF5: ISH_GP6
	 * NF6: USB_C_GPP_B14] ==> ACZ_SPKR */
	PAD_CFG_NF(GPP_B14, NONE, DEEP, NF1),
	/* GPP_D5 : SRCCLKREQ0_L ==> PCIE_REFCLK_SSD1_REQ_N */
	PAD_CFG_NF(GPP_D5, NONE, DEEP, NF1),
	/* GPP_D6 : [NF1: SRCCLKREQ1# NF6: USB_C_GPP_D6] ==> SOC_GPP_E10 (NC) */
	PAD_NC(GPP_D6, NONE),
	/* GPP_D7 : SRCCLKREQ2_L ==> WLAN_CLKREQ_ODL */
	PAD_CFG_NF(GPP_D7, NONE, DEEP, NF1),
	/* GPP_D9 : [NF1: ISH_SPI_CS# NF2: DDP3_CTRLCLK NF4: TBT_LSX2_TXD NF5: BSSB_LS2_RX
	 * NF6: USB_C_GPP_D9 NF7: GSPI2_CS0#] ==> HOST_MCU_FW_UP_STRAP */
	PAD_CFG_GPO_LOCK(GPP_D9, 0, LOCK_CONFIG),
	/* GPP_E7 : [NF1: PROC_GP1 NF6: USB_C_GPP_E7] ==> SD_PERST_L */
	PAD_CFG_GPO(GPP_E7, 1, DEEP),
	/* GPP_E10 : THC0_SPI1_CS_L/GSPI0_CS0_L ==> GSPI0_SOC_FP_CS_L */
	PAD_CFG_NF(GPP_E10, NONE, DEEP, NF7),
	/* GPP_E11 : [NF2: THC0_SPI1_CLK NF6: USB_C_GPP_E11
	 * NF7: GSPI0_CLK] ==> GSPI0_SOC_FP_CLK */
	PAD_CFG_NF(GPP_E11, NONE, DEEP, NF7),
	/* GPP_E12 : THC0_SPI1_IO1/I2C0A_SDA/GSPI0_MISO ==> GSPI0_SOC_DI_FP_DO */
	PAD_CFG_NF(GPP_E12, NONE, DEEP, NF7),
	/* GPP_E13 : THC0_SPI1_IO0/I2C0A_SCL/GSPI0_MOSI ==> GSPI0_SOC_DO_FP_DI */
	PAD_CFG_NF(GPP_E13, NONE, DEEP, NF7),
	/* GPP_E15 : SRCCLK_OE8_L ==> SOC_GPP_E15 (NC) */
	PAD_NC(GPP_E15, NONE),
	/* GPP_E18 : [NF1: DDP1_CTRLCLK NF4: TBT_LSX0_TXD NF5: BSSB_LS0_RX
	 * NF6: USB_C_GPP_E18] ==> SOC_FPMCU_INT_L */
	PAD_CFG_GPI_IRQ_WAKE(GPP_E18, NONE, PWROK, LEVEL, INVERT),
	/* GPP_E20 : [NF1: DDP2_CTRLCLK NF4: TBT_LSX1_TXD NF5: BSSB_LS1_RX
	 * NF6: USB_C_GPP_E20] ==> EN_FP_PWR */
	PAD_CFG_GPO_LOCK(GPP_E20, 0, LOCK_CONFIG),
	/* GPP_E21 : DDP2_CTRLDATA/TBT_LSX1_RXD/BSSB_LS1_TX ==> FP_RST_ODL */
	PAD_CFG_GPO_LOCK(GPP_E21, 0, LOCK_CONFIG),
	/* GPP_F11 : [NF3: THC1_SPI2_CLK NF4: GSPI1_CLK
	 * NF6: USB_C_GPP_F11] ==> GSPI1_SOC_TCHSCR_CLK */
	PAD_CFG_NF_LOCK(GPP_F11, NONE, NF4, LOCK_CONFIG),
	/* GPP_F12 : [NF1: GSXDOUT NF3: THC1_SPI2_IO0 NF4: GSPI1_MOSI NF5: I2C1A_SCL
	 * NF6: USB_C_GPP_F12] ==> GSPI1_SOC_DO_TCHSCR_DI */
	PAD_CFG_NF_LOCK(GPP_F12, NONE, NF4, LOCK_CONFIG),
	/* GPP_F13 : [NF1: GSXSLOAD NF3: THC1_SPI2_IO1 NF4: GSPI1_MISIO NF5: I2C1A_SDA
	 * NF6: USB_C_GPP_F13] ==> GSPI1_SOC_DI_TCHSCR_DO */
	PAD_CFG_NF_LOCK(GPP_F13, NONE, NF4, LOCK_CONFIG),
	/* GPP_F15 : [NF1: GSXSRESET# NF3: THC1_SPI2_IO3
	 * NF6: USB_C_GPP_F15] ==> PCH_TCHSCR_REPORT_EN */
	PAD_CFG_GPO(GPP_F15, 0, PLTRST),
	/* GPP_F16 : [NF1: GSXCLK NF3: THC1_SPI2_CS# NF4: GSPI1_CS0#
	 * NF6: USB_C_GPP_F16] ==> GSPI1_SOC_TCHSCR_CS_L */
	PAD_CFG_NF_LOCK(GPP_F16, NONE, NF4, LOCK_CONFIG),
	/* GPP_F18 : [NF3: THC1_SPI2_INT# NF6: USB_C_GPP_F18] ==> TCHSCR_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_F18, NONE, DEEP, EDGE_SINGLE, INVERT),
	/* GPP_S4 : SNDW2_CLK/DMIC_CLK_B0 ==> MEM_STRAP_0 */
	PAD_CFG_GPI(GPP_S4, NONE, PLTRST),
	/* GPP_S5 : SNDW2_DATA/DMIC_CLK_B1 ==> MEM_STRAP_1 */
	PAD_CFG_GPI(GPP_S5, NONE, PLTRST),
	/* GPP_S6 : SNDW3_CLK/DMIC_CLK_A1 ==> MEM_STRAP_2 */
	PAD_CFG_GPI(GPP_S6, NONE, PLTRST),
	/* GPP_S7 : SNDW3_DATA/DMIC_DATA1 ==> MEM_STRAP_3 */
	PAD_CFG_GPI(GPP_S7, NONE, PLTRST),
};


const struct pad_config *variant_gpio_override_table(size_t *num)
{
	*num = ARRAY_SIZE(override_gpio_table);
	return override_gpio_table;
}

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* GPP_D11 : [] ==> EN_PP3300_SSD (NC) */
	PAD_NC(GPP_D11, NONE),
	/* GPP_E2  : THC0_SPI1_IO3 ==> GSC_PCH_INT_ODL */
	PAD_CFG_GPI_APIC_LOCK(GPP_E2, NONE, LEVEL, INVERT, LOCK_CONFIG),
	/* GPP_E8 : GPP_E8 ==> PCH_WP_OD */
	PAD_CFG_GPI_LOCK(GPP_E8, NONE, LOCK_CONFIG),
	/* GPP_F9 : [NF1: BOOTMPC NF6: USB_C_GPP_F9] ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_F9, 0, DEEP),
	/* F21 : EXT_PWR_GATE2# ==> NC */
	PAD_NC(GPP_F21, NONE),
	/* GPP_H8 : [NF1: I2C4_SDA NF2: CNV_MFUART2_RXD NF6: USB_C_GPP_H8] ==> PCH_I2C_GSC_SDA */
	PAD_CFG_NF(GPP_H8, NONE, DEEP, NF1),
	/* GPP_H9 : [NF1: I2C4_SCL NF2: CNV_MFUART2_TXD] ==> PCH_I2C_GSC_SCL */
	PAD_CFG_NF(GPP_H9, NONE, DEEP, NF1),
	/* H10 : UART0_RXD ==> UART_PCH_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, PLTRST, NF2),
	/* H11 : UART0_TXD ==> UART_PCH_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, PLTRST, NF2),
	/* GPP_S0 : SNDW0_CLL/I2S1_SCLK ==> MEM_CH_SEL */
	PAD_CFG_GPI(GPP_S0, NONE, DEEP),

	/* CPU PCIe VGPIO for PEG60 */
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
	/* GPP_S4 : SNDW2_CLK/DMIC_CLK_B0 ==> MEM_STRAP_0 */
	PAD_CFG_GPI(GPP_S4, NONE, PLTRST),
	/* GPP_S5 : SNDW2_DATA/DMIC_CLK_B1 ==> MEM_STRAP_1 */
	PAD_CFG_GPI(GPP_S5, NONE, PLTRST),
	/* GPP_S6 : SNDW3_CLK/DMIC_CLK_A1 ==> MEM_STRAP_2 */
	PAD_CFG_GPI(GPP_S6, NONE, PLTRST),
	/* GPP_S7 : SNDW3_DATA/DMIC_DATA1 ==> MEM_STRAP_3 */
	PAD_CFG_GPI(GPP_S7, NONE, PLTRST),
	/* GPP_F7 : [NF6: USB_C_GPP_F7] ==> EN_PP3300_TCHSCR */
	PAD_CFG_GPO(GPP_F7, 1, PLTRST),
	/* GPP_F9 : [NF1: BOOTMPC NF6: USB_C_GPP_F9] ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_F9, 1, DEEP),
	/* GPP_F17 : [NF3: THC1_SPI2_RST# NF6: USB_C_GPP_F17] ==> TCHSCR_RST_L */
	PAD_CFG_GPO(GPP_F17, 0, DEEP),
	/* GPP_S0 : SNDW0_CLL/I2S1_SCLK ==> MEM_CH_SEL */
	PAD_CFG_GPI(GPP_S0, NONE, DEEP),
};

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
