/* SPDX-License-Identifier: GPL-2.0-only */

#include <variants.h>

/* Early pad configuration in bootblock */
const struct pad_config early_gpio_table[] = {
	/* Debug Connector */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),				/* RXD */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),				/* TXD */

	/* F12:		Board ID 0					*/
	PAD_CFG_GPI_LOCK(GPP_F12, NONE, LOCK_CONFIG),
	/* F13:		Board ID 1					*/
	PAD_CFG_GPI_LOCK(GPP_F13, NONE, LOCK_CONFIG),
	/* F14:		Board ID 2					*/
	PAD_CFG_GPI_LOCK(GPP_F14, NONE, LOCK_CONFIG),
	/* F15:		Board ID 3					*/
	PAD_CFG_GPI_LOCK(GPP_F15, NONE, LOCK_CONFIG),

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
	PAD_NC(GPD4, NONE),						/* Sleep S3 */
	PAD_NC(GPD5, NONE),						/* Sleep S4 */
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
	PAD_CFG_NF(GPP_H4, NONE, DEEP, NF1),				/* Data */
	PAD_CFG_NF(GPP_H5, NONE, DEEP, NF1),				/* Clock */
	PAD_CFG_GPI_APIC_LOW(GPP_D11, NONE, PLTRST),			/* Interrupt */

	/* SSD */
	PAD_CFG_NF(GPP_D6, NONE, DEEP, NF1),				/* Clock Request 5 */
	PAD_CFG_GPO(GPP_F20, 1, PLTRST),				/* Reset */
	PAD_CFG_GPO(GPP_D14, 1, DEEP),					/* Enable */

	/* SSD 2 */
	PAD_CFG_NF(GPP_H19, NONE, DEEP, NF1),				/* Clock Request 2 */
	PAD_CFG_GPO(GPP_H0, 1, PLTRST),					/* Reset */
	PAD_CFG_GPO(GPP_D16, 1, PLTRST),				/* Enable */

	/* Wireless */
	PAD_CFG_NF(GPP_D7, NONE, DEEP, NF1),				/* Clock Request 1 */
	PAD_CFG_GPO(GPP_H2, 1, PLTRST),					/* Reset */
	PAD_CFG_GPO(GPP_E3, 1, DEEP),					/* WiFi RF Kill */
	PAD_CFG_GPO(GPP_A13, 1, DEEP),					/* Bluetooth RF Kill */

	/* Display */
	PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1),				/* eDP Hot Plug */
	PAD_CFG_NF(GPP_A18, NONE, DEEP, NF1),				/* HDMI Hot Plug */
	PAD_CFG_NF(GPP_H15, NONE, DEEP, NF1),				/* HDMI Clock */
	PAD_CFG_NF(GPP_H17, NONE, DEEP, NF1),				/* HDMI Data */
	PAD_CFG_NF(GPP_B11, NONE, DEEP, NF1),				/* PMC Interrupt */
	PAD_CFG_NF(GPP_C6, NONE, DEEP, NF1),				/* SML Clock */
	PAD_CFG_NF(GPP_C7, NONE, DEEP, NF1),				/* SML Data */

	/* Retimer */
	PAD_CFG_NF(GPP_E18, NONE, DEEP, NF2),				/* TBT0 TXD */
	PAD_CFG_NF(GPP_E19, NONE, DEEP, NF2),				/* TBT0 RXD */
	PAD_CFG_NF(GPP_E20, NONE, DEEP, NF4),				/* TBT1 TXD */
	PAD_CFG_NF(GPP_E21, NONE, DEEP, NF4),				/* TBT1 RXD */
	PAD_CFG_GPO(GPP_E4, 0, DEEP),					/* Force Power */

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

	/* TPM */
	PAD_CFG_GPI_APIC_LOW(GPP_D10, NONE, PLTRST),			/* Interrupt */

	/* SMBus */
	PAD_CFG_NF(GPP_C0, NONE, DEEP, NF1),				/* Clock */
	PAD_CFG_NF(GPP_C1, NONE, DEEP, NF1),				/* Data */
	PAD_CFG_GPO(GPP_E8, 1, DEEP),					/* DRAM Sleep */

	/* Config Straps 									[ Low      / High     ] */
	PAD_CFG_GPO(GPP_B14, 0, RSMRST),				/* Top Swap		[ Disabled / Enabled  ] */
	PAD_CFG_GPO(GPP_B18, 0, RSMRST),				/* Reboot Support	[ Enabled  / Disabled ] */
	PAD_CFG_GPO(GPP_C2, 1, RSMRST),					/* TLS Confidentiality	[ Disabled / Enabled  ] */
	PAD_CFG_GPO(GPP_C5, 0, RSMRST),					/* eSPI			[ Enabled  / Disabled ] */
	PAD_CFG_GPO(GPP_E6, 0, RSMRST),					/* JTAG ODT		[ Disabled / Enabled  ] */
	PAD_CFG_GPO(GPP_H1, 0, RSMRST),					/* BFX Strap 2 Bit 3	[ Disabled / Enabled  ] */
	PAD_CFG_GPO(GPP_F2, 1, RSMRST),					/* M.2 CNVi		[ Enabled  / Disabled ] */
	PAD_CFG_GPO(GPP_F7, 0, RSMRST),					/* MCRO LDO		[ Disabled / Bypass   ] */
	PAD_CFG_GPO(GPD7, 0, RSMRST),					/* RTC Clock Delay	[ Disabled / 95ms     ] */

	PAD_NC(GPD2, NONE),
	PAD_NC(GPD6, NONE),
	PAD_NC(GPD9, NONE),
	PAD_NC(GPD10, NONE),
	PAD_NC(GPD11, NONE),

	PAD_NC(GPP_A6, NONE),
	PAD_NC(GPP_A7, NONE),
	PAD_NC(GPP_A8, NONE),
	PAD_NC(GPP_A11, NONE),
	PAD_NC(GPP_A12, NONE),
	PAD_NC(GPP_A14, NONE),
	PAD_NC(GPP_A15, NONE),
	PAD_NC(GPP_A16, NONE),
	PAD_NC(GPP_A17, NONE),
	PAD_NC(GPP_A19, NONE),
	PAD_NC(GPP_A20, NONE),
	PAD_NC(GPP_A21, NONE),
	PAD_NC(GPP_A22, NONE),
	PAD_NC(GPP_A23, NONE),

	PAD_NC(GPP_B3, NONE),
	PAD_NC(GPP_B4, NONE),
	PAD_NC(GPP_B5, NONE),
	PAD_NC(GPP_B6, NONE),
	PAD_NC(GPP_B7, NONE),
	PAD_NC(GPP_B8, NONE),
	PAD_NC(GPP_B9, NONE),
	PAD_NC(GPP_B10, NONE),
	PAD_NC(GPP_B12, NONE),
	PAD_NC(GPP_B15, NONE),
	PAD_NC(GPP_B16, NONE),
	PAD_NC(GPP_B17, NONE),
	PAD_NC(GPP_B19, NONE),
	PAD_NC(GPP_B20, NONE),
	PAD_NC(GPP_B21, NONE),
	PAD_NC(GPP_B22, NONE),
	PAD_NC(GPP_B23, NONE),
	PAD_NC(GPP_B24, NONE),
	PAD_NC(GPP_B25, NONE),

	PAD_NC(GPP_C3, NONE),
	PAD_NC(GPP_C4, NONE),
	PAD_NC(GPP_C8, NONE),
	PAD_NC(GPP_C9, NONE),
	PAD_NC(GPP_C10, NONE),
	PAD_NC(GPP_C11, NONE),
	PAD_NC(GPP_C12, NONE),
	PAD_NC(GPP_C13, NONE),
	PAD_NC(GPP_C14, NONE),
	PAD_NC(GPP_C15, NONE),
	PAD_NC(GPP_C16, NONE),
	PAD_NC(GPP_C17, NONE),
	PAD_NC(GPP_C18, NONE),
	PAD_NC(GPP_C19, NONE),
	PAD_NC(GPP_C20, NONE),
	PAD_NC(GPP_C21, NONE),
	PAD_NC(GPP_C22, NONE),
	PAD_NC(GPP_C23, NONE),

	PAD_NC(GPP_D0, NONE),
	PAD_NC(GPP_D1, NONE),
	PAD_NC(GPP_D2, NONE),
	PAD_NC(GPP_D3, NONE),
	PAD_NC(GPP_D4, NONE),
	PAD_NC(GPP_D5, NONE),
	PAD_NC(GPP_D8, NONE),
	PAD_NC(GPP_D9, NONE),
	PAD_NC(GPP_D12, NONE),
	PAD_NC(GPP_D13, NONE),
	PAD_NC(GPP_D15, NONE),
	PAD_NC(GPP_D17, NONE),
	PAD_NC(GPP_D18, NONE),
	PAD_NC(GPP_D19, NONE),

	PAD_NC(GPP_E0, NONE),
	PAD_NC(GPP_E1, NONE),
	PAD_NC(GPP_E2, NONE),
	PAD_NC(GPP_E5, NONE),
	PAD_NC(GPP_E7, NONE),
	PAD_NC(GPP_E9, NONE),
	PAD_NC(GPP_E10, NONE),
	PAD_NC(GPP_E11, NONE),
	PAD_NC(GPP_E12, NONE),
	PAD_NC(GPP_E13, NONE),
	PAD_NC(GPP_E15, NONE),
	PAD_NC(GPP_E16, NONE),
	PAD_NC(GPP_E17, NONE),
	PAD_NC(GPP_E22, NONE),
	PAD_NC(GPP_E23, NONE),

	PAD_NC(GPP_F0, NONE),
	PAD_NC(GPP_F1, NONE),
	PAD_NC(GPP_F2, NONE),
	PAD_NC(GPP_F3, NONE),
	PAD_NC(GPP_F4, NONE),
	PAD_NC(GPP_F5, NONE),
	PAD_NC(GPP_F6, NONE),
	PAD_NC(GPP_F8, NONE),
	PAD_NC(GPP_F9, NONE),
	PAD_NC(GPP_F10, NONE),
	PAD_NC(GPP_F11, NONE),
	PAD_NC(GPP_F16, NONE),
	PAD_NC(GPP_F17, NONE),
	PAD_NC(GPP_F18, NONE),
	PAD_NC(GPP_F19, NONE),
	PAD_NC(GPP_F21, NONE),
	PAD_NC(GPP_F22, NONE),
	PAD_NC(GPP_F23, NONE),

	PAD_NC(GPP_H3, NONE),
	PAD_NC(GPP_H6, NONE),
	PAD_NC(GPP_H7, NONE),
	PAD_NC(GPP_H8, NONE),
	PAD_NC(GPP_H9, NONE),
	PAD_NC(GPP_H12, NONE),
	PAD_NC(GPP_H13, NONE),
	PAD_NC(GPP_H14, NONE),
	PAD_NC(GPP_H16, NONE),
	PAD_NC(GPP_H20, NONE),
	PAD_NC(GPP_H21, NONE),
	PAD_NC(GPP_H22, NONE),
	PAD_NC(GPP_H23, NONE),

	PAD_NC(GPP_S0, NONE),
	PAD_NC(GPP_S1, NONE),
	PAD_NC(GPP_S2, NONE),
	PAD_NC(GPP_S3, NONE),
	PAD_NC(GPP_S4, NONE),
	PAD_NC(GPP_S5, NONE),
	PAD_NC(GPP_S6, NONE),
	PAD_NC(GPP_S7, NONE),

	PAD_NC(GPP_T0, NONE),
	PAD_NC(GPP_T1, NONE),
	PAD_NC(GPP_T2, NONE),
	PAD_NC(GPP_T3, NONE),
	PAD_NC(GPP_T4, NONE),
	PAD_NC(GPP_T5, NONE),
	PAD_NC(GPP_T6, NONE),
	PAD_NC(GPP_T7, NONE),
	PAD_NC(GPP_T8, NONE),
	PAD_NC(GPP_T9, NONE),
	PAD_NC(GPP_T10, NONE),
	PAD_NC(GPP_T11, NONE),
	PAD_NC(GPP_T12, NONE),
	PAD_NC(GPP_T13, NONE),
	PAD_NC(GPP_T14, NONE),
	PAD_NC(GPP_T15, NONE),

	PAD_NC(GPP_R5, NONE),
	PAD_NC(GPP_R6, NONE),
	PAD_NC(GPP_R7, NONE),
};

const struct pad_config *variant_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}
