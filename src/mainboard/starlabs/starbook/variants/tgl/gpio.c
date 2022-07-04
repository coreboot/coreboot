/* SPDX-License-Identifier: GPL-2.0-only */

#include <variants.h>

/*
 * All definitions are taken from a comparison of the output of "inteltool -a"
 * using the stock BIOS and with coreboot.
 */

/* Early pad configuration in bootblock */
const struct pad_config early_gpio_table[] = {
	/* C20:	UART2_RXD_R			*/
	PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),
	/* C21:	UART2_TXD_R			*/
	PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),
};

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

/* Pad configuration in ramstage. */
const struct pad_config gpio_table[] = {
	/* REFERENCE:	EP PER SCHEMATIC	*/

	/* GPD0:	PCH_BATLOW#		*/
	PAD_CFG_NF(GPD0, NONE, DEEP, NF1),
	/* GPD1:	AC_PRESENT		*/
	PAD_CFG_NF(GPD1, NONE, DEEP, NF1),
	/* GPD2:	LAN_WAKE#		*/
	PAD_NC(GPD2, NONE),
	/* GPD3:	SIO_PWRBTN#		*/
	PAD_CFG_NF(GPD3, UP_20K, DEEP, NF1),
	/* GPD4:	SIO_SLP_S3#		*/
	PAD_CFG_NF(GPD4, NONE, DEEP, NF1),
	/* GPD5:	SIO_SLP_S4#		*/
	PAD_CFG_NF(GPD5, NONE, DEEP, NF1),
	/* GPD6:	SIO_SLP_A#		*/
	PAD_CFG_NF(GPD6, NONE, DEEP, NF1),
	/* GPD7:	PCH_TBT_PERST#		*/
	PAD_CFG_GPO(GPD7, 0, PLTRST),
	/* GPD8:	SUSCLK			*/
	PAD_CFG_NF(GPD8, NONE, DEEP, NF1),
	/* GPD9:	SIO_SLP_WLAN#		*/
	PAD_CFG_NF(GPD9, NONE, DEEP, NF1),
	/* GPD10:	SIO_SLP_S5#		*/
	PAD_CFG_NF(GPD10, NONE, DEEP, NF1),
	/* GPD11:	PM_LANPHY_EN		*/
	PAD_CFG_NF(GPD11, NONE, DEEP, NF1),

	/* A0:	ESPI_IO_0			*/
	/* A1:	ESPI_IO_1			*/
	/* A2:	ESPI_IO_2			*/
	/* A3:	ESPI_IO_3			*/
	/* A4:	ESPI_CS_L			*/
	/* A5:	ESPI_CLK			*/
	/* A6:	Not Connected(TP764)		*/
	/* A7:	WLAN_PCM_CLK			*/
	PAD_NC(GPP_A7, NONE),
	/* A8:	WLAN_PCM_RST			*/
	PAD_CFG_NF(GPP_A8, NONE, DEEP, NF2),
	/* A9:	WLAN_PCM_CLKREQ0		*/
	PAD_CFG_NF(GPP_A9, NONE, DEEP, NF2),
	/* A10:	WLAN_PCM_IN			*/
	PAD_NC(GPP_A10, NONE),
	/* A11:	M2_CPU_SSD_RST_N		*/
	PAD_CFG_GPO(GPP_A11, 1, PLTRST),
	/* A12:	SATAGP_1			*/
	PAD_CFG_NF(GPP_A12, UP_20K, DEEP, NF1),
	/* A13:	Not Connected			*/
	PAD_NC(GPP_A13, NONE),
	/* A14:	Not Connected			*/
	PAD_NC(GPP_A14, NONE),
	/* A15	Not Connected			*/
	PAD_NC(GPP_A15, NONE),
	/* A16:	USB2_OCB_3			*/
	PAD_CFG_NF(GPP_A16, NONE, DEEP, NF1),
	/* A17:	Not Connected			*/
	PAD_NC(GPP_A17, NONE),
	/* A18:	DDIB_HPD			*/
	PAD_CFG_NF(GPP_A18, NONE, DEEP, NF1),
	/* A19	Not Connected			*/
	PAD_NC(GPP_A19, NONE),
	/* A20:	Not Connected			*/
	PAD_NC(GPP_A20, NONE),
	/* A21	Not Connected			*/
	PAD_NC(GPP_A21, NONE),
	/* A22:	Not Connected			*/
	PAD_NC(GPP_A22, NONE),
	/* A23:	TC_RETIMER_FORCE_PWR		*/
	PAD_CFG_GPO(GPP_A23, 0, PLTRST),

	/* B0:	CORE_VID_0			*/
	PAD_CFG_NF(GPP_B0, NONE, DEEP, NF1),
	/* B1:	CORE_VID_1			*/
	PAD_CFG_NF(GPP_B1, NONE, DEEP, NF1),
	/* B2:	VRALERT_N			*/
	PAD_NC(GPP_B2, NONE),
	/* B3:	Not Connected			*/
	PAD_NC(GPP_B3, NONE),
	/* B4:	Not Connected			*/
	PAD_NC(GPP_B4, NONE),
	/* B5:	Not Connected			*/
	PAD_NC(GPP_B5, NONE),
	/* B6:	Not Connected			*/
	PAD_NC(GPP_B6, NONE),
	/* B7:	Not Connected			*/
	PAD_NC(GPP_B7, NONE),
	/* B8:	Not Connected			*/
	PAD_NC(GPP_B8, NONE),
	/* B9:	PWR_MON_I2C_SDA_R		*/
	PAD_CFG_NF(GPP_B9, NONE, DEEP, NF1),
	/* B10:	PWR_MON_I2C_SCL_R		*/
	PAD_CFG_NF(GPP_B10, NONE, DEEP, NF1),
	/* B11:	I2C_PMC_PD_INT_N		*/
	PAD_CFG_NF(GPP_B11, NONE, DEEP, NF1),
	/* B12:	PM_SLP_S0_N			*/
	PAD_CFG_NF(GPP_B12, NONE, DEEP, NF1),
	/* B13:	PLT_RST_N			*/
	PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1),
	/* B14:	FPS_RST_N			*/
	PAD_CFG_GPO(GPP_B14, 1, PLTRST),
	/* B15:	Not Connected			*/
	PAD_NC(GPP_B15, NONE),
	/* B16:	M2_PCH_SSD_PWREN		*/
	PAD_NC(GPP_B16, NONE),
	/* B17:	Not Connected			*/
	PAD_NC(GPP_B17, NONE),
	/* B18:	UF_CAM_STROBE			*/
	PAD_CFG_GPO(GPP_B18, 0, DEEP),
	/* B19:	GSPI1_CS0_FPS_N			*/
	PAD_NC(GPP_B19, NONE),
	/* B20:	GSPI1_CLK_FPS			*/
	PAD_NC(GPP_B20, NONE),
	/* B21:	GSPI1_MISO_FPS			*/
	PAD_NC(GPP_B21, NONE),
	/* B22:	GSPI1_MOSI_FPS			*/
	PAD_CFG_GPO(GPP_B22, 0, DEEP),
	/* B23:	CPU_CLKFREQ			*/
	PAD_CFG_GPO(GPP_B23, 0, DEEP),

	/* C0:	SMBCLK				*/
	PAD_CFG_NF(GPP_C0, NONE, DEEP, NF1),
	/* C1:	SMBDATA				*/
	PAD_CFG_NF(GPP_C1, NONE, DEEP, NF1),
	/* C2:	SMBALERT_N			*/
	PAD_CFG_GPO(GPP_C2, 0, DEEP),
	/* C3:	SML0_CLK			*/
	PAD_CFG_NF(GPP_C3, NONE, DEEP, NF1),
	/* C4:	SML0_DATA			*/
	PAD_CFG_NF(GPP_C4, NONE, DEEP, NF1),
	/* C5:	SML0ALERT_IN			*/
	PAD_CFG_GPO(GPP_C5, 0, DEEP),
	/* C6:	SML1_CLK			*/
	PAD_CFG_NF(GPP_C6, NONE, PWROK, NF1),
	/* C7:	SML1_DATA			*/
	PAD_CFG_NF(GPP_C7, NONE, PWROK, NF1),
	/* C8:	CLICK_PAD_INT_N			*/
	PAD_CFG_GPI_APIC_LOW(GPP_C8, NONE, PLTRST),
	/* C9:	Not Connected			*/
	PAD_NC(GPP_C9, NONE),
	/* C10:	Not Connected			*/
	PAD_NC(GPP_C10, NONE),
	/* C11:	Not Connected			*/
	PAD_NC(GPP_C11, NONE),
	/* C12:	Not Connected			*/
	PAD_NC(GPP_C12, NONE),
	/* C13:	Not Connected			*/
	PAD_NC(GPP_C13, NONE),
	/* C14:	TPM_IRQ				*/
	PAD_CFG_NF(GPP_C14, NONE, DEEP, NF1),
	/* C15:	TPM_RST				*/
	PAD_NC(GPP_C15, NONE),
	/* C16:	I2C0_SDA			*/
	PAD_CFG_NF(GPP_C16, NONE, DEEP, NF1),
	/* C17:	I2C0_SCL			*/
	PAD_CFG_NF(GPP_C17, NONE, DEEP, NF1),
	/* C18:	TOUCH_I2C_SDA			*/
	PAD_CFG_NF(GPP_C18, NONE, DEEP, NF1),
	/* C19:	TOUCH_I2C_CLK			*/
	PAD_CFG_NF(GPP_C19, NONE, DEEP, NF1),
	/* C22:	Not Connected			*/
	PAD_NC(GPP_C22, NONE),
	/* C23:	WLAN_WAKE_N			*/
	PAD_NC(GPP_C23, NONE),

	/* D0:	ACCEL1_INT			*/
	PAD_NC(GPP_D0, NONE),
	/* D1:	ACCEL2_INT			*/
	PAD_NC(GPP_D1, NONE),
	/* D2:	Not Connected			*/
	PAD_NC(GPP_D2, NONE),
	/* D3:	Not Connected			*/
	PAD_NC(GPP_D3, NONE),
	/* D4:	Not Connected			*/
	PAD_NC(GPP_D4, NONE),
	/* D5:	CLKREQ0_M2_SSD_N		*/
	PAD_NC(GPP_D5, NONE),
	/* D6:	CLKREQ1_WLAN_N			*/
	PAD_NC(GPP_D6, NONE),
	/* D7:	LAN_CLKREQ#			*/
	PAD_NC(GPP_D7, NONE),
	/* D8:	Not Connected			*/
	PAD_NC(GPP_D8, NONE),
	/* D9:	Not Connected			*/
	PAD_NC(GPP_D9, NONE),
	/* D10:	Not Connected			*/
	PAD_NC(GPP_D10, NONE),
	/* D11:	Not Connected			*/
	PAD_NC(GPP_D11, NONE),
	/* D12:	Not Connected			*/
	PAD_NC(GPP_D12, NONE),
	/* D13:	Not Connected			*/
	PAD_NC(GPP_D13, NONE),
	/* D14:	Not Connected			*/
	PAD_NC(GPP_D14, NONE),
	/* D15:	Not Connected			*/
	PAD_NC(GPP_D15, NONE),
	/* D16:	CPU_SSD_PWREN			*/
	PAD_CFG_GPO(GPP_D16, 1, PLTRST),
	/* D17:	Not Connected			*/
	PAD_NC(GPP_D17, NONE),
	/* D18:	Not Connected			*/
	PAD_NC(GPP_D18, NONE),
	/* D19:	GPPC_D_19_WFCAM_PD_N		*/
	PAD_CFG_TERM_GPO(GPP_D19, 1, UP_20K, DEEP),

	/* E0:	SATAXPCIE_0_SATAGP_0		*/
	PAD_NC(GPP_E0, NONE),
	/* E1:	Not Connected			*/
	PAD_NC(GPP_E1, NONE),
	/* E2:	Not Connected			*/
	PAD_NC(GPP_E2, NONE),
	/* E3:	FPS_INT				*/
	PAD_CFG_NF(GPP_E3, NONE, DEEP, NF1),
	/* E4:	Not Connected			*/
	PAD_NC(GPP_E4, NONE),
	/* E5:	Not Connected			*/
	PAD_NC(GPP_E5, NONE),
	/* E6:	THC0_SPI1_RST_N_TCH_PNL		*/
	PAD_NC(GPP_E6, NONE),
	/* E7:	EC_SMI_LP_N			*/
	PAD_NC(GPP_E7, NONE),
	/* E8:	EC_SLP_S0IX_N			*/
	PAD_NC(GPP_E8, NONE),
	/* E9:	USB2_TCP01_OC_N			*/
	PAD_CFG_NF(GPP_E9, NONE, DEEP, NF1),
	/* E10:	SPI1_TCH_PNL_CS_N		*/
	PAD_NC(GPP_E10, NONE),
	/* E11:	SPI1_CLK			*/
	PAD_NC(GPP_E11, NONE),
	/* E12:	Not Connected			*/
	PAD_NC(GPP_E12, NONE),
	/* E13:	Not Connected			*/
	PAD_NC(GPP_E13, NONE),
	/* E14:	EDP_HPD				*/
	PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1),
	/* E15:	Not Connected			*/
	PAD_NC(GPP_E15, NONE),
	/* E16:	Not Connected			*/
	PAD_NC(GPP_E16, NONE),
	/* E17:	Not Connected			*/
	PAD_NC(GPP_E17, NONE),
	/* E18:	TBT_LSX0_TXD			*/
	PAD_NC(GPP_E18, NATIVE),
	/* E19:	TBT_LSX0_RXD			*/
	PAD_NC(GPP_E19, NATIVE),
	/* E20:	Not Connected			*/
	PAD_NC(GPP_E20, NONE),
	/* E21:	TBT_LSX1_RXD			*/
	PAD_NC(GPP_E21, NATIVE),
	/* E22:	Not Connected			*/
	PAD_NC(GPP_E22, NONE),
	/* E23:	Not Connected			*/
	PAD_NC(GPP_E23, NONE),

	/* F0:	CNV_BRI_DT_BT_UART0_RTS_R	*/
	PAD_CFG_NF(GPP_F0, NONE, DEEP, NF1),
	/* F1:	CNV_BRI_RSP_BT_UART0_RX_R	*/
	PAD_CFG_NF(GPP_F1, NONE, DEEP, NF1),
	/* F2:	CNV_RGI_DT_BT_UART0_TX_R	*/
	PAD_CFG_NF(GPP_F2, NONE, DEEP, NF1),
	/* F3:	CNV_RGI_RSP_BT_UART0_CTS	*/
	PAD_CFG_NF(GPP_F3, NONE, DEEP, NF1),
	/* F4:	Not Connected			*/
	PAD_NC(GPP_F4, NONE),
	/* F5:	GPPC_F5_MODEM_CLKREQ		*/
	PAD_NC(GPP_F5, NONE),
	/* F6:	Not Connected			*/
	PAD_NC(GPP_F6, NONE),
	/* F7:	BIOS_REC			*/
	PAD_CFG_GPO(GPP_F7, 1, PLTRST),
	/* F8:	Not Connected			*/
	PAD_NC(GPP_F8, NONE),
	/* F9:	Not Connected			*/
	PAD_NC(GPP_F9, NONE),
	/* F10:	GPPC_F_10			*/
	PAD_CFG_GPO(GPP_F10, 0, DEEP),
	/* F11:	Not Connected			*/
	PAD_NC(GPP_F11, NONE),
	/* F12:	Not Connected			*/
	PAD_NC(GPP_F12, NONE),
	/* F13:	Not Connected			*/
	PAD_NC(GPP_F13, NONE),
	/* F14:	Not Connected			*/
	PAD_NC(GPP_F14, NONE),
	/* F15:	Not Connected			*/
	PAD_NC(GPP_F15, NONE),
	/* F16:	Not Connected			*/
	PAD_NC(GPP_F16, NONE),
	/* F17:	TOUCH_PANEL_RESET_N		*/
	PAD_NC(GPP_F17, NONE),
	/* F18:	TOUCH_PANEL_INT_N		*/
	PAD_NC(GPP_F18, NONE),
	/* F19:	Not Connected			*/
	PAD_NC(GPP_F19, NONE),
	/* F20:	Not Connected			*/
	PAD_NC(GPP_F20, NONE),
	/* F21:	Not Connected			*/
	PAD_NC(GPP_F21, NONE),
	/* F22:	Not Connected			*/
	PAD_NC(GPP_F22, NONE),
	/* F23:	Not Connected			*/
	PAD_NC(GPP_F23, NONE),

	/* H0:	GPPC_H0_M2_SSD_RST_N		*/
	PAD_CFG_GPO(GPP_H0, 0, DEEP),
	/* H1:	GPPC_H_1			*/
	PAD_CFG_GPO(GPP_H1, 0, DEEP),
	/* H2:	GPPC_H_2			*/
	PAD_CFG_GPO(GPP_H2, 0, DEEP),
	/* H3:	Not Connected			*/
	PAD_NC(GPP_H3, NONE),
	/* H4:	GSENSOR_I2C_SDA			*/
	PAD_NC(GPP_H4, NONE),
	/* H5:	GSENSOR_I2C_SCL			*/
	PAD_NC(GPP_H5, NONE),
	/* H6:	Not Connected			*/
	PAD_NC(GPP_H6, NONE),
	/* H7:	Not Connected			*/
	PAD_NC(GPP_H7, NONE),
	/* H8:	Not Connected			*/
	PAD_NC(GPP_H8, NONE),
	/* H9:	Not Connected			*/
	PAD_NC(GPP_H9, NONE),
	/* H10:	Not Connected			*/
	PAD_NC(GPP_H10, NONE),
	/* H11:	Not Connected			*/
	PAD_NC(GPP_H11, NONE),
	/* H12:	Not Connected			*/
	PAD_NC(GPP_H12, NONE),
	/* H13:	Not Connected			*/
	PAD_NC(GPP_H13, NONE),
	/* H14:	Not Connected			*/
	PAD_NC(GPP_H14, NONE),
	/* H15:	Not Connected			*/
	PAD_NC(GPP_H15, NONE),
	/* H16:	DDIB_DDC_SCL			*/
	PAD_CFG_NF(GPP_H16, NONE, DEEP, NF1),
	/* H17:	DDIB_DDC_SDA			*/
	PAD_CFG_NF(GPP_H17, NONE, DEEP, NF1),
	/* H18:	CPU_C10_GATE_N			*/
	PAD_CFG_NF(GPP_H18, NONE, DEEP, NF1),
	/* H19:	UART_BT_WAKE_N			*/
	PAD_NC(GPP_H19, NONE),
	/* H20:	Not Connected			*/
	PAD_NC(GPP_H20, NONE),
	/* H21:	Not Connected			*/
	PAD_NC(GPP_H21, NONE),
	/* H22:	Not Connected			*/
	PAD_NC(GPP_H22, NONE),
	/* H23:	Not Connected			*/
	PAD_NC(GPP_H23, NONE),

	/* R0:	HDA_BCLK			*/
	PAD_CFG_NF(GPP_R0, NONE, DEEP, NF1),
	/* R1:	HDA_SYNC			*/
	PAD_CFG_NF(GPP_R1, NATIVE, DEEP, NF1),
	/* R2:	HDA_SDO				*/
	PAD_CFG_NF(GPP_R2, NATIVE, DEEP, NF1),
	/* R3:	HDA_SDI_0_SSP0_RXD		*/
	PAD_CFG_NF(GPP_R3, NATIVE, DEEP, NF1),
	/* R4:	Not Connected			*/
	PAD_NC(GPP_R4, NONE),
	/* R5:	Not Connected			*/
	PAD_NC(GPP_R5, NONE),
	/* R6:	Not Connected			*/
	PAD_NC(GPP_R6, NONE),
	/* R7:	Not Connected			*/
	PAD_NC(GPP_R7, NONE),

	/* S0:	Not Connected			*/
	PAD_NC(GPP_S0, NONE),
	/* S1:	Not Connected			*/
	PAD_NC(GPP_S1, NONE),
	/* S2:	Not Connected			*/
	PAD_NC(GPP_S2, NONE),
	/* S3:	Not Connected			*/
	PAD_NC(GPP_S3, NONE),
	/* S4:	Not Connected			*/
	PAD_NC(GPP_S4, NONE),
	/* S5:	Not Connected			*/
	PAD_NC(GPP_S5, NONE),
	/* S6:	Not Connected			*/
	PAD_NC(GPP_S6, NONE),
	/* S7:	Not Connected			*/
	PAD_NC(GPP_S7, NONE),

	/* T2:	Not Connected			*/
	PAD_NC(GPP_T2, NONE),
	/* T3:	Not Connected			*/
	PAD_NC(GPP_T3, NONE),

	/* U4:	Not Connected			*/
	PAD_NC(GPP_U4, NONE),
	/* U5:	Not Connected			*/
	PAD_NC(GPP_U5, NONE),
};

const struct pad_config *variant_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}
