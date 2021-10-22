/* SPDX-License-Identifier: GPL-2.0-only */

#include <variants.h>

/*
 * All definitions are taken from a comparison of the output of "inteltool -a"
 * using the stock BIOS and with coreboot.
 */

/* Early pad configuration in bootblock. */
const struct pad_config early_gpio_table[] = {
	/* C20:	 UART2_RXD			*/
	PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),
	/* C21:	 UART2_TXD			*/
	PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),
};

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

/* Pad configuration in ramstage. */
const struct pad_config gpio_table[] = {
	/* GPD0:	PM_BATLOW_N		*/
	PAD_CFG_NF(GPD0, UP_20K, PWROK, NF1),
	/* GPD1:	AC_PRESENT		*/
	PAD_CFG_NF(GPD1, NATIVE, PWROK, NF1),
	/* GPD2:	Not Connected		*/
	PAD_NC(GPD2, NONE),
	/* GPD3:	SOC_PWRBTN_N		*/
	PAD_CFG_NF(GPD3, UP_20K, PWROK, NF1),
	/* GPD4:	SLP_S3_N		*/
	PAD_CFG_NF(GPD4, NONE, PWROK, NF1),
	/* GPD5:	SLP_S4_N		*/
	PAD_CFG_NF(GPD5, NONE, PWROK, NF1),
	/* GPD6:	Not Connected		*/
	PAD_NC(GPD6, NONE),
	/* GPD7:	Not Connected		*/
	PAD_NC(GPD7, NONE),
	/* GPD8:	SUS_CLK			*/
	PAD_CFG_NF(GPD8, NONE, PWROK, NF1),
	/* GPD9:	Not Connected		*/
	PAD_NC(GPD9, NONE),
	/* GPD10:	Not Connected		*/
	PAD_NC(GPD10, NONE),
	/* GPD11:	Not Connected		*/
	PAD_NC(GPD11, NONE),
	/* GPP_A0:	KBRST_N			*/
	PAD_NC(GPP_A0, NONE),
	/* GPP_A1:	LDC_AD0			*/
	PAD_CFG_NF(GPP_A1, NONE, DEEP, NF1),
	/* GPP_A2:	LPC_AD1			*/
	PAD_CFG_NF(GPP_A2, NONE, DEEP, NF1),
	/* GPP_A3:	LDC_AD2			*/
	PAD_CFG_NF(GPP_A3, NONE, DEEP, NF1),
	/* GPP_A4:	LDC_AD3			*/
	PAD_CFG_NF(GPP_A4, NONE, DEEP, NF1),
	/* GPP_A5:	LPC_FRAME_N		*/
	PAD_CFG_NF(GPP_A5, NONE, DEEP, NF1),
	/* GPP_A6:	LPC_SERIRQ		*/
	PAD_CFG_NF(GPP_A6, NONE, DEEP, NF1),
	/* GPP_A7:	Not Connected		*/
	PAD_NC(GPP_A7, NONE),
	/* GPP_A8:	PM_CLKRUN_N		*/
	PAD_CFG_NF(GPP_A8, NONE, DEEP, NF1),
	/* GPP_A9:	LPC_CLK_EC		*/
	PAD_CFG_NF(GPP_A9, DN_20K, DEEP, NF1),
	/* GPP_A10:	LPC_CLK_Debug		*/
	PAD_NC(GPP_A10, DN_20K),
	/* GPP_A11:	PME_N			*/
	PAD_CFG_GPI(GPP_A11, DN_20K, DEEP),
	/* GPP_A12:	Not Connected		*/
	PAD_NC(GPP_A12, NONE),
	/* GPP_A13:	SUSPWRDNACK		*/
	PAD_NC(GPP_A13, DN_20K),
	/* GPP_A14:	PM_SUS_STAT_N		*/
	PAD_NC(GPP_A14, DN_20K),
	/* GPP_A15:	SUSACK#			*/
	PAD_NC(GPP_A15, DN_20K),
	/* GPP_A16:	Not Connected		*/
	PAD_NC(GPP_A16, DN_20K),
	/* GPP_A17:	Not Connected		*/
	PAD_NC(GPP_A17, DN_20K),
	/* GPP_A18:	Not Connected		*/
	PAD_NC(GPP_A18, DN_20K),
	/* GPP_A19:	Not Connected		*/
	PAD_NC(GPP_A19, DN_20K),
	/* GPP_A20:	Not Connected		*/
	PAD_NC(GPP_A20, NONE),
	/* GPP_A21:	Not Connected		*/
	PAD_NC(GPP_A21, DN_20K),
	/* GPP_A22:	FP_SSP0_INT		*/
	PAD_NC(GPP_A22, DN_20K),
	/* GPP_A23:	FP_SSP0_RST		*/
	PAD_NC(GPP_A23, DN_20K),
	/* GPP_B0:	Not Connected		*/
	PAD_NC(GPP_B0, DN_20K),
	/* GPP_B1:	Not Connected		*/
	PAD_NC(GPP_B1, DN_20K),
	/* GPP_B2:	+V3P3A_PCH		*/
	PAD_NC(GPP_B2, DN_20K),
	/* GPP_B3:	Not Connected		*/
	PAD_NC(GPP_B3, DN_20K),
	/* GPP_B4:	BT_OFF_N_MCP		*/
	PAD_CFG_TERM_GPO(GPP_B4, 1, UP_20K, DEEP),
	/* GPP_B5:	SRCCLKREQ0#		*/
	PAD_CFG_NF(GPP_B5, NONE, DEEP, NF1),
	/* GPP_B6:	CLKREQ1#		*/
	PAD_CFG_GPI(GPP_B6, DN_20K, DEEP),
	/* GPP_B7:	CLKREQ2#		*/
	PAD_CFG_NF(GPP_B7, DN_20K, DEEP, NF1),
	/* GPP_B8:	CLKREQ3#		*/
	PAD_CFG_NF(GPP_B8, DN_20K, DEEP, NF1),
	/* GPP_B9:	WLAN_CLKREQ#		*/
	PAD_CFG_NF(GPP_B9, NONE, DEEP, NF1),
	/* GPP_B10:	CLKREQ5#		*/
	PAD_CFG_NF(GPP_B10, DN_20K, DEEP, NF1),
	/* GPP_B11:	Not Connected		*/
	PAD_NC(GPP_B11, DN_20K),
	/* GPP_B12	SLP_S0_N		*/
	PAD_NC(GPP_B12, DN_20K),
	/* GPP_B13:	SYS_RESET#		*/
	PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1),
	/* GPP_B14:	HDA_SPKR		*/
	PAD_NC(GPP_B14, DN_20K),
	/* GPP_B15:	Not Connected		*/
	PAD_NC(GPP_B15, DN_20K),
	/* GPP_B16:	Not Connected		*/
	PAD_NC(GPP_B16, DN_20K),
	/* GPP_B17:	Not Connected		*/
	PAD_NC(GPP_B17, DN_20K),
	/* GPP_B18:	+V3P3A_PCH		*/
	PAD_NC(GPP_B18, DN_20K),
	/* GPP_B19:	FP_SSP0_CS		*/
	PAD_NC(GPP_B19, DN_20K),
	/* GPP_B20:	FP_SSP0_CLK		*/
	PAD_NC(GPP_B20, DN_20K),
	/* GPP_21:	FP_SSP0_MISO		*/
	PAD_NC(GPP_B21, DN_20K),
	/* GPP_22:	FP_SSP0_MOSI		*/
	PAD_NC(GPP_B22, DN_20K),
	/* GPP_B23:	SML1ALERT#/PCHHOT#	*/
	PAD_NC(GPP_B23, DN_20K),
	/*GPP_C0:	SMBCLK			*/
	PAD_CFG_NF(GPP_C0, UP_20K, DEEP, NF1),
	/* GPP_C1:	SMBDATA			*/
	PAD_CFG_NF(GPP_C1, UP_20K, DEEP, NF1),
	/* GPP_C2:	SMBALERT#		*/
	PAD_NC(GPP_C2, DN_20K),
	/* GPP_C3:	SML0CLK			*/
	PAD_NC(GPP_C3, DN_20K),
	/* GPP_C4:	SML0DATA		*/
	PAD_NC(GPP_C4, DN_20K),
	/* GPP_C5:	SML0ALERT#		*/
	PAD_NC(GPP_C5, DN_20K),
	/* GPP_C6:	SML1CLK			*/
	PAD_NC(GPP_C6, DN_20K),
	/* GPP_C7:	SML1DATA		*/
	PAD_NC(GPP_C7, DN_20K),
	/* GPP_C8:	UART0_RXD		*/
	PAD_CFG_NF(GPP_C8, UP_20K, DEEP, NF1),
	/* GPP_C9:	UART0_TXD		*/
	PAD_CFG_NF(GPP_C9, UP_20K, DEEP, NF1),
	/* GPP_C10:	UART0_RTS#		*/
	PAD_CFG_NF(GPP_C10, NONE, DEEP, NF1),
	/* GPP_C11:	UART0_CTS#		*/
	PAD_CFG_NF(GPP_C11, UP_20K, DEEP, NF1),
	/* GPP_C12:	Not Connected		*/
	PAD_NC(GPP_C12, UP_20K),
	/* GPP_C13:	Not Connected		*/
	PAD_NC(GPP_C13, UP_20K),
	/* GPP_C14:	Not Connected		*/
	PAD_NC(GPP_C14, UP_20K),
	/* GPP_C15:	Not Connected		*/
	PAD_NC(GPP_C15, UP_20K),
	/* GPP_C16:	DAT_I2C_TP		*/
	PAD_CFG_NF(GPP_C16, NONE, DEEP, NF1),
	/* GPP_C17:	CLK_I2C_TP		*/
	PAD_CFG_NF(GPP_C17, NONE, DEEP, NF1),
	/* GPP_C18:	TOUCH_I2C_SDA		*/
	PAD_CFG_NF(GPP_C18, NONE, DEEP, NF1),
	/* GPP_C19:	TOUCH_I2C_CLK		*/
	PAD_CFG_NF(GPP_C19, NONE, DEEP, NF1),
	/* GPP_C22:	Not Connected		*/
	PAD_NC(GPP_C22, NONE),
	/* GPP_C23:	TOUCHPAD_INT		*/
	PAD_CFG_GPI_APIC_LOW(GPP_C23, NONE, PLTRST),
	/* GPP_D0:	Not Connected		*/
	PAD_NC(GPP_D0, DN_20K),
	/* GPP_D1:	Not Connected		*/
	PAD_NC(GPP_D1, DN_20K),
	/* GPP_D2:	Not Connected		*/
	PAD_NC(GPP_D2, DN_20K),
	/* GPP_D3:	Not Connected		*/
	PAD_NC(GPP_D3, DN_20K),
	/* GPP_D4:	Not Connected		*/
	PAD_NC(GPP_D4, DN_20K),
	/* GPP_D5:	Not Connected		*/
	PAD_NC(GPP_D5, DN_20K),
	/* GPP_D6:	Not Connected		*/
	PAD_NC(GPP_D6, DN_20K),
	/* GPP_D7:	Not Connected		*/
	PAD_NC(GPP_D7, DN_20K),
	/* GPP_D8:	Not Connected		*/
	PAD_NC(GPP_D8, DN_20K),
	/* GPP_D9:	VOLUME_UP		*/
	PAD_NC(GPP_D9, DN_20K),
	/* GPP_D10:	VOLUME_DOWN		*/
	PAD_NC(GPP_D10, DN_20K),
	/* GPP_D11:	Not Connected		*/
	PAD_NC(GPP_D11, DN_20K),
	/* GPP_D12:	Not Connected		*/
	PAD_NC(GPP_D12, DN_20K),
	/* GPP_D13:	Not Connected		*/
	PAD_NC(GPP_D13, DN_20K),
	/* GPP_D14:	GPP_D14			*/
	PAD_NC(GPP_D14, DN_20K),
	/* GPP_D15:	GPP_D15			*/
	PAD_NC(GPP_D15, DN_20K),
	/* GPP_D16:	GPP_D16			*/
	PAD_NC(GPP_D16, DN_20K),
	/* GPP_D17:	PCH_AUDIO_PWREN		*/
	PAD_NC(GPP_D17, DN_20K),
	/* GPP_D18:	Not Connected		*/
	PAD_NC(GPP_D18, DN_20K),
	/* GPP_D19:	Not Connected		*/
	PAD_NC(GPP_D19, DN_20K),
	/* GPP_D20:	WIFI_OFF_N_MCP		*/
	PAD_CFG_TERM_GPO(GPP_D20, 1, UP_20K, DEEP),
	/* GPP_D21:	Not Connected		*/
	PAD_NC(GPP_D21, DN_20K),
	/* GPP_D22:	Not Connected		*/
	PAD_NC(GPP_D22, DN_20K),
	/* GPP_D23:	Not Connected		*/
	PAD_NC(GPP_D23, DN_20K),
	/* GPP_E0:	Not Connected		*/
	PAD_NC(GPP_E0, DN_20K),
	/* GPP_E1:	Not Connected		*/
	PAD_NC(GPP_E1, DN_20K),
	/* GPP_E2:	SATA2_SSD_Type		*/
	PAD_NC(GPP_E2, DN_20K),
	/* GPP_E3:	Not Connected		*/
	PAD_NC(GPP_E3, DN_20K),
	/* GPP_E4:	Not Connected		*/
	PAD_NC(GPP_E4, DN_20K),
	/* GPP_E5:	Not Connected		*/
	PAD_NC(GPP_E5, DN_20K),
	/* GPP_E6:	SATA2_SSD_DEVSLP	*/
	PAD_CFG_NF(GPP_E6, NONE, PWROK, NF1),
	/* GPP_E7:	TOUCH_PANEL_INT_N	*/
	PAD_NC(GPP_E7, DN_20K),
	/* GPP_E8:	Not Connected		*/
	PAD_NC(GPP_E8, DN_20K),
	/* GPP_E9:	USB2_P1_WP1_OC_N	*/
	PAD_CFG_NF(GPP_E9, NONE, DEEP, NF1),
	/* GPP_E10:	USB2_P2_WP2_OC_N	*/
	PAD_CFG_NF(GPP_E10, NONE, DEEP, NF1),
	/* GPP_E11:	USB2_P3_WP2_OC_N	*/
	PAD_CFG_NF(GPP_E11, NONE, DEEP, NF1),
	/* GPP_E12:	Not Connected		*/
	PAD_NC(GPP_E12, DN_20K),
	/* GPP_E13:	DDI1_HPD		*/
	PAD_CFG_NF(GPP_E13, NONE, DEEP, NF1),
	/* GPP_E14:	DDI2_HPD		*/
	PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1),
	/* E15:  SMC_SMI_N                      */
	PAD_CFG_GPI_SMI_LOW(GPP_E15, NONE, DEEP, EDGE_SINGLE),
	/* E16:  SMC_SCI_N                      */
	PAD_CFG_GPI_SCI_LOW(GPP_E16, NONE, PLTRST, LEVEL),
	/* GPP_E17:	EDP_HPD			*/
	PAD_CFG_NF(GPP_E17, NONE, DEEP, NF1),
	/* GPP_E18:	DDI1_DDC_SCL		*/
	PAD_CFG_NF(GPP_E18, NONE, DEEP, NF1),
	/* GPP_E19:	DDI1_DDC_SDA		*/
	PAD_CFG_NF(GPP_E19, DN_20K, DEEP, NF1),
	/* GPP_E20:	Not Connected		*/
	PAD_NC(GPP_E20, DN_20K),
	/* GPP_E21:	DDI2_DDC_SDA		*/
	PAD_NC(GPP_E21, DN_20K),
	/* GPP_E22:	Not Connected		*/
	PAD_NC(GPP_E22, DN_20K),
	/* GPP_E23:	TOUCH_PANEL_RESET_N	*/
	PAD_NC(GPP_E23, DN_20K),
	/* GPP_F0:	Not Connected		*/
	PAD_NC(GPP_F0, DN_20K),
	/* GPP_F1:	Not Connected		*/
	PAD_NC(GPP_F1, DN_20K),
	/* GPP_F2:	Not Connected		*/
	PAD_NC(GPP_F2, DN_20K),
	/* GPP_F3:	Not Connected		*/
	PAD_NC(GPP_F3, DN_20K),
	/* GPP_F4:	GSENSOR_I2C_SDA		*/
	PAD_NC(GPP_F4, DN_20K),
	/* GPP_F5:	GSENSOR_I2C_SCL		*/
	PAD_NC(GPP_F5, DN_20K),
	/* GPP_F6:	Not Connected		*/
	PAD_NC(GPP_F6, DN_20K),
	/* GPP_F7:	Not Connected		*/
	PAD_NC(GPP_F7, DN_20K),
	/* GPP_F8:	Not Connected		*/
	PAD_NC(GPP_F8, NONE),
	/* GPP_F9:	Not Connected		*/
	PAD_NC(GPP_F9, NONE),
	/* GPP_F10:	ACCEL2_INT		*/
	PAD_NC(GPP_F10, DN_20K),
	/* GPP_F11:	ACCEL1_INT		*/
	PAD_NC(GPP_F11, DN_20K),
	/* GPP_F12:	Not Connected		*/
	PAD_NC(GPP_F12, DN_20K),
	/* GPP_F13:	Not Connected		*/
	PAD_NC(GPP_F13, DN_20K),
	/* GPP_F14:	Not Connected		*/
	PAD_NC(GPP_F14, DN_20K),
	/* GPP_F15:	Not Connected		*/
	PAD_NC(GPP_F15, DN_20K),
	/* GPP_F16:	Not Connected		*/
	PAD_NC(GPP_F16, DN_20K),
	/* GPP_F17:	Not Connected		*/
	PAD_NC(GPP_F17, DN_20K),
	/* GPP_F18:	Not Connected		*/
	PAD_NC(GPP_F18, DN_20K),
	/* GPP_F19:	Not Connected		*/
	PAD_NC(GPP_F19, DN_20K),
	/* GPP_F20:	Not Connected		*/
	PAD_NC(GPP_F20, DN_20K),
	/* GPP_F21:	Not Connected		*/
	PAD_NC(GPP_F21, DN_20K),
	/* GPP_F22:	Not Connected		*/
	PAD_NC(GPP_F22, DN_20K),
	/* GPP_F23:	Not Connected		*/
	PAD_NC(GPP_F23, DN_20K),
	/* GPP_G0:	Not Connected		*/
	PAD_NC(GPP_G0, NONE),
	/* GPP_G1:	Not Connected		*/
	PAD_NC(GPP_G1, NONE),
	/* GPP_G2:	Not Connected		*/
	PAD_NC(GPP_G2, NONE),
	/* GPP_G3:	Not Connected		*/
	PAD_NC(GPP_G3, NONE),
	/* GPP_G4:	Not Connected		*/
	PAD_NC(GPP_G4, NONE),
	/* GPP_G5:	Not Connected		*/
	PAD_NC(GPP_G5, NONE),
	/* GPP_G6:	Not Connected		*/
	PAD_NC(GPP_G6, NONE),
	/* GPP_G7:	Not Connected		*/
	PAD_NC(GPP_G7, NONE),
};

const struct pad_config *variant_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}
