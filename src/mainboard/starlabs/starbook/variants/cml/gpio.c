/* SPDX-License-Identifier: GPL-2.0-only */

#include <variants.h>

/* Early pad configuration in bootblock */
const struct pad_config early_gpio_table[] = {
	/* Debug Connector */
	PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),				/* RXD */
	PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),				/* TXD */
};

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

/* Pad configuration in ramstage. */
/* clang-format off */
const struct pad_config gpio_table[] = {
	/* General Purpose I/O Deep */
	PAD_CFG_NF(GPD0, NONE, DEEP, NF1),				/* Battery Low */
	PAD_CFG_NF(GPD1, NONE, DEEP, NF1),				/* Charger Connected */
	PAD_CFG_NF(GPD3, UP_20K, DEEP, NF1),				/* Power Button */
	PAD_CFG_NF(GPD4, NONE, DEEP, NF1),				/* Sleep S3 */
	PAD_CFG_NF(GPD5, NONE, DEEP, NF1),				/* Sleep S4 */
	PAD_CFG_NF(GPD8, NONE, DEEP, NF1),				/* Wireless Suspend Clock */
	PAD_CFG_NF(GPD10, NONE, DEEP, NF1),				/* Sleep S5 */

	/* LPC/eSPI */
	PAD_CFG_NF(GPP_A0, NONE, DEEP, NF1),				/* Reset */
	PAD_CFG_NF(GPP_A1, NATIVE, DEEP, NF1),				/* Data 0 */
	PAD_CFG_NF(GPP_A2, NATIVE, DEEP, NF1),				/* Data 1 */
	PAD_CFG_NF(GPP_A3, NATIVE, DEEP, NF1),				/* Data 2 */
	PAD_CFG_NF(GPP_A4, NATIVE, DEEP, NF1),				/* Data 3 */
	PAD_CFG_NF(GPP_A5, NONE, DEEP, NF1),				/* Frame */
	PAD_CFG_NF(GPP_A6, NONE, DEEP, NF1),				/* Serial IRQ */
	PAD_CFG_NF(GPP_A8, NONE, DEEP, NF1),				/* Clock Run */
	PAD_CFG_NF(GPP_A9, DN_20K, DEEP, NF1),				/* Clock */

	/* Touchpad */
	PAD_CFG_NF(GPP_C16, NONE, DEEP, NF1),				/* Data */
	PAD_CFG_NF(GPP_C17, NONE, DEEP, NF1),				/* Clock */
	PAD_CFG_GPI_APIC_LOW(GPP_B3, NONE, PLTRST),			/* Interrupt */

	/* SSD */
	PAD_CFG_NF(GPP_B6, NONE, DEEP, NF1),				/* Clock Request 1 */

	/* Wireless */
	PAD_CFG_NF(GPP_F4, UP_20K, DEEP, NF1),				/* BRI Data */
	PAD_CFG_NF(GPP_F5, UP_20K, DEEP, NF1),				/* BRI Response */
	PAD_CFG_NF(GPP_F6, UP_20K, DEEP, NF1),				/* RGI Data */
	PAD_CFG_NF(GPP_F7, UP_20K, DEEP, NF1),				/* RGI Response */
	PAD_CFG_NF(GPP_H1, UP_20K, DEEP, NF3),				/* RF Reset */
	PAD_CFG_NF(GPP_H2, UP_20K, DEEP, NF3),				/* Modem Clock Request */
	PAD_CFG_GPO(GPP_C2, 1, DEEP),					/* WiFi RF Kill */
	PAD_CFG_GPO(GPP_B4, 1, DEEP),					/* Bluetooth RF Kill */

	/* Display */
	PAD_CFG_NF(GPP_E17, NONE, DEEP, NF1),				/* eDP Hot Plug */
	PAD_CFG_NF(GPP_E13, NONE, DEEP, NF1),				/* HDMI Hot Plug */
	PAD_CFG_NF(GPP_E18, NONE, DEEP, NF1),				/* HDMI DDC Clock */
	PAD_CFG_NF(GPP_E19, NONE, DEEP, NF1),				/* HDMI DDC Data */
	PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1),				/* USB-C DP Hot Plug */
	PAD_CFG_NF(GPP_E20, NONE, DEEP, NF1),				/* USB-C DP Clock */
	PAD_CFG_NF(GPP_E21, NONE, DEEP, NF1),				/* USB-C DP Data */

	/* PCH */
	PAD_CFG_NF(GPP_H18, NONE, DEEP, NF1),				/* C10 Gate */
	PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1),				/* Platform Reset */
	PAD_CFG_NF(GPP_B12, NONE, DEEP, NF1),				/* Sleep S0 */
	PAD_CFG_GPO(GPP_A13, 1, PLTRST),				/* SUSPWRDNACK */
	PAD_CFG_NF(GPP_A14, NONE, DEEP, NF1),				/* Suspend Status */

	/* EC */
	PAD_CFG_GPI_SMI_LOW(GPP_E15, NONE, DEEP, EDGE_SINGLE),		/* SMI */
	PAD_CFG_GPI_SCI_LOW(GPP_E16, NONE, PLTRST, LEVEL),		/* SCI */
	PAD_CFG_GPO(GPP_D16, 0, PLTRST),				/* GPIO 2 */

	/* Storage */
	PAD_CFG_NF(GPP_E1, UP_20K, DEEP, NF1),				/* PCIe/SATA Detect */
	PAD_CFG_NF(GPP_E5, NONE, DEEP, NF1),				/* SATA Device Sleep */

	/* Config Straps */
	PAD_CFG_GPO(GPD7, 0, PLTRST),					/* Crystal Input Mode */
	PAD_CFG_GPO(GPP_B18, 0, DEEP),					/* Reboot Support */
	PAD_CFG_GPO(GPP_C5, 0, DEEP),					/* LPC/eSPI Select */
	PAD_CFG_GPO(GPP_D12, 0, DEEP),					/* JTAG ODT */
	PAD_CFG_NF(GPP_F23, DN_20K, DEEP, NF1),				/* A4WP Present */
	PAD_CFG_GPO(GPP_H21, 0, DEEP),					/* Crystal Frequency */
	PAD_CFG_GPO(GPP_H23, 0, DEEP),					/* MAF/SAF */

	/* Unused pads */
	PAD_NC(GPD2, NONE),
	PAD_NC(GPD6, NONE),
	PAD_NC(GPD9, NONE),
	PAD_NC(GPD11, NONE),

	PAD_NC(GPP_A7, NONE),
	PAD_NC(GPP_A10, NONE),
	PAD_NC(GPP_A11, NONE),
	PAD_NC(GPP_A12, NONE),
	PAD_NC(GPP_A15, NONE),
	PAD_NC(GPP_A16, NONE),
	PAD_NC(GPP_A17, NONE),
	PAD_NC(GPP_A18, NONE),
	PAD_NC(GPP_A19, NONE),
	PAD_NC(GPP_A20, NONE),
	PAD_NC(GPP_A21, NONE),
	PAD_NC(GPP_A22, NONE),
	PAD_NC(GPP_A23, NONE),

	PAD_NC(GPP_B0, NONE),
	PAD_NC(GPP_B1, NONE),
	PAD_NC(GPP_B2, NONE),
	PAD_NC(GPP_B5, NONE),
	PAD_NC(GPP_B7, NONE),
	PAD_NC(GPP_B8, NONE),
	PAD_NC(GPP_B9, NONE),
	PAD_NC(GPP_B10, NONE),
	PAD_NC(GPP_B11, NONE),
	PAD_NC(GPP_B14, NONE),
	PAD_NC(GPP_B15, NONE),
	PAD_NC(GPP_B16, NONE),
	PAD_NC(GPP_B17, NONE),
	PAD_NC(GPP_B19, NONE),
	PAD_NC(GPP_B20, NONE),
	PAD_NC(GPP_B21, NONE),
	PAD_NC(GPP_B22, NONE),
	PAD_NC(GPP_B23, NONE),

	PAD_NC(GPP_C0, NONE),
	PAD_NC(GPP_C1, NONE),
	PAD_NC(GPP_C3, NONE),
	PAD_NC(GPP_C4, NONE),
	PAD_NC(GPP_C6, NONE),
	PAD_NC(GPP_C7, NONE),
	PAD_NC(GPP_C8, NONE),
	PAD_NC(GPP_C9, NONE),
	PAD_NC(GPP_C10, NONE),
	PAD_NC(GPP_C11, NONE),
	PAD_NC(GPP_C12, NONE),
	PAD_NC(GPP_C13, NONE),
	PAD_NC(GPP_C14, NONE),
	PAD_NC(GPP_C15, NONE),
	PAD_NC(GPP_C18, NONE),
	PAD_NC(GPP_C19, NONE),
	PAD_NC(GPP_C22, NONE),
	PAD_NC(GPP_C23, NONE),

	PAD_NC(GPP_D0, NONE),
	PAD_NC(GPP_D1, NONE),
	PAD_NC(GPP_D2, NONE),
	PAD_NC(GPP_D3, NONE),
	PAD_NC(GPP_D4, NONE),
	PAD_NC(GPP_D5, NONE),
	PAD_NC(GPP_D6, NONE),
	PAD_NC(GPP_D7, NONE),
	PAD_NC(GPP_D8, NONE),
	PAD_NC(GPP_D9, NONE),
	PAD_NC(GPP_D10, NONE),
	PAD_NC(GPP_D11, NONE),
	PAD_NC(GPP_D13, NONE),
	PAD_NC(GPP_D14, NONE),
	PAD_NC(GPP_D15, NONE),
	PAD_NC(GPP_D17, NONE),
	PAD_NC(GPP_D18, NONE),
	PAD_NC(GPP_D19, NONE),
	PAD_NC(GPP_D20, NONE),
	PAD_NC(GPP_D21, NONE),
	PAD_NC(GPP_D22, NONE),
	PAD_NC(GPP_D23, NONE),

	PAD_NC(GPP_E0, NONE),
	PAD_NC(GPP_E2, NONE),
	PAD_NC(GPP_E3, NONE),
	PAD_NC(GPP_E4, NONE),
	PAD_NC(GPP_E6, NONE),
	PAD_NC(GPP_E7, NONE),
	PAD_NC(GPP_E8, NONE),
	PAD_NC(GPP_E9, NONE),
	PAD_NC(GPP_E10, NONE),
	PAD_NC(GPP_E11, NONE),
	PAD_NC(GPP_E12, NONE),
	PAD_NC(GPP_E22, NONE),
	PAD_NC(GPP_E23, NONE),

	PAD_NC(GPP_F0, NONE),
	PAD_NC(GPP_F1, NONE),
	PAD_NC(GPP_F2, NONE),
	PAD_NC(GPP_F3, NONE),
	PAD_NC(GPP_F8, NONE),
	PAD_NC(GPP_F9, NONE),
	PAD_NC(GPP_F10, NONE),
	PAD_NC(GPP_F11, NONE),
	PAD_NC(GPP_F12, NONE),
	PAD_NC(GPP_F13, NONE),
	PAD_NC(GPP_F14, NONE),
	PAD_NC(GPP_F15, NONE),
	PAD_NC(GPP_F16, NONE),
	PAD_NC(GPP_F17, NONE),
	PAD_NC(GPP_F18, NONE),
	PAD_NC(GPP_F19, NONE),
	PAD_NC(GPP_F20, NONE),
	PAD_NC(GPP_F21, NONE),
	PAD_NC(GPP_F22, NONE),

	PAD_NC(GPP_G0, NONE),
	PAD_NC(GPP_G1, NONE),
	PAD_NC(GPP_G2, NONE),
	PAD_NC(GPP_G3, NONE),
	PAD_NC(GPP_G4, NONE),
	PAD_NC(GPP_G5, NONE),
	PAD_NC(GPP_G6, NONE),
	PAD_NC(GPP_G7, NONE),

	PAD_NC(GPP_H0, NONE),
	PAD_NC(GPP_H3, NONE),
	PAD_NC(GPP_H4, NONE),
	PAD_NC(GPP_H5, NONE),
	PAD_NC(GPP_H6, NONE),
	PAD_NC(GPP_H7, NONE),
	PAD_NC(GPP_H8, NONE),
	PAD_NC(GPP_H9, NONE),
	PAD_NC(GPP_H10, NONE),
	PAD_NC(GPP_H11, NONE),
	PAD_NC(GPP_H12, NONE),
	PAD_NC(GPP_H13, NONE),
	PAD_NC(GPP_H14, NONE),
	PAD_NC(GPP_H15, NONE),
	PAD_NC(GPP_H16, NONE),
	PAD_NC(GPP_H17, NONE),
	PAD_NC(GPP_H19, NONE),
	PAD_NC(GPP_H20, NONE),
	PAD_NC(GPP_H22, NONE),
};
/* clang-format on */

const struct pad_config *variant_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}
