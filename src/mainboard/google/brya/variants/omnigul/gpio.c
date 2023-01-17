/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <soc/gpio.h>

/* Pad configuration in ramstage */
static const struct pad_config override_gpio_table[] = {
	/* A0 thru A5, A9 and A10 come configured out of reset, do not touch */
	/* A0  : ESPI_IO0 ==> ESPI_PCH_D0_EC_R */
	/* A1  : ESPI_IO1 ==> ESPI_PCH_D1_EC_R */
	/* A2  : ESPI_IO2 ==> ESPI_PCH_D2_EC_R */
	/* A3  : ESPI_IO3 ==> ESPI_PCH_D3_EC_R */
	/* A4  : ESPI_CS# ==> ESPI_PCH_CS_EC_R_L */
	/* A5  : ESPI_ALERT0# ==> NC  */
	/* A6  : ESPI_ALERT1# ==> NC  */
	PAD_NC(GPP_A6, NONE),
	/* A7  : SRCCLK_OE7# ==> GPP_A7 */
	PAD_CFG_GPI(GPP_A7, NONE, DEEP),
	/* A8  : SRCCLKREQ7# ==> NC */
	PAD_NC(GPP_A8, NONE),
	/* A9  : ESPI_CLK ==> ESPI_PCH_CLK_R */
	/* A10 : ESPI_RESET# ==> ESPI_PCH_RST_EC_L */
	/* A11 : PMC_I2C_SDA ==> EN_SPKR_PA */
	/* A12 : SATAXPCIE1 ==> NC */
	PAD_NC(GPP_A12, NONE),
	/* A13 : PMC_I2C_SCL ==> GSC_PCH_INT_ODL */
	/* A14 : USB_OC1# ==> USB_C1_OC_ODL */
	/* A15 : USB_OC2# ==> NC */
	PAD_NC(GPP_A15, NONE),
	/* A16 : USB_OC3# ==> USB_A0_OC_ODL */
	/* A17 : DISP_MISCC ==> EN_FCAM_PWR */
	/* A18 : DDSP_HPDB ==> HDMI_HPD */
	/* A19 : DDSP_HPD1 ==> NC */
	PAD_NC(GPP_A19, NONE),
	/* A20 : DDSP_HPD2 ==> NC */
	PAD_NC(GPP_A20, NONE),
	/* A21 : DDPC_CTRCLK ==> NC */
	PAD_NC(GPP_A21, NONE),
	/* A22 : DDPC_CTRLDATA ==> NC */
	PAD_NC(GPP_A22, NONE),
	/* A23 : ESPI_CS1# ==> AUD_HP_INT_L */

	/* B0  : SOC_VID0 */
	/* B1  : SOC_VID1 */
	/* B2  : VRALERT# ==> NC */
	PAD_NC_LOCK(GPP_B2, NONE, LOCK_CONFIG),
	/* B3  : PROC_GP2 ==> GPP_B3 */
	PAD_CFG_GPI_LOCK(GPP_B3, NONE, LOCK_CONFIG),
	/* B4  : PROC_GP3 ==> SSD_PERST_L */
	/* B5  : ISH_I2C0_SDA ==> NC */
	PAD_NC_LOCK(GPP_B5, NONE, LOCK_CONFIG),
	/* B6  : ISH_I2C0_SCL ==> NC */
	PAD_NC_LOCK(GPP_B6, NONE, LOCK_CONFIG),
	/* B7  : ISH_12C1_SDA ==> PCH_I2C_TCHSCR_R_SDA */
	/* B8  : ISH_I2C1_SCL ==> PCH_I2C_TCHSCR_R_SCL */
	/* B9  : NC */
	/* B10 : NC */
	/* B11 : PMCALERT# ==> EN_PP3300_WLAN */
	/* B12 : SLP_S0# ==> SLP_S0_L */
	/* B13 : PLTRST# ==> PLT_RST_L */
	/* B14 : SPKR ==> GPP_B14_STRAP */
	/* B15 : TIME_SYNC0 ==> FP_USER_PRES_FP_L */
	/* B16 : I2C5_SDA ==> PCH_I2C_TCHPAD_SDA */
	/* B17 : I2C5_SCL ==> PCH_I2C_TCHPAD_SCL */
	/* B18 : ADR_COMPLETE ==> GPP_B18_STRAP */
	/* B19 : NC */
	/* B20 : NC */
	/* B21 : NC */
	/* B22 : NC */
	/* B23 : SML1ALERT# ==> PCHHOT_ODL_STRAP */

	/* C0  : SMBCLK ==> EN_PP3300_TCHSCR */
	/* C1  : SMBDATA ==> USI_RST_L */
	/* C2  : SMBALERT# ==> GPP_C2_STRAP */
	/* C3 : SML0CLK ==> NC */
	PAD_NC(GPP_C3, NONE),
	/* C4 : SML0DATA ==> NC */
	PAD_NC(GPP_C4, NONE),
	/* C5  : SML0ALERT# ==> GPP_C5_BOOT_STRAP0 */
	/* C6  : SML1CLK ==> USI_REPORT_EN */
	/* C7  : SML1DATA ==> USI_INT */

	/* D0  : ISH_GP0 ==> PCH_FP_BOOT0 */
	/* D1  : ISH_GP1 ==> FP_RST_ODL */
	/* D2  : ISH_GP2 ==> EN_FP_PWR */
	/* D3  : ISH_GP3 ==> NC */
	PAD_NC_LOCK(GPP_D3, NONE, LOCK_CONFIG),
	/* D4  : IMGCLKOUT0 ==> BT_DISABLE_L */
	/* D5  : SRCCLKREQ0# ==> GPP_D5 */
	PAD_CFG_GPI(GPP_D5, NONE, DEEP),
	/* D6  : SRCCLKREQ1# ==> SSD_CLKREQ_ODL */
	/* D7  : SRCCLKREQ2# ==> WLAN_CLKREQ_ODL */
	/* D8  : SRCCLKREQ3# ==> GPP_D8 */
	PAD_CFG_GPI(GPP_D8, NONE, DEEP),
	/* D9  : ISH_SPI_CS# ==> NC */
	PAD_NC_LOCK(GPP_D9, NONE, LOCK_CONFIG),
	/* D10 : ISH_SPI_CLK ==> GPP_D10_STRAP */
	PAD_NC_LOCK(GPP_D10, NONE, LOCK_CONFIG),
	/* D11 : ISH_SPI_MISO ==> EN_PP3300_SSD */
	/* D12 : ISH_SPI_MOSI ==> GPP_D12_STRAP */
	/* D13 : ISH_UART0_RXD ==> GPP_D13 */
	PAD_CFG_GPI_LOCK(GPP_D13, NONE, LOCK_CONFIG),
	/* D14 : ISH_UART0_TXD ==> NC */
	PAD_NC_LOCK(GPP_D14, NONE, LOCK_CONFIG),
	/* D15 : ISH_UART0_RTS# ==> GPP_D15 */
	PAD_NC_LOCK(GPP_D15, NONE, LOCK_CONFIG),
	/* D16 : ISH_UART0_CTS# ==> GPP_D16 */
	PAD_NC_LOCK(GPP_D16, NONE, LOCK_CONFIG),
	/* D17 : UART1_RXD ==> GPP_D17 */
	PAD_CFG_GPI_LOCK(GPP_D17, NONE, LOCK_CONFIG),
	/* D18 : UART1_TXD ==> NC */
	PAD_NC_LOCK(GPP_D18, NONE, LOCK_CONFIG),
	/* D19 : I2S_MCLK1_OUT ==> I2S_MCLK_R */

	/* E0  : SATAXPCIE0 ==> NC */
	PAD_NC(GPP_E0, NONE),
	/* E1  : THC0_SPI1_IO2 ==> MEM_STRAP_2 */
	/* E2  : THC0_SPI1_IO3 ==> MEM_STRAP_1 */
	/* E3  : PROC_GP0 ==> GPP_E3 */
	PAD_CFG_GPI(GPP_E3, NONE, DEEP),
	/* E4  : SATA_DEVSLP0 ==> NC */
	PAD_NC(GPP_E4, NONE),
	/* E5  : SATA_DEVSLP1 ==> USB_A0_RT_RST_ODL */
	/* E6  : THC0_SPI1_RST# ==> GPPE6_STRAP */
	/* E7  : PROC_GP1 ==> GPP_E7 */
	PAD_CFG_GPI(GPP_E7, NONE, DEEP),
	/* E8  : SLP_DRAM# ==> WIFI_DISABLE_L */
	/* E9  : USB_OC0# ==> USB_C0_OC_ODL */
	/* E10 : THC0_SPI1_CS# ==> GPP_E10 */
	PAD_CFG_GPI_LOCK(GPP_E10, NONE, LOCK_CONFIG),
	/* E11 : THC0_SPI1_CLK ==> MEM_STRAP_0 */
	/* E12 : THC0_SPI1_IO1 ==> MEM_STRAP_3 */
	/* E13 : THC0_SPI1_IO2 ==> MEM_CH_SEL */
	/* E14 : DDSP_HPDA ==> SOC_EDP_HPD */
	/* E15 : RSVD_TP ==> PCH_WP_OD */
	/* E16 : RSVD_TP ==> NC */
	PAD_NC(GPP_E16, NONE),
	/* E17 : THC0_SPI1_INT# ==> GPP_E17 */
	PAD_CFG_GPI_LOCK(GPP_E17, NONE, LOCK_CONFIG),
	/* E18 : DDP1_CTRLCLK ==> NC */
	PAD_NC(GPP_E18, NONE),
	/* E19 : DDP1_CTRLDATA ==> GPP_E19_STRAP */
	PAD_CFG_GPI(GPP_E19, NONE, DEEP),
	/* E20 : DDP2_CTRLCLK ==> NC */
	PAD_NC(GPP_E20, NONE),
	/* E21 : DDP2_CTRLDATA ==> USB_C1_LSX_SOC_RX_STRAP */
	/* E22 : DDPA_CTRLCLK ==> USB_C0_AUX_DC_STRAP_P */
	/* E23 : DDPA_CTRLDATA ==> USB_C0_AUX_DC_N */

	/* F0  : CNV_BRI_DT ==> CNV_BRI_DT_STRAP */
	/* F1  : CNV_BRI_RSP ==> CNV_BRI_RSP */
	/* F2  : CNV_RGI_DT ==> CNV_RGI_DT_STRAP */
	/* F3  : CNV_RGI_RSP ==> CNV_RGI_RSP */
	/* F4  : CNV_RF_RESET# ==> CNV_RF_RST_L */
	/* F5  : MODEM_CLKREQ ==> CNV_CLKREQ0 */
	/* F6  : CNV_PA_BLANKING ==> NC */
	PAD_NC(GPP_F6, NONE),
	/* F7  : GPPF7_STRAP */
	/* F8  : NC */
	/* F9  : BOOTMPC ==> SLP_S0_GATE_R */
	/* F10 : GPPF10_STRAP */
	/* F11 : THC1_SPI2_CLK ==> GSPI_PCH_CLK_FPMCU_R */
	/* F12 : GSXDOUT ==> GSPI_PCH_DO_FPMCU_DI_R */
	/* F13 : GSXDOUT ==> GSPI_PCH_DI_FPMCU_DO */
	/* F14 : GSXDIN ==> TCHPAD_INT_ODL */
	/* F15 : GSXSRESET# ==> FPMCU_INT_L */
	/* F16 : GSXCLK ==> GSPI_PCH_CS_FPMCU_R_L */
	/* F17 : THC1_SPI2_RST# ==> EC_PCH_INT_ODL */
	/* F18 : THC1_SPI2_INT# ==> EC_IN_RW_OD */
	/* F19 : SRCCLKREQ6# ==> NC */
	PAD_NC(GPP_F19, NONE),
	/* F20 : EXT_PWR_GATE# ==> NC */
	PAD_NC(GPP_F20, NONE),
	/* F21 : EXT_PWR_GATE2# ==> GPP_F21 */
	PAD_CFG_GPI(GPP_F21, NONE, DEEP),
	/* F22 : NC ==> GPP_F22 */
	/* F23 : V1P05_CTRL ==> V1P05EXT_CTRL */
	PAD_CFG_NF(GPP_F23, NONE, DEEP, NF1),

	/* H0  : GPPH0_BOOT_STRAP1 */
	/* H1  : GPPH1_BOOT_STRAP2 */
	/* H2  : GPPH2_BOOT_STRAP3 */
	/* H3  : SX_EXIT_HOLDOFF# ==> WLAN_PCIE_WAKE_ODL */
	/* H4  : I2C0_SDA ==> PCH_I2C_AUD_SDA */
	/* H5  : I2C0_SCL ==> PCH_I2C_AUD_SCL */
	/* H6  : I2C1_SDA ==> PCH_I2C_TPM_SDA */
	/* H7  : I2C1_SCL ==> PCH_I2C_TPM_SCL */
	/* H8  : I2C4_SDA ==> NC */
	PAD_NC(GPP_H8, NONE),
	/* H9  : I2C4_SCL ==> NC */
	PAD_NC(GPP_H9, NONE),
	/* H10 : UART0_RXD ==> UART_PCH_RX_DBG_TX */
	/* H11 : UART0_RXD ==> UART_PCH_TX_DBG_RX */
	/* H12 : I2C7_SDA ==> GPP_H12 */
	PAD_CFG_GPI_LOCK(GPP_H12, NONE, LOCK_CONFIG),
	/* H13 : I2C7_SCL ==> NC */
	PAD_NC_LOCK(GPP_H13, NONE, LOCK_CONFIG),
	/* H14 : NC */
	/* H15 : DDPB_CTRLCLK ==> DDIB_HDMI_CTRLCLK */
	/* H16 : NC */
	/* H17 : DDPB_CTRLDATA ==> DDIB_HDMI_CTRLDATA */
	/* H18 : PROC_C10_GATE# ==> CPU_C10_GATE_L */
	/* H19 : SRCCLKREQ4# ==> GPP_H19 */
	PAD_CFG_GPI(GPP_H19, NONE, DEEP),
	/* H20 : IMGCLKOUT1 ==> WLAN_PERST_L */
	/* H21 : IMGCLKOUT2 ==>  NC */
	PAD_NC(GPP_H21, NONE),
	/* H22 : IMGCLKOUT3 ==> NC */
	PAD_NC(GPP_H22, NONE),
	/* H23 : SRCCLKREQ5# ==> GPP_H23 */
	PAD_CFG_GPI(GPP_H23, NONE, DEEP),

	/* R0 : HDA_BCLK ==> I2S_HP_SCLK_R */
	/* R1 : HDA_SYNC ==> I2S_HP_SFRM_R */
	/* R2 : HDA_SDO ==> I2S_PCH_TX_HP_RX_STRAP */
	/* R3 : HDA_SDIO ==> I2S_PCH_RX_HP_TX */
	/* R4 : HDA_RST# ==> DMIC_CLK1_R */
	PAD_CFG_NF(GPP_R4, NONE, DEEP, NF3),
	/* R5 : HDA_SDI1 ==> DMIC_DATA1_R */
	PAD_CFG_NF(GPP_R5, NONE, DEEP, NF3),
	/* R6 : I2S2_TXD ==> DMIC_CLK0_R */
	PAD_CFG_NF(GPP_R6, NONE, DEEP, NF3),
	/* R7 : I2S2_RXD ==> DMIC_DATA0_R */
	PAD_CFG_NF(GPP_R7, NONE, DEEP, NF3),

	/* S0 : SNDW0_CLK ==> I2S_SPKR_SCLK_R */
	PAD_CFG_NF(GPP_S0, NONE, DEEP, NF4),
	/* S1 : SNDW0_DATA ==> I2S_SPKR_SFRM_R */
	PAD_CFG_NF(GPP_S1, NONE, DEEP, NF4),
	/* S2 : SNDW1_CLK ==> I2S_PCH_TX_SPKR_RX_R */
	PAD_CFG_NF(GPP_S2, NONE, DEEP, NF4),
	/* S3 : SNDW1_DATA ==> I2S_PCH_RX_SPKR_TX */
	PAD_NC(GPP_S3, NONE),
	/* S4 : SNDW2_CLK ==> NC */
	PAD_NC(GPP_S4, NONE),
	/* S5 : SNDW2_DATA ==> NC */
	PAD_NC(GPP_S5, NONE),
	/* S6 : SNDW3_CLK ==> NC  */
	PAD_NC(GPP_S6, NONE),
	/* S7 : SNDW3_DATA ==> NC */
	PAD_NC(GPP_S7, NONE),

	/* GPD0: BATLOW# ==> BATLOW_L */
	/* GPD1: ACPRESENT ==> PCH_ACPRESENT */
	/* GPD2 : LAN_WAKE# ==> EC_PCH_WAKE_ODL */
	/* GPD3: PWRBTN# ==> EC_PCH_PWR_BTN_ODL */
	/* GPD4: SLP_S3# ==> SLP_S3_R_L */
	/* GPD5: SLP_S4# ==> SLP_S4_L */
	/* GPD6: SLP_A# ==> SLP_A_L_CAP_SITE */
	/* GPD7: GPD7_STRAP */
	/* GPD8: SUSCLK ==> PCH_SUSCLK */
	/* GPD9: SLP_WLAN# ==> SLP_WLAN_L */
	/* GPD10: SLP_S5# ==> SLP_S5_L */
	/* GPD11: LANPHYC ==> GPD11 */
	PAD_NC(GPD11, NONE),

	/* Configure the virtual CNVi Bluetooth I2S GPIO pads */
	/* BT_I2S_BCLK */
	PAD_CFG_NF(GPP_VGPIO_30, NONE, DEEP, NF3),
	/* BT_I2S_SYNC */
	PAD_CFG_NF(GPP_VGPIO_31, NONE, DEEP, NF3),
	/* BT_I2S_SDO */
	PAD_CFG_NF(GPP_VGPIO_32, NONE, DEEP, NF3),
	/* BT_I2S_SDI */
	PAD_CFG_NF(GPP_VGPIO_33, NONE, DEEP, NF3),
	/* SSP2_SCLK */
	PAD_CFG_NF(GPP_VGPIO_34, NONE, DEEP, NF1),
	/* SSP2_SFRM */
	PAD_CFG_NF(GPP_VGPIO_35, NONE, DEEP, NF1),
	/* SSP_TXD */
	PAD_CFG_NF(GPP_VGPIO_36, NONE, DEEP, NF1),
	/* SSP_RXD */
	PAD_CFG_NF(GPP_VGPIO_37, NONE, DEEP, NF1),
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* A13 : PMC_I2C_SCL ==> GSC_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),
	/* B4  : PROC_GP3 ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 0, DEEP),
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
	/* D11 : SATAXPCIE1 ==> EN_PP3300_SSD */
	PAD_CFG_GPO(GPP_D11, 1, DEEP),
	/* E13 : THC0_SPI1_IO2 ==> MEM_CH_SEL */
	PAD_CFG_GPI(GPP_E13, NONE, DEEP),
	/* E15 : RSVD_TP ==> PCH_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER_LOCK(GPP_E15, NONE, LOCK_CONFIG),
	/* F18 : THC1_SPI2_INT# ==> EC_IN_RW_OD */
	PAD_CFG_GPI_LOCK(GPP_F18, NONE, LOCK_CONFIG),
	/* H6  : I2C1_SDA ==> PCH_I2C_TPM_SDA */
	PAD_CFG_NF(GPP_H6, NONE, DEEP, NF1),
	/* H7  : I2C1_SCL ==> PCH_I2C_TPM_SCL */
	PAD_CFG_NF(GPP_H7, NONE, DEEP, NF1),
	/* H10 : UART0_RXD ==> UART_PCH_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* H11 : UART0_TXD ==> UART_PCH_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
};

static const struct pad_config romstage_gpio_table[] = {
	/*
	 * B4  : PROC_GP3 ==> SSD_PERST_L
	 * B4 is programmed here so that it is sequenced after EN_PP3300_SSD.
	 */
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
