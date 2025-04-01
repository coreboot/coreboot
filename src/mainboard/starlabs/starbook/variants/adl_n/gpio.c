/* SPDX-License-Identifier: GPL-2.0-only */

#include <variants.h>

/* Early pad configuration in bootblock */
const struct pad_config early_gpio_table[] = {
	/* Debug Connector */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),				/* RXD */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),				/* TXD */
};

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

/* Pad configuration in ramstage. */
const struct pad_config gpio_table[] = {
	/* General Purpose I/O Deep */
	PAD_CFG_NF(GPD0, NONE, DEEP, NF1),				/* Battery Low */
	PAD_CFG_NF(GPD1, NONE, DEEP, NF1),				/* Charger Connected */
	PAD_CFG_NF(GPD3, UP_20K, DEEP, NF1),				/* Power Button */
	PAD_CFG_NF(GPD4, NONE, DEEP, NF1),				/* Sleep S3 */
	PAD_CFG_NF(GPD5, NONE, DEEP, NF1),				/* Sleep S4 */
	PAD_CFG_NF(GPD8, NONE, DEEP, NF1),				/* Bluetooth Suspend */

	/* eSPI */
	// PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A0, UP_20K, DEEP, NF1),	/* eSPI IO 0 */
	// PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A1, UP_20K, DEEP, NF1),	/* eSPI IO 1 */
	// PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A2, UP_20K, DEEP, NF1),	/* eSPI IO 2 */
	// PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A3, UP_20K, DEEP, NF1),	/* eSPI IO 3 */
	// PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A4, UP_20K, DEEP, NF1),	/* eSPI CS 0 */
	// PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A9, UP_20K, DEEP, NF1),	/* eSPI Clk */
	// PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A10, NONE, DEEP, NF1),	/* eSPI Reset */

	/* Touchpad */
	PAD_CFG_NF(GPP_B7, NONE, DEEP, NF2),				/* Data */
	PAD_CFG_NF(GPP_B8, NONE, DEEP, NF2),				/* Clock */
	PAD_CFG_GPI_APIC_LOW(GPP_E12, NONE, PLTRST),			/* Interrupt */

	/* SSD */
	PAD_CFG_NF(GPP_D5, NONE, DEEP, NF1),				/* Clock Request 0 */
	PAD_CFG_GPO(GPP_H0, 1, PLTRST),					/* Reset */
	PAD_CFG_GPO(GPP_D16, 1, DEEP),					/* Enable */

	/* Wireless */
	PAD_CFG_NF(GPP_D6, NONE, DEEP, NF1),				/* Clock Request 1 */
	PAD_CFG_GPO(GPP_H2, 1, PLTRST),					/* Reset */
	PAD_CFG_GPO(GPP_E3, 1, DEEP),					/* WiFi RF Kill */
	PAD_CFG_GPO(GPP_A13, 1, DEEP),					/* Bluetooth RF Kill */

	/* Display */
	PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1),				/* eDP Hot Plug */
	PAD_CFG_NF(GPP_A18, NONE, DEEP, NF1),				/* HDMI Hot Plug */
	PAD_CFG_NF(GPP_H15, NONE, DEEP, NF1),				/* HDMI Clock */
	PAD_CFG_NF(GPP_H17, NONE, DEEP, NF1),				/* HDMI Data */
	PAD_CFG_NF(GPP_A19, NONE, DEEP, NF1),				/* TCP0 Hot Plug */

	/* High-Definition Audio */
	PAD_CFG_NF(GPP_R0, NATIVE, DEEP, NF1),				/* Clock */
	PAD_CFG_NF(GPP_R1, NATIVE, DEEP, NF1),				/* Sync */
	PAD_CFG_NF(GPP_R2, NATIVE, DEEP, NF1),				/* Data Output */
	PAD_CFG_NF(GPP_R3, NATIVE, DEEP, NF1),				/* Data Input */
	PAD_CFG_NF(GPP_R4, NATIVE, DEEP, NF1),				/* Reset */

	/* PCH */
	PAD_CFG_NF(GPP_H18, NONE, DEEP, NF1),				/* C10 Gate */
	PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1),				/* Platform Reset */
	PAD_CFG_NF(GPP_B0, NONE, DEEP, NF1),				/* Vendor ID 0 */
	PAD_CFG_NF(GPP_B1, NONE, DEEP, NF1),				/* Vendor ID 1 */
	PAD_CFG_GPI_SCI(GPP_B2, NONE, PLTRST, EDGE_SINGLE, INVERT),	/* Processor Hot */

	PAD_NC(GPD2, NONE),
	PAD_NC(GPD6, NONE),
	/* GPD7:	Power Adapter Disable				*/
	PAD_CFG_GPO(GPD7, 0, PWROK),
	PAD_NC(GPD9, NONE),
	PAD_NC(GPD10, NONE),
	PAD_NC(GPD11, NONE),

	/* A5:		Not Connected					*/
	PAD_NC(GPP_A5, NONE),
	/* A6:		Not Connected					*/
	PAD_NC(GPP_A6, NONE),
	/* A7:		Embedded Controller SCI				*/
	PAD_NC(GPP_A7, NONE),
	/* A8:		Not Connected					*/
	PAD_NC(GPP_A8, NONE),
	/* A11:		Not Connected					*/
	PAD_NC(GPP_A11, NONE),
	/* A12:		Not Connected					*/
	PAD_NC(GPP_A12, NONE),
	/* A14:		Test Point 45					*/
	PAD_NC(GPP_A14, NONE),
	/* A15:		Test Point 52					*/
	PAD_NC(GPP_A15, NONE),
	/* A16:		USB OverCurrent 3				*/
	PAD_NC(GPP_A16, NONE),
	/* A17:		Not Connected					*/
	PAD_NC(GPP_A17, NONE),
	/* A20:		Test Point 44					*/
	PAD_NC(GPP_A20, NONE),
	/* A21:		Fingerprint Reader Interrupt			*/
	PAD_NC(GPP_A21, NONE),
	/* A22:		Fingerprint Reader Reset			*/
	PAD_NC(GPP_A22, NONE),
	/* A23:		Not Connected					*/
	PAD_NC(GPP_A23, NONE),


	/* B3:		Not Connected					*/
	PAD_NC(GPP_B3, NONE),
	/* B4:		Not Connected					*/
	PAD_NC(GPP_B4, NONE),
	/* B5:		I2C 2 SDA		Touch Panel SDA		*/
	PAD_NC(GPP_B5, NONE),
	/* B6:		I2C 2 SCL		Touch Panel Clock	*/
	PAD_NC(GPP_B6, NONE),
	/* B9:		Not Connected					*/
	PAD_NC(GPP_B9, NONE),
	/* B10:		Not Connected					*/
	PAD_NC(GPP_B10, NONE),
	/* B11:		Not Connected					*/
	PAD_NC(GPP_B11, NONE),
	/* B12:		PM SLP S0					*/
	PAD_NC(GPP_B12, NONE),
	/* B14:		Top Swap Override	Weak Internal PD 20K
				High:	Enabled
				Low:	Disabled			*/
	PAD_CFG_GPO(GPP_B14, 0, PLTRST),
	/* B15:		Not Connected					*/
	PAD_NC(GPP_B15, NONE),
	/* B16:		Not Connected					*/
	PAD_NC(GPP_B16, NONE),
	/* B17:		Not Connected					*/
	PAD_NC(GPP_B17, NONE),
	/* B18:		Reboot Support		Weak Internal PD 20K
				High:	Disabled
				Low:	Enabled				*/
	PAD_NC(GPP_B18, NONE),
	/* B19:		Not Connected					*/
	PAD_NC(GPP_B19, NONE),
	/* B20:		Not Connected					*/
	PAD_NC(GPP_B20, NONE),
	/* B21:		Not Connected					*/
	PAD_NC(GPP_B21, NONE),
	/* B22:		Not Connected					*/
	PAD_NC(GPP_B22, NONE),
	/* B23:		Not used	MiPi Camera			*/
	PAD_NC(GPP_B23, NONE),
	/* B24:		Not Connected					*/
	PAD_NC(GPP_B24, NONE),
	/* B25:		Not Connected					*/
	PAD_NC(GPP_B25, NONE),

	/* C0:		SMB Clock					*/
	PAD_CFG_NF(GPP_C0, NONE, DEEP, NF1),
	/* C1:		SMB Data					*/
	PAD_CFG_NF(GPP_C1, NONE, DEEP, NF1),
	/* C2:		TLS Confidentiality	Weak Internal PD 20K
				Low:	Disabled
				High:	Enabled				*/
	PAD_CFG_GPO(GPP_C2, 1, PLTRST),
	/* C3:		SML 0 Clock					*/
	PAD_NC(GPP_C3, NONE),
	/* C4:		SML 0 Data					*/
	PAD_NC(GPP_C4, NONE),
	/* C5:		Boot Strap		Weak Internal PD 20K
				Low:	ESPI
				High:	Disabled			*/
	PAD_CFG_GPO(GPP_C5, 0, PLTRST),
	/* C6:		SML 1 Clock					*/
	PAD_NC(GPP_C6, NONE),
	/* C7:		SML 1 Data					*/
	PAD_NC(GPP_C7, NONE),
	/* C8:		Not Connected					*/
	PAD_NC(GPP_C8, NONE),
	/* C9:		Not Connected					*/
	PAD_NC(GPP_C9, NONE),
	/* C10:		Not Connected					*/
	PAD_NC(GPP_C10, NONE),
	/* C11:		Not Connected					*/
	PAD_NC(GPP_C11, NONE),
	/* C12:		Not Connected					*/
	PAD_NC(GPP_C12, NONE),
	/* C13:		Not Connected					*/
	PAD_NC(GPP_C13, NONE),
	/* C14:		Not Connected					*/
	PAD_NC(GPP_C14, NONE),
	/* C15:		Not Connected					*/
	PAD_NC(GPP_C15, NONE),
	/* C16:		Not Connected					*/
	PAD_NC(GPP_C16, NONE),
	/* C17:		Not Connected					*/
	PAD_NC(GPP_C17, NONE),
	/* C18:		Not Connected					*/
	PAD_NC(GPP_C18, NONE),
	/* C19:		Not Connected					*/
	PAD_NC(GPP_C19, NONE),
	/* C20:		Not Connected					*/
	PAD_NC(GPP_C20, NONE),
	/* C21:		Not Connected					*/
	PAD_NC(GPP_C21, NONE),
	/* C22:		Not Connected					*/
	PAD_NC(GPP_C22, NONE),
	/* C23:		Not Connected					*/
	PAD_NC(GPP_C23, NONE),

	/* D0:		Not used		Audio ID 0		*/
	PAD_NC(GPP_D0, NONE),
	/* D1:		Not used		Audio ID 1		*/
	PAD_NC(GPP_D1, NONE),
	/* D2:		Not used		Audio ID 2		*/
	PAD_NC(GPP_D2, NONE),
	/* D3:		Not Connected					*/
	PAD_NC(GPP_D3, NONE),
	/* D4:		Not Connected					*/
	PAD_NC(GPP_D4, NONE),
	/* D7:		Clock Request 2					*/
	PAD_NC(GPP_D7, NONE),
	/* D8:		Clock Request 3					*/
	PAD_NC(GPP_D8, NONE),
	/* D9:		GSPI 2 FPS					*/
	PAD_NC(GPP_D9, NONE),
	/* D10:		GSPI 2 Clock					*/
	PAD_NC(GPP_D10, NONE),
	/* D11:		GSPI 2 MISO FPS					*/
	PAD_NC(GPP_D11, NONE),
	/* D12:		GSPI 2 MOSI FPS					*/
	PAD_NC(GPP_D12, NONE),
	/* D13:		Wireless LAN Wake				*/
	PAD_NC(GPP_D13, NONE),
	/* D14:		CPU M.2 SSD Power Enable			*/
	PAD_NC(GPP_D14, NONE),
	/* D15:		Not Connected					*/
	PAD_NC(GPP_D15, NONE),
	/* D17:		Not used		Fingerprint ID		*/
	PAD_NC(GPP_D17, NONE),
	/* D18:		Not Connected					*/
	PAD_NC(GPP_D18, NONE),
	/* D19:		Test Point 21					*/
	PAD_NC(GPP_D19, NONE),

	/* E0:		SATA x PCIe					*/
	PAD_NC(GPP_E0, NONE),
	/* E1:		Not used		Accelerometer Interrupt	*/
	PAD_NC(GPP_E1, NONE),
	/* E2:		Not Connected					*/
	PAD_NC(GPP_E2, NONE),
	/* E4:		Test Point 14					*/
	PAD_NC(GPP_E4, NONE),
	/* E5:		Not Connected					*/
	PAD_NC(GPP_E5, NONE),
	/* E6:		JTAG ODT		No internal PD
				Low:	Disabled
				High:	Enabled				*/
	PAD_CFG_GPO(GPP_E6, 0, DEEP),
	/* E7:		Embedded Controller SMI				*/
	PAD_NC(GPP_E7, NONE),
	/* E8:		DRAM Sleep					*/
	PAD_CFG_GPO(GPP_E8, 1, DEEP),
	/* E9:		USB OverCurrent 0				*/
	PAD_NC(GPP_E9, NONE),
	/* E10:		PWD Amplifier Input				*/
	PAD_NC(GPP_E10, NONE),
	/* E11:		TPM IRQ						*/
	PAD_CFG_GPI_APIC_LOW(GPP_E11, NONE, PLTRST),
	/* E13:		Not connected					*/
	PAD_NC(GPP_E13, NONE),
	/* E15:		Not Connected					*/
	PAD_NC(GPP_E15, NONE),
	/* E16:		Not Connected					*/
	PAD_NC(GPP_E16, NONE),
	/* E17:		Not Connected					*/
	PAD_NC(GPP_E17, NONE),
	/* E18:		Not Connected					*/
	PAD_NC(GPP_E18, NONE),
	/* E19:		Thunderbolt LSX RXD				*/
	PAD_NC(GPP_E19, NONE),
	/* E20:		Not Connected					*/
	PAD_NC(GPP_E20, NONE),
	/* E21:		Not Connected					*/
	PAD_NC(GPP_E21, NONE),
	/* E22:		Not Connected					*/
	PAD_NC(GPP_E22, NONE),
	/* E23:		Not Connected					*/
	PAD_NC(GPP_E23, NONE),

	/* F0:		CNV BRI Data					*/
	PAD_NC(GPP_F0, NONE),
	/* F1:		CNV BRI Response				*/
	PAD_NC(GPP_F1, NONE),
	/* F2:		CNV RGI Data					*/
	PAD_NC(GPP_F2, NONE),
	/* F3:		CNV RGI Response				*/
	PAD_NC(GPP_F3, NONE),
	/* F4:		CNV RF Reset					*/
	PAD_NC(GPP_F4, NONE),
	/* F5:		Not used		MODEM_CLKREQ		*/
	PAD_NC(GPP_F5, NONE),
	/* F6:		CNV PA Blanking					*/
	PAD_NC(GPP_F6, NONE),
	/* F7:		TBT LSX VCCIO		Weak Internal PD 20K
				Low:	1.8V
				High:	3.3V				*/
	PAD_CFG_GPO(GPP_F7, 0, DEEP),
	/* F8:		Not Connected					*/
	PAD_NC(GPP_F8, NONE),
	/* F9:								*/
	PAD_NC(GPP_F9, NONE),
	/* F10:		Weak Internal PD 20K				*/
	PAD_CFG_GPO(GPP_F10, 0, DEEP),
	/* F11:		TPM ID						*/
	PAD_NC(GPP_F11, NONE),
	/* F12:		Not Connected					*/
	PAD_NC(GPP_F12, NONE),
	/* F13:		Not Connected					*/
	PAD_NC(GPP_F13, NONE),
	/* F14:		Not Connected					*/
	PAD_NC(GPP_F14, NONE),
	/* F15:		Not used					*/
	PAD_NC(GPP_F15, NONE),
	/* F16:		Not Connected					*/
	PAD_NC(GPP_F16, NONE),
	/* F17:		Not used					*/
	PAD_NC(GPP_F17, NONE),
	/* F18:		Not used					*/
	PAD_NC(GPP_F18, NONE),
	/* F19:		Not Connected					*/
	PAD_NC(GPP_F19, NONE),
	/* F20:		Not Connected					*/
	PAD_NC(GPP_F20, NONE),
	/* F21:		Not Connected					*/
	PAD_NC(GPP_F21, NONE),
	/* F22:		Not Connected					*/
	PAD_NC(GPP_F22, NONE),
	/* F23:		Not Connected					*/
	PAD_NC(GPP_F23, NONE),

	/* H1:		BFX Strap 2 Bit 3	Weak Internal PD 20K	*/
	PAD_CFG_GPO(GPP_H1, 0, DEEP),
	/* H3:		Not Connected					*/
	PAD_NC(GPP_H3, NONE),
	/* H4:		I2C 0 SDA		Touchpad		*/
	PAD_NC(GPP_H4, NONE),
	/* H5:		I2C 0 SCL		Touchpad		*/
	PAD_NC(GPP_H5, NONE),
	/* H6:		Not Connected					*/
	PAD_NC(GPP_H6, NONE),
	/* H7:		Not Connected					*/
	PAD_NC(GPP_H7, NONE),
	/* H8:		I2C 4 SDA		GSensor			*/
	PAD_NC(GPP_H8, NONE),
	/* H9:		I2C 4 SDL		GSensor			*/
	PAD_NC(GPP_H9, NONE),
	/* H12:		Not Connected					*/
	PAD_NC(GPP_H12, NONE),
	/* H13:		PCH M.2 SSD Device Sleep			*/
	PAD_NC(GPP_H13, NONE),
	/* H14:		Not Connected					*/
	PAD_NC(GPP_H14, NONE),
	/* H16:		Not Connected					*/
	PAD_NC(GPP_H16, NONE),
	/* H19:		Clock Request 4		CPU M.2 SSD		*/
	PAD_NC(GPP_H19, NONE),
	/* H20:		Not Connected					*/
	PAD_NC(GPP_H20, NONE),
	/* H21:		Not Connected					*/
	PAD_NC(GPP_H21, NONE),
	/* H22:		Not Connected					*/
	PAD_NC(GPP_H22, NONE),
	/* H23:		Clock Request 5					*/
	PAD_NC(GPP_H23, NONE),

	/* S0:		Not Connected					*/
	PAD_NC(GPP_S0, NONE),
	/* S1:		Not Connected					*/
	PAD_NC(GPP_S1, NONE),
	/* S2:		DMIC Clock					*/
	PAD_NC(GPP_S2, NONE),
	/* S3:		DMIC Data					*/
	PAD_NC(GPP_S3, NONE),
	/* S4:		Not Connected					*/
	PAD_NC(GPP_S4, NONE),
	/* S5:		Not Connected					*/
	PAD_NC(GPP_S5, NONE),
	/* S6:		Not Connected					*/
	PAD_NC(GPP_S6, NONE),
	/* S7:		Not Connected					*/
	PAD_NC(GPP_S7, NONE),

	/* T0:		 Not Connected					*/
	PAD_NC(GPP_T0, NONE),
	/* T1:		 Not Connected					*/
	PAD_NC(GPP_T1, NONE),
	/* T2:		 Not Connected					*/
	PAD_NC(GPP_T2, NONE),
	/* T3:		 Not Connected					*/
	PAD_NC(GPP_T3, NONE),
	/* T4:		 Not Connected					*/
	PAD_NC(GPP_T4, NONE),
	/* T5:		 Not Connected					*/
	PAD_NC(GPP_T5, NONE),
	/* T6:		 Not Connected					*/
	PAD_NC(GPP_T6, NONE),
	/* T7:		 Not Connected					*/
	PAD_NC(GPP_T7, NONE),
	/* T8:		 Not Connected					*/
	PAD_NC(GPP_T8, NONE),
	/* T9:		 Not Connected					*/
	PAD_NC(GPP_T9, NONE),
	/* T10:		 Not Connected					*/
	PAD_NC(GPP_T10, NONE),
	/* T11:		 Not Connected					*/
	PAD_NC(GPP_T11, NONE),
	/* T12:		 Not Connected					*/
	PAD_NC(GPP_T12, NONE),
	/* T13:		 Not Connected					*/
	PAD_NC(GPP_T13, NONE),
	/* T14:		 Not Connected					*/
	PAD_NC(GPP_T14, NONE),
	/* T15:		 Not Connected					*/
	PAD_NC(GPP_T15, NONE),

	PAD_NC(GPP_R5, NONE),
	/* R6:		Not Connected					*/
	PAD_NC(GPP_R6, NONE),
	/* R7:		Not Connected					*/
	PAD_NC(GPP_R7, NONE),
};

const struct pad_config *variant_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}
