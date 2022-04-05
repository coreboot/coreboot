#ifndef GPIO_NAMES_ALDERLAKE_H
#define GPIO_NAMES_ALDERLAKE_H

#include "gpio_groups.h"

/* ----------------------------- Alder Lake H ----------------------------- */

const char *const alderlake_pch_h_group_a_names[] = {
	"GPP_A0",		"ESPI_IO0",		"n/a",				"n/a",	"n/a",	"n/a",	"USB_C_GPP_A0",
	"GPP_A1",		"ESPI_IO1",		"n/a",				"n/a",	"n/a",	"n/a",	"USB_C_GPP_A1",
	"GPP_A2",		"ESPI_IO2",		"SUSWARN# / SUSPWRDNACK",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_A2",
	"GPP_A3",		"ESPI_IO3",		"SUSACK#",			"n/a",	"n/a",	"n/a",	"USB_C_GPP_A3",
	"GPP_A4",		"ESPI_CS0#",		"n/a",				"n/a",	"n/a",	"n/a",	"USB_C_GPP_A4",
	"GPP_A5",		"ESPI_CLK",		"n/a",				"n/a",	"n/a",	"n/a",	"USB_C_GPP_A5",
	"GPP_A6",		"ESPI_RESET#",		"n/a",				"n/a",	"n/a",	"n/a",	"USB_C_GPP_A6",
	"GPP_A7",		"ESPI_CS1#",		"n/a",				"n/a",	"n/a",	"n/a",	"USB_C_GPP_A7",
	"GPP_A8",		"ESPI_CS2#",		"n/a",				"n/a",	"n/a",	"n/a",	"USB_C_GPP_A8",
	"GPP_A9",		"ESPI_CS3#",		"n/a",				"n/a",	"n/a",	"n/a",	"USB_C_GPP_A9",
	"GPP_A10",		"ESPI_ALERT0#",		"n/a",				"n/a",	"n/a",	"n/a",	"USB_C_GPP_A10",
	"GPP_A11",		"ESPI_ALERT1#",		"n/a",				"n/a",	"n/a",	"n/a",	"USB_C_GPP_A11",
	"GPP_A12",		"ESPI_ALERT2#",		"n/a",				"n/a",	"n/a",	"n/a",	"USB_C_GPP_A12",
	"GPP_A13",		"ESPI_ALERT3#",		"n/a",				"n/a",	"n/a",	"n/a",	"USB_C_GPP_A13",
	"GPP_A14",		"n/a",			"n/a",				"n/a",	"n/a",	"n/a",	"USB_C_GPP_A14",
	"ESPI_CLK_LOOPBK",	"ESPI_CLK_LOOPBK",	"n/a",				"n/a",	"n/a",	"n/a",	"n/a",
};

const struct gpio_group alderlake_pch_h_group_a = {
	.display	= "------- GPIO Group GPP_A -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_h_group_a_names) / 7,
	.func_count	= 7,
	.pad_names	= alderlake_pch_h_group_a_names,
};

const char *const alderlake_pch_h_group_b_names[] = {
	"GPP_B0",	"PCIE_LNK_DOWN",	"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_B0",
	"GPP_B1",	"ISH_UART0_RTS#",	"GSPI2_CS1#",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_B1",
	"GPP_B2",	"VRALERT#",		"n/a",		"n/a",	"n/a",  "n/a",	"USB_C_GPP_B2",
	"GPP_B3",	"CPU_GP2",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_B3",
	"GPP_B4",	"CPU_GP3",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_B4",
	"GPP_B5",	"SX_EXIT_HOLDOFF#",	"ISH_GP6",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_B5",
	"GPP_B6",	"CLKOUT_48",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_B6",
	"GPP_B7",	"ISH_GP7",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_B7",
	"GPP_B8",	"ISH_GP0",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_B8",
	"GPP_B9",	"ISH_GP1",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_B9",
	"GPP_B10",	"ISH_GP2",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_B10",
	"GPP_B11",	"n/a",			"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_B11",
	"GPP_B12",	"SLP_S0#",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_B12",
	"GPP_B13",	"PLTRST#",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_B13",
	"GPP_B14",	"SPKR",			"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_B14",
	"GPP_B15",	"ISH_GP3",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_B15",
	"GPP_B16",	"ISH_GP4",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_B16",
	"GPP_B17",	"ISH_GP5",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_B17",
	"GPP_B18",	"PMCALERT#",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_B18",
	"GPP_B19",	"RSVD",			"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_B19",
	"GPP_B20",	"RSVD",			"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_B20",
	"GPP_B21",	"n/a",			"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_B21",
	"GPP_B22",	"n/a",			"n/a",		"n/a",  "n/a",	"n/a",	"USB_C_GPP_B22",
	"GPP_B23",	"SML1ALERT#",		"PCHHOT#",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_B23",
};

const struct gpio_group alderlake_pch_h_group_b = {
	.display	= "------- GPIO Group GPP_B -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_h_group_b_names) / 7,
	.func_count	= 7,
	.pad_names	= alderlake_pch_h_group_b_names,
};

const char *const alderlake_pch_h_group_c_names[] = {
	"GPP_C0",	"SMBCLK",		"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_C0",
	"GPP_C1",	"SMBDATA",		"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_C1",
	"GPP_C2",	"SMBALERT#",		"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_C2",
	"GPP_C3",	"ISH_UART0_RXD",	"n/a",			"I2C2_SDA",	"n/a",	"n/a",	"USB_C_GPP_C3",
	"GPP_C4",	"ISH_UART0_TXD",	"n/a",			"I2C2_SCL",	"n/a",	"n/a",	"USB_C_GPP_C4",
	"GPP_C5",	"SML0ALERT#",		"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_C5",
	"GPP_C6",	"ISH_I2C2_SDA",		"I2C3_SDA",		"n/a",		"n/a",	"n/a",	"USB_C_GPP_C6",
	"GPP_C7",	"ISH_I2C2_SCL",		"I2C3_SCL",		"n/a",		"n/a",	"n/a",	"USB_C_GPP_C7",
	"GPP_C8",	"UART0_RXD",		"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_C8",
	"GPP_C9",	"UART0_TXD",		"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_C9",
	"GPP_C10",	"UART0_RTS#",		"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_C10",
	"GPP_C11",	"UART0_CTS#",		"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_C11",
	"GPP_C12",	"UART1_RXD",		"ISH_UART1_RXD",	"n/a",		"n/a",	"n/a",	"USB_C_GPP_C12",
	"GPP_C13",	"UART1_TXD",		"ISH_UART1_TXD",	"n/a",		"n/a",	"n/a",	"USB_C_GPP_C13",
	"GPP_C14",	"UART1_RTS#",		"ISH_UART1_RTS#",	"n/a",		"n/a",	"n/a",	"USB_C_GPP_C14",
	"GPP_C15",	"UART1_CTS#",		"ISH_UART1_CTS#",	"n/a",		"n/a",	"n/a",	"USB_C_GPP_C15",
	"GPP_C16",	"I2C0_SDA",		"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_C16",
	"GPP_C17",	"I2C0_SCL",		"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_C17",
	"GPP_C18",	"I2C1_SDA",		"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_C18",
	"GPP_C19",	"I2C1_SCL",		"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_C19",
	"GPP_C20",	"UART2_RXD",		"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_C20",
	"GPP_C21",	"UART2_TXD",		"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_C21",
	"GPP_C22",	"UART2_RTS#",		"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_C22",
	"GPP_C23",	"UART2_CTS#",		"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_C23",
};

const struct gpio_group alderlake_pch_h_group_c = {
	.display	= "------- GPIO Group GPP_C -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_h_group_c_names) / 7,
	.func_count	= 7,
	.pad_names	= alderlake_pch_h_group_c_names,
};

const char *const alderlake_pch_h_group_d_names[] = {
	"GPP_D0",	"SRCCLKREQ0#",	"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_D0",
	"GPP_D1",	"SRCCLKREQ1#",	"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_D1",
	"GPP_D2",	"SRCCLKREQ2#",	"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_D2",
	"GPP_D3",	"SRCCLKREQ3#",	"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_D3",
	"GPP_D4",	"SML1CLK",	"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_D4",
	"GPP_D5",	"n/a",		"CNV_RF_RESET#",	"n/a",		"n/a",	"n/a",	"USB_C_GPP_D5",
	"GPP_D6",	"n/a",		"n/a",			"MODEM_CLKREQ",	"n/a",	"n/a",	"USB_C_GPP_D6",
	"GPP_D7",	"n/a",		"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_D7",
	"GPP_D8",	"n/a",		"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_D8",
	"GPP_D9",	"SML0CLK",	"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_D9",
	"GPP_D10",	"SML0DATA",	"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_D10",
	"GPP_D11",	"SRCCLKREQ4#",	"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_D11",
	"GPP_D12",	"SRCCLKREQ5#",	"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_D12",
	"GPP_D13",	"SRCCLKREQ6#",	"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_D13",
	"GPP_D14",	"SRCCLKREQ7#",	"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_D14",
	"GPP_D15",	"SML1DATA",	"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_D15",
	"GPP_D16",	"GSPI3_CS0#",	"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_D16",
	"GPP_D17",	"GSPI3_CLK",	"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_D17",
	"GPP_D18",	"GSPI3_MISO",	"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_D18",
	"GPP_D19",	"GSPI3_MOSI",	"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_D19",
	"GPP_D20",	"UART3_RXD",	"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_D20",
	"GPP_D21",	"UART3_TXD",	"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_D21",
	"GPP_D22",	"UART3_RTS#",	"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_D22",
	"GPP_D23",	"UART3_CTS#",	"n/a",			"n/a",		"n/a",	"n/a",	"USB_C_GPP_D23",
};

const struct gpio_group alderlake_pch_h_group_d = {
	.display	= "------- GPIO Group GPP_D -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_h_group_d_names) / 7,
	.func_count	= 7,
	.pad_names	= alderlake_pch_h_group_d_names,
};

const char *const alderlake_pch_h_group_e_names[] = {
	"GPP_E0",		"SATAXPCIE0",		"SATAGP0",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_E0",
	"GPP_E1",		"SATAXPCIE1",		"SATAGP1",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_E1",
	"GPP_E2",		"SATAXPCIE2",		"SATAGP2",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_E2",
	"GPP_E3",		"CPU_GP0",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_E3",
	"GPP_E4",		"SATA_DEVSLP0",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_E4",
	"GPP_E5",		"SATA_DEVSLP1",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_E5",
	"GPP_E6",		"SATA_DEVSLP2",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_E6",
	"GPP_E7",		"CPU_GP1",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_E7",
	"GPP_E8",		"SATALED#",		"SPI1_CS1#",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_E8",
	"GPP_E9",		"USB_OC0#",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_E9",
	"GPP_E10",		"USB_OC1#",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_E10",
	"GPP_E11",		"USB_OC2#",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_E11",
	"GPP_E12",		"USB_OC3#",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_E12",
	"GPP_E13",		"n/a",			"n/a",		"SBK0",	"BK0",	"n/a",	"USB_C_GPP_E13",
	"GPP_E14",		"n/a",			"n/a",		"SBK1",	"BK1",	"n/a",	"USB_C_GPP_E14",
	"GPP_E15",		"n/a",			"n/a",		"SBK2",	"BK2",	"n/a",	"USB_C_GPP_E15",
	"GPP_E16",		"n/a",			"n/a",		"SBK3",	"BK3",	"n/a",	"USB_C_GPP_E16",
	"GPP_E17",		"n/a",			"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_E17",
	"GPP_E18",		"n/a",			"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_E18",
	"GPP_E19",		"n/a",			"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_E19",
	"GPP_E20",		"n/a",			"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_E20",
	"GPP_E21",		"ISH_UART0_CTS#",	"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_E21",
	"SPI1_THC0_CLK_LOOPBK",	"SPI1_THC0_CLK_LOOPBK",	"n/a",		"n/a",	"n/a",	"n/a",	"n/a",
};

const struct gpio_group alderlake_pch_h_group_e = {
	.display	= "------- GPIO Group GPP_E -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_h_group_e_names) / 7,
	.func_count	= 7,
	.pad_names	= alderlake_pch_h_group_e_names,
};

const char *const alderlake_pch_h_group_f_names[] = {
	"GPP_F0",	"SATAXPCIE3",		"SATAGP3",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_F0",
	"GPP_F1",	"SATAXPCIE4",		"SATAGP4",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_F1",
	"GPP_F2",	"SATAXPCIE5",		"SATAGP5",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_F2",
	"GPP_F3",	"SATAXPCIE6",		"SATAGP6",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_F3",
	"GPP_F4",	"SATAXPCIE7",		"SATAGP7",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_F4",
	"GPP_F5",	"SATA_DEVSLP3",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_F5",
	"GPP_F6",	"SATA_DEVSLP4",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_F6",
	"GPP_F7",	"SATA_DEVSLP5",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_F7",
	"GPP_F8",	"SATA_DEVSLP6",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_F8",
	"GPP_F9",	"SATA_DEVSLP7",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_F9",
	"GPP_F10",	"SATA_SCLOCK",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_F10",
	"GPP_F11",	"SATA_SLOAD",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_F11",
	"GPP_F12",	"SATA_SDATAOUT1",	"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_F12",
	"GPP_F13",	"SATA_SDATAOUT0",	"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_F13",
	"GPP_F14",	"PS_ON#",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_F14",
	"GPP_F15",	"n/a",			"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_F15",
	"GPP_F16",	"n/a",			"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_F16",
	"GPP_F17",	"n/a",			"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_F17",
	"GPP_F18",	"n/a",			"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_F18",
	"GPP_F19",	"eDP_VDDEN",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_F19",
	"GPP_F20",	"eDP_BKLTEN",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_F20",
	"GPP_F21",	"eDP_BKLTCTL",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_F21",
	"GPP_F22",	"VNN_CTRL",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_F22",
	"GPP_F23",	"n/a",			"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_F23",
};

const struct gpio_group alderlake_pch_h_group_f = {
	.display	= "------- GPIO Group GPP_F -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_h_group_f_names) / 7,
	.func_count	= 7,
	.pad_names	= alderlake_pch_h_group_f_names,
};

const char *const alderlake_pch_h_group_g_names[] = {
	"GPP_G0",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_G0",
	"GPP_G1",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_G1",
	"GPP_G2",	"DNX_FORCE_RELOAD",	"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_G2",
	"GPP_G3",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_G3",
	"GPP_G4",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_G4",
	"GPP_G5",	"SLP_DRAM#",		"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_G5",
	"GPP_G6",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_G6",
	"GPP_G7",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_G7",
};

const struct gpio_group alderlake_pch_h_group_g = {
	.display	= "------- GPIO Group GPP_G -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_h_group_g_names) / 7,
	.func_count	= 7,
	.pad_names	= alderlake_pch_h_group_g_names,
};

const char *const alderlake_pch_h_group_h_names[] = {
	"GPP_H0",	"n/a",		"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_H0",
	"GPP_H1",	"n/a",		"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_H1",
	"GPP_H2",	"SRCCLKREQ8#",	"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_H2",
	"GPP_H3",	"SRCCLKREQ9#",	"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_H3",
	"GPP_H4",	"SRCCLKREQ10#",	"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_H4",
	"GPP_H5",	"SRCCLKREQ11#",	"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_H5",
	"GPP_H6",	"SRCCLKREQ12#",	"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_H6",
	"GPP_H7",	"SRCCLKREQ13#",	"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_H7",
	"GPP_H8",	"SRCCLKREQ14#",	"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_H8",
	"GPP_H9",	"SRCCLKREQ15#",	"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_H9",
	"GPP_H10",	"SML2CLK",	"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_H10",
	"GPP_H11",	"SML2DATA",	"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_H11",
	"GPP_H12",	"SML2ALERT#",	"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_H12",
	"GPP_H13",	"SML3CLK",	"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_H13",
	"GPP_H14",	"SML3DATA",	"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_H14",
	"GPP_H15",	"SML3ALERT#",	"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_H15",
	"GPP_H16",	"SML4CLK",	"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_H16",
	"GPP_H17",	"SML4DATA",	"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_H17",
	"GPP_H18",	"SML4ALERT#",	"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_H18",
	"GPP_H19",	"ISH_I2C0_SDA",	"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_H19",
	"GPP_H20",	"ISH_I2C0_SCL",	"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_H20",
	"GPP_H21",	"ISH_I2C1_SDA",	"SMI#",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_H21",
	"GPP_H22",	"ISH_I2C1_SCL",	"NMI#",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_H22",
	"GPP_H23",	"TIME_SYNC0",	"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_H23",

};

const struct gpio_group alderlake_pch_h_group_h = {
	.display	= "------- GPIO Group GPP_H -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_h_group_h_names) / 7,
	.func_count	= 7,
	.pad_names	= alderlake_pch_h_group_h_names,
};

const char *const alderlake_pch_h_group_i_names[] = {
	"GPP_I0",		"EXT_PWR_GATE#",	"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_I0",
	"GPP_I1",		"DDSP_HPD1",		"DISP_MISC1",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_I1",
	"GPP_I2",		"DDSP_HPD2",		"DISP_MISC2",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_I2",
	"GPP_I3",		"DDSP_HPD3",		"DISP_MISC3",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_I3",
	"GPP_I4",		"DDSP_HPD4",		"DISP_MISC4",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_I4",
	"GPP_I5",		"DDPB_CTRLCLK",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_I5",
	"GPP_I6",		"DDPB_CTRLDATA",	"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_I6",
	"GPP_I7",		"DDPC_CTRLCLK",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_I7",
	"GPP_I8",		"DDPC_CTRLDATA",	"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_I8",
	"GPP_I9",		"GSPI0_CS1#",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_I9",
	"GPP_I10",		"GSPI1_CS1#",		"TIME_SYNC1",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_I10",
	"GPP_I11",		"USB_OC4#",		"I2C4_SDA",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_I11",
	"GPP_I12",		"USB_OC5#",		"I2C4_SCL",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_I12",
	"GPP_I13",		"USB_OC6#",		"I2C5_SDA",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_I13",
	"GPP_I14",		"USB_OC7#",		"I2C5_SCL",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_I14",
	"GPP_I15",		"GSPI0_CS0#",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_I16",		"GSPI0_CLK",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_I17",		"GSPI0_MISO",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_I18",		"GSPI0_MOSI",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_I19",		"GSPI1_CS0#",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_I20",		"GSPI1_CLK",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_I21",		"GSPI1_MISO",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_I22",		"GSPI1_MOSI",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",
	"GSPI0_CLK_LOOPBK",	"GSPI0_CLK_LOOPBK",	"n/a",		"n/a",	"n/a",	"n/a",	"n/a",
	"GSPI1_CLK_LOOPBK",	"GSPI1_CLK_LOOPBK",	"n/a",		"n/a",	"n/a",	"n/a",	"n/a",

};

const struct gpio_group alderlake_pch_h_group_i = {
	.display	= "------- GPIO Group GPP_I -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_h_group_i_names) / 7,
	.func_count	= 7,
	.pad_names	= alderlake_pch_h_group_i_names,
};

const char *const alderlake_pch_h_group_j_names[] = {
	"GPP_J0",	"CNV_PA_BLANKING",	"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_J0",
	"GPP_J1",	"CPU_C10_GATE#",	"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_J1",
	"GPP_J2",	"CNV_BRI_DT",		"UART0_RTS#",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_J2",
	"GPP_J3",	"CNV_BRI_RSP",		"UART0_RXD",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_J3",
	"GPP_J4",	"CNV_RGI_DT",		"UART0_TXD",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_J4",
	"GPP_J5",	"CNV_RGI_RSP",		"UART0_CTS#",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_J5",
	"GPP_J6",	"CNV_MFUART2_RXD",	"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_J6",
	"GPP_J7",	"CNV_MFUART2_TXD",	"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_J7",
	"GPP_J8",	"SRCCLKREQ16#",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_J8",
	"GPP_J9",	"SRCCLKREQ17#",		"n/a",		"n/a",	"n/a",	"n/a",	"USB_C_GPP_J9",
	"GPP_J10",	"BSSB_LS_RX",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_J11",	"BSSB_LS_TX",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",

};

const struct gpio_group alderlake_pch_h_group_j = {
	.display	= "------- GPIO Group GPP_J -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_h_group_j_names) / 7,
	.func_count	= 7,
	.pad_names	= alderlake_pch_h_group_j_names,
};


const char *const alderlake_pch_h_group_k_names[] = {
	"GPP_K0",	"GSXDOUT",	"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_K0",
	"GPP_K1",	"GSXSLOAD",	"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_K1",
	"GPP_K2",	"GSXDIN",	"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_K2",
	"GPP_K3",	"GSXSRESET#",	"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_K3",
	"GPP_K4",	"GSXCLK",	"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_K4",
	"GPP_K5",	"ADR_COMPLETE",	"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_K5",
	"GPP_K6",	"n/a",		"n/a",	"n/a",	"n/a",	"n/a",	"n/a",		/* Not available */
	"GPP_K7",	"n/a",		"n/a",	"n/a",	"n/a",	"n/a",	"n/a",		/* Not available */
	"GPP_K8",	"CORE_VID0",	"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_K8",
	"GPP_K9",	"CORE_VID1",	"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_K9",
	"GPP_K10",	"n/a",		"n/a",	"n/a",	"n/a",	"n/a",	"n/a",		/* Not available */
	"GPP_K11",	"n/a",		"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_K11",
	"SYS_PWROK",	"SYS_PWROK",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"SYS_RESETB",	"SYS_RESETB",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"MLK_RSTB",	"MLK_RSTB",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a"
};

const struct gpio_group alderlake_pch_h_group_k = {
	.display	= "------- GPIO Group GPP_K -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_h_group_k_names) / 7,
	.func_count	= 7,
	.pad_names	= alderlake_pch_h_group_k_names,
};

const char *const alderlake_pch_h_group_r_names[] = {
	"GPP_R0",		"HDA_BCLK",		"n/a",			"n/a",		"HDACPU_BCLK",	"n/a",	"USB_C_GPP_R0",
	"GPP_R1",		"HDA_SYNC",		"n/a",			"n/a",		"n/a",		"n/a",	"USB_C_GPP_R1",
	"GPP_R2",		"HDA_SDO",		"n/a",			"n/a",		"HDACPU_SDO",	"n/a",	"USB_C_GPP_R2",
	"GPP_R3",		"HDA_SDI0",		"n/a",			"n/a",		"HDACPU_SDI",	"n/a",	"USB_C_GPP_R3",
	"GPP_R4",		"HDA_RST#",		"n/a",			"n/a",		"n/a",		"n/a",	"USB_C_GPP_R4",
	"GPP_R5",		"HDA_SDI1",		"n/a",			"n/a",		"n/a",		"n/a",	"USB_C_GPP_R5",
	"GPP_R6",		"n/a",			"n/a",			"n/a",		"n/a",		"n/a",	"USB_C_GPP_R6",
	"GPP_R7",		"n/a",			"n/a",			"n/a",		"n/a",		"n/a",	"USB_C_GPP_R7",
	"GPP_R8",		"n/a",			"n/a",			"n/a",		"n/a",		"n/a",	"USB_C_GPP_R8",
	"GPP_R9",		"DDSP_HPDA",		"DISP_MISCA",		"n/a",		"n/a",		"n/a",	"USB_C_GPP_R9",
	"GPP_R10",		"DDSP_HPDB",		"DISP_MISCB",		"n/a",		"n/a",		"n/a",	"USB_C_GPP_R10",
	"GPP_R11",		"DDSP_HPDC",		"DISP_MISCC",		"n/a",		"n/a",		"n/a",	"USB_C_GPP_R11",
	"GPP_R12",		"ISH_SPI_CS#",		"DDP3_CTRLCLK",		"GSPI2_CS0#",	"n/a",		"n/a",	"USB_C_GPP_R12",
	"GPP_R13",		"ISH_SPI_CLK",		"DDP3_CTRLDATA",	"GSPI2_CLK",	"n/a",		"n/a",	"USB_C_GPP_R13",
	"GPP_R14",		"ISH_SPI_MISO",		"DDP4_CTRLCLK",		"GSPI2_MISO",	"n/a",		"n/a",	"USB_C_GPP_R14",
	"GPP_R15",		"ISH_SPI_MOSI",		"DDP4_CTRLDATA",	"GSPI2_MOSI",	"n/a",		"n/a",	"USB_C_GPP_R15",
	"GPP_R16",		"DDP1_CTRLCLK",		"n/a",			"n/a",		"n/a",		"n/a",	"USB_C_GPP_R16",
	"GPP_R17",		"DDP1_CTRLDATA",	"n/a",			"n/a",		"n/a",		"n/a",	"USB_C_GPP_R17",
	"GPP_R18",		"DDP2_CTRLCLK",		"n/a",			"n/a",		"n/a",		"n/a",	"USB_C_GPP_R18",
	"GPP_R19",		"DDP2_CTRLDATA",	"n/a",			"n/a",		"n/a",		"n/a",	"USB_C_GPP_R19",
	"GPP_R20",		"DDPA_CTRLCLK",		"n/a",			"n/a",		"n/a",		"n/a",	"n/a",
	"GPP_R21",		"DDPA_CTRLDATA",	"n/a",			"n/a",		"n/a",		"n/a",	"n/a",
	"GSPI2_CLK_LOOPBK",	"GSPI2_CLK_LOOPBK",	"n/a",			"n/a",		"n/a",		"n/a",	"n/a",
};

const struct gpio_group alderlake_pch_h_group_r = {
	.display	= "------- GPIO Group GPP_R -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_h_group_r_names) / 7,
	.func_count	= 7,
	.pad_names	= alderlake_pch_h_group_r_names,
};

const char *const alderlake_pch_h_group_s_names[] = {
	"GPP_S0",	"SNDW1_CLK",	"n/a",
	"GPP_S1",	"SNDW1_DATA",	"n/a",
	"GPP_S2",	"SNDW2_CLK",	"DMIC_CLKB0",
	"GPP_S3",	"SNDW2_DATA",	"DMIC_CLKB1",
	"GPP_S4",	"SNDW3_CLK",	"DMIC_CLKA1",
	"GPP_S5",	"SNDW3_DATA",	"DMIC_DATA1",
	"GPP_S6",	"SNDW4_CLK",	"DMIC_CLKA0",
	"GPP_S7",	"SNDW4_DATA",	"DMIC_DATA0",
};

const struct gpio_group alderlake_pch_h_group_s = {
	.display	= "------- GPIO Group GPP_S -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_h_group_s_names) / 3,
	.func_count	= 3,
	.pad_names	= alderlake_pch_h_group_s_names,
};

const char *const alderlake_pch_h_group_gpd_names[] = {
	"GPD0",		"BATLOW#",
	"GPD1",		"ACPRESENT",
	"GPD2",		"LAN_WAKE#",
	"GPD3",		"PWRBTN#",
	"GPD4",		"SLP_S3#",
	"GPD5",		"SLP_S4#",
	"GPD6",		"SLP_A#",
	"GPD7",		"n/a",
	"GPD8",		"SUSCLK",
	"GPD9",		"SLP_WLAN#",
	"GPD10",	"SLP_S5#",
	"GPD11",	"LANPHYPC",
	"GPD12",	"n/a",
};

const struct gpio_group alderlake_pch_h_group_gpd = {
	.display	= "------- GPIO Group GPD -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_h_group_gpd_names) / 2,
	.func_count	= 2,
	.pad_names	= alderlake_pch_h_group_gpd_names,
};

const char *const alderlake_pch_h_group_vgpio_names[] = {
	"VGPIO_0",	"VGPIO_0",
	"VGPIO_4",	"VGPIO_4",
	"VGPIO_5",	"VGPIO_5",
	"VGPIO_6",	"VGPIO_6",
	"VGPIO_7",	"VGPIO_7",
	"VGPIO_8",	"VGPIO_8",
	"VGPIO_9",	"VGPIO_9",
	"VGPIO_10",	"VGPIO_10",
	"VGPIO_11",	"VGPIO_11",
	"VGPIO_12",	"VGPIO_12",
	"VGPIO_13",	"VGPIO_13",
	"VGPIO_18",	"VGPIO_18",
	"VGPIO_19",	"VGPIO_19",
	"VGPIO_20",	"VGPIO_20",
	"VGPIO_21",	"VGPIO_21",
	"VGPIO_22",	"VGPIO_22",
	"VGPIO_23",	"VGPIO_23",
	"VGPIO_24",	"VGPIO_24",
	"VGPIO_25",	"VGPIO_25",
	"VGPIO_30",	"VGPIO_30",
	"VGPIO_31",	"VGPIO_31",
	"VGPIO_32",	"VGPIO_32",
	"VGPIO_33",	"VGPIO_33",
	"VGPIO_34",	"VGPIO_34",
	"VGPIO_35",	"VGPIO_35",
	"VGPIO_36",	"VGPIO_36",
	"VGPIO_37",	"VGPIO_37",
};

const struct gpio_group alderlake_pch_h_group_vgpio = {
	.display	= "------- GPIO Group vGPIO -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_h_group_vgpio_names) / 2,
	.func_count	= 2,
	.pad_names	= alderlake_pch_h_group_vgpio_names,
};

const char *const alderlake_pch_h_group_vgpio_0_names[] = {
	"VGPIO_USB_0",		"VGPIO_USB_0",
	"VGPIO_USB_1",		"VGPIO_USB_1",
	"VGPIO_USB_2",		"VGPIO_USB_2",
	"VGPIO_USB_3",		"VGPIO_USB_3",
	"VGPIO_USB_8",		"VGPIO_USB_8",
	"VGPIO_USB_9",		"VGPIO_USB_9",
	"VGPIO_USB_10",		"VGPIO_USB_10",
	"VGPIO_USB_11",		"VGPIO_USB_11",
};

const struct gpio_group alderlake_pch_h_group_vgpio_0 = {
	.display	= "------- GPIO Group vGPIO_0 -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_h_group_vgpio_0_names) / 2,
	.func_count	= 2,
	.pad_names	= alderlake_pch_h_group_vgpio_0_names,
};

const char *const alderlake_pch_h_group_spi_names[] = {
	"SPI0_IO_2",		"SPI0_IO_2",
	"SPI0_IO_3",		"SPI0_IO_3",
	"SPI0_MOSI_IO_0",	"SPI0_MOSI_IO_0",
	"SPI0_MISO_IO_1",	"SPI0_MISO_IO_1",
	"SPI0_TPM_CSB",		"SPI0_TPM_CSB",
	"SPI0_FLASH_0_CSB",	"SPI0_FLASH_0_CSB",
	"SPI0_FLASH_1_CSB",	"SPI0_FLASH_1_CSB",
	"SPI0_CLK",		"SPI0_CLK",
	"SPI0_CLK_LOOPBK",	"SPI0_CLK_LOOPBK",
};

const struct gpio_group alderlake_pch_h_group_spi = {
	.display	= "------- GPIO Group SPI -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_h_group_spi_names) / 2,
	.func_count	= 2,
	.pad_names	= alderlake_pch_h_group_spi_names,
};

const char *const alderlake_pch_h_group_vgpio_3_names[] = {
	"VGPIO_PCIE_0",		"VGPIO_PCIE_0",
	"VGPIO_PCIE_1",		"VGPIO_PCIE_1",
	"VGPIO_PCIE_2",		"VGPIO_PCIE_2",
	"VGPIO_PCIE_3",		"VGPIO_PCIE_3",
	"VGPIO_PCIE_4",		"VGPIO_PCIE_4",
	"VGPIO_PCIE_5",		"VGPIO_PCIE_5",
	"VGPIO_PCIE_6",		"VGPIO_PCIE_6",
	"VGPIO_PCIE_7",		"VGPIO_PCIE_7",
	"VGPIO_PCIE_8",		"VGPIO_PCIE_8",
	"VGPIO_PCIE_9",		"VGPIO_PCIE_9",
	"VGPIO_PCIE_10",	"VGPIO_PCIE_10",
	"VGPIO_PCIE_11",	"VGPIO_PCIE_11",
	"VGPIO_PCIE_12",	"VGPIO_PCIE_12",
	"VGPIO_PCIE_13",	"VGPIO_PCIE_13",
	"VGPIO_PCIE_14",	"VGPIO_PCIE_14",
	"VGPIO_PCIE_15",	"VGPIO_PCIE_15",
	"VGPIO_PCIE_16",	"VGPIO_PCIE_16",
	"VGPIO_PCIE_17",	"VGPIO_PCIE_17",
	"VGPIO_PCIE_18",	"VGPIO_PCIE_18",
	"VGPIO_PCIE_19",	"VGPIO_PCIE_19",
	"VGPIO_PCIE_20",	"VGPIO_PCIE_20",
	"VGPIO_PCIE_21",	"VGPIO_PCIE_21",
	"VGPIO_PCIE_22",	"VGPIO_PCIE_22",
	"VGPIO_PCIE_23",	"VGPIO_PCIE_23",
	"VGPIO_PCIE_24",	"VGPIO_PCIE_24",
	"VGPIO_PCIE_25",	"VGPIO_PCIE_25",
	"VGPIO_PCIE_26",	"VGPIO_PCIE_26",
	"VGPIO_PCIE_27",	"VGPIO_PCIE_27",
	"VGPIO_PCIE_28",	"VGPIO_PCIE_28",
	"VGPIO_PCIE_29",	"VGPIO_PCIE_29",
	"VGPIO_PCIE_30",	"VGPIO_PCIE_30",
	"VGPIO_PCIE_31",	"VGPIO_PCIE_31",
	"VGPIO_PCIE_32",	"VGPIO_PCIE_32",
	"VGPIO_PCIE_33",	"VGPIO_PCIE_33",
	"VGPIO_PCIE_34",	"VGPIO_PCIE_34",
	"VGPIO_PCIE_35",	"VGPIO_PCIE_35",
	"VGPIO_PCIE_36",	"VGPIO_PCIE_36",
	"VGPIO_PCIE_37",	"VGPIO_PCIE_37",
	"VGPIO_PCIE_38",	"VGPIO_PCIE_38",
	"VGPIO_PCIE_39",	"VGPIO_PCIE_39",
	"VGPIO_PCIE_40",	"VGPIO_PCIE_40",
	"VGPIO_PCIE_41",	"VGPIO_PCIE_41",
	"VGPIO_PCIE_42",	"VGPIO_PCIE_42",
	"VGPIO_PCIE_43",	"VGPIO_PCIE_43",
	"VGPIO_PCIE_44",	"VGPIO_PCIE_44",
	"VGPIO_PCIE_45",	"VGPIO_PCIE_45",
	"VGPIO_PCIE_46",	"VGPIO_PCIE_46",
	"VGPIO_PCIE_47",	"VGPIO_PCIE_47",
	"VGPIO_PCIE_48",	"VGPIO_PCIE_48",
	"VGPIO_PCIE_49",	"VGPIO_PCIE_49",
	"VGPIO_PCIE_50",	"VGPIO_PCIE_50",
	"VGPIO_PCIE_51",	"VGPIO_PCIE_51",
	"VGPIO_PCIE_52",	"VGPIO_PCIE_52",
	"VGPIO_PCIE_53",	"VGPIO_PCIE_53",
	"VGPIO_PCIE_54",	"VGPIO_PCIE_54",
	"VGPIO_PCIE_55",	"VGPIO_PCIE_55",
	"VGPIO_PCIE_56",	"VGPIO_PCIE_56",
	"VGPIO_PCIE_57",	"VGPIO_PCIE_57",
	"VGPIO_PCIE_58",	"VGPIO_PCIE_58",
	"VGPIO_PCIE_59",	"VGPIO_PCIE_59",
	"VGPIO_PCIE_60",	"VGPIO_PCIE_60",
	"VGPIO_PCIE_61",	"VGPIO_PCIE_61",
	"VGPIO_PCIE_62",	"VGPIO_PCIE_62",
	"VGPIO_PCIE_63",	"VGPIO_PCIE_63",
	"VGPIO_PCIE_64",	"VGPIO_PCIE_64",
	"VGPIO_PCIE_65",	"VGPIO_PCIE_65",
	"VGPIO_PCIE_66",	"VGPIO_PCIE_66",
	"VGPIO_PCIE_67",	"VGPIO_PCIE_67",
	"VGPIO_PCIE_68",	"VGPIO_PCIE_68",
	"VGPIO_PCIE_69",	"VGPIO_PCIE_69",
	"VGPIO_PCIE_70",	"VGPIO_PCIE_70",
	"VGPIO_PCIE_71",	"VGPIO_PCIE_71",
	"VGPIO_PCIE_72",	"VGPIO_PCIE_72",
	"VGPIO_PCIE_73",	"VGPIO_PCIE_73",
	"VGPIO_PCIE_74",	"VGPIO_PCIE_74",
	"VGPIO_PCIE_75",	"VGPIO_PCIE_75",
	"VGPIO_PCIE_76",	"VGPIO_PCIE_76",
	"VGPIO_PCIE_77",	"VGPIO_PCIE_77",
	"VGPIO_PCIE_78",	"VGPIO_PCIE_78",
	"VGPIO_PCIE_79",	"VGPIO_PCIE_79",
	"VGPIO_PCIE_80",	"VGPIO_PCIE_80",
	"VGPIO_PCIE_81",	"VGPIO_PCIE_81",
	"VGPIO_PCIE_82",	"VGPIO_PCIE_82",
	"VGPIO_PCIE_83",	"VGPIO_PCIE_83",
};

const struct gpio_group alderlake_pch_h_group_vgpio_3 = {
	.display	= "------- GPIO Group vGPIO_3 -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_h_group_vgpio_3_names) / 2,
	.func_count	= 2,
	.pad_names	= alderlake_pch_h_group_vgpio_3_names,
};

const char *const alderlake_pch_h_group_jtag_names[] = {
	"JTAG_TDO",	"JTAG_TDO",
	"JTAGX",	"JTAGX",
	"PRDYB",	"PRDYB",
	"PREQB",	"PREQB",
	"JTAG_TDI",	"JTAG_TDI",
	"JTAG_TMS",	"JTAG_TMS",
	"JTAG_TCK",	"JTAG_TCK",
	"DBG_PMODE",	"DBG_PMODE",
	"CPU_TRSTB",	"CPU_TRSTB",
};

const struct gpio_group alderlake_pch_h_group_jtag = {
	.display	= "------- GPIO Group JTAG -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_h_group_jtag_names) / 2,
	.func_count	= 2,
	.pad_names	= alderlake_pch_h_group_jtag_names,
};

const char *const alderlake_pch_h_group_cpu_names[] = {
	"HDACPU_SDI",		"HDACPU_SDI",
	"HDACPU_SDO",		"HDACPU_SDO",
	"HDACPU_SCLK",		"HDACPU_SCLK",
	"PM_SYNC",		"PM_SYNC",
	"PECI",			"PECI",
	"CPUPWRGD",		"CPUPWRGD",
	"THRMTRIPB",		"THRMTRIPB",
	"PLTRST_CPUB",		"PLTRST_CPUB",
	"PM_DOWN",		"PM_DOWN",
	"TRIGGER_IN",		"TRIGGER_IN",
	"TRIGGER_OUT",		"TRIGGER_OUT",
	"CLKOUT_CPURTC",	"CLKOUT_CPURTC",
	"VCCST_OVERRIDE",	"VCCST_OVERRIDE",
	"C10_WAKE",		"C10_WAKE",
};

const struct gpio_group alderlake_pch_h_group_cpu = {
	.display	= "------- GPIO Group CPU -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_h_group_cpu_names) / 2,
	.func_count	= 2,
	.pad_names	= alderlake_pch_h_group_cpu_names,
};

const struct gpio_group *const alderlake_pch_h_community_0_groups[] = {
	&alderlake_pch_h_group_i,
	&alderlake_pch_h_group_r,
	&alderlake_pch_h_group_j,
	&alderlake_pch_h_group_vgpio,
	&alderlake_pch_h_group_vgpio_0,
};

const struct gpio_community alderlake_pch_h_community_0 = {
	.name		= "------- GPIO Community 0 -------",
	.pcr_port_id	= 0x6e,
	.group_count	= ARRAY_SIZE(alderlake_pch_h_community_0_groups),
	.groups		= alderlake_pch_h_community_0_groups,
};

const struct gpio_group *const alderlake_pch_h_community_1_groups[] = {
	&alderlake_pch_h_group_b,
	&alderlake_pch_h_group_g,
	&alderlake_pch_h_group_h,
};
const struct gpio_community alderlake_pch_h_community_1 = {
	.name		= "------- GPIO Community 1 -------",
	.pcr_port_id	= 0x6d,
	.group_count	= ARRAY_SIZE(alderlake_pch_h_community_1_groups),
	.groups		= alderlake_pch_h_community_1_groups,
};

const struct gpio_group *const alderlake_pch_h_community_2_groups[] = {
	&alderlake_pch_h_group_gpd,
};

const struct gpio_community alderlake_pch_h_community_2 = {
	.name		= "------- GPIO Community 2 -------",
	.pcr_port_id	= 0x6c,
	.group_count	= ARRAY_SIZE(alderlake_pch_h_community_2_groups),
	.groups		= alderlake_pch_h_community_2_groups,
};

const struct gpio_group *const alderlake_pch_h_community_3_groups[] = {
	&alderlake_pch_h_group_spi,
	&alderlake_pch_h_group_a,
	&alderlake_pch_h_group_c,
	&alderlake_pch_h_group_vgpio_3,
};

const struct gpio_community alderlake_pch_h_community_3 = {
	.name		= "------- GPIO Community 3 -------",
	.pcr_port_id	= 0x6b,
	.group_count	= ARRAY_SIZE(alderlake_pch_h_community_3_groups),
	.groups		= alderlake_pch_h_community_3_groups,
};

const struct gpio_group *const alderlake_pch_h_community_4_groups[] = {
	&alderlake_pch_h_group_s,
	&alderlake_pch_h_group_e,
	&alderlake_pch_h_group_k,
	&alderlake_pch_h_group_f,
};

const struct gpio_community alderlake_pch_h_community_4 = {
	.name		= "------- GPIO Community 4 -------",
	.pcr_port_id	= 0x6a,
	.group_count	= ARRAY_SIZE(alderlake_pch_h_community_4_groups),
	.groups		= alderlake_pch_h_community_4_groups,
};

const struct gpio_group *const alderlake_pch_h_community_5_groups[] = {
	&alderlake_pch_h_group_d,
	&alderlake_pch_h_group_jtag,
	&alderlake_pch_h_group_cpu,
};

const struct gpio_community alderlake_pch_h_community_5 = {
	.name		= "------- GPIO Community 5 -------",
	.pcr_port_id	= 0x69,
	.group_count	= ARRAY_SIZE(alderlake_pch_h_community_5_groups),
	.groups		= alderlake_pch_h_community_5_groups,
};

const struct gpio_community *const alderlake_pch_h_communities[] = {
	&alderlake_pch_h_community_0,
	&alderlake_pch_h_community_1,
	&alderlake_pch_h_community_2,
	&alderlake_pch_h_community_3,
	&alderlake_pch_h_community_4,
	&alderlake_pch_h_community_5,
};

#endif
