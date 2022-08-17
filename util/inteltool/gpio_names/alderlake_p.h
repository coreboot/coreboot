#ifndef GPIO_NAMES_ALDERLAKE_P
#define GPIO_NAMES_ALDERLAKE_P

#include "gpio_groups.h"

/* ----------------------------- Alder Lake P ----------------------------- */

const char *const alderlake_pch_p_group_a_names[] = {
	"GPP_A0",		"ESPI_IO0",		"n/a",				"n/a",	"n/a",		"n/a",	"USB_C_GPP_A0",
	"GPP_A1",		"ESPI_IO1",		"n/a",				"n/a",	"n/a",		"n/a",	"USB_C_GPP_A1",
	"GPP_A2",		"ESPI_IO2",		"SUSWARN# / SUSPWRDNACK",	"n/a",	"n/a",		"n/a",	"USB_C_GPP_A2",
	"GPP_A3",		"ESPI_IO3",		"SUSACK#",			"n/a",	"n/a",		"n/a",	"USB_C_GPP_A3",
	"GPP_A4",		"ESPI_CS0#",		"n/a",				"n/a",	"n/a",		"n/a",	"USB_C_GPP_A4",
	"GPP_A5",		"ESPI_ALERT0#",		"n/a",				"n/a",	"n/a",		"n/a",	"USB_C_GPP_A5",
	"GPP_A6",		"ESPI_ALERT1#",		"n/a",				"n/a",	"n/a",		"n/a",	"USB_C_GPP_A6",
	"GPP_A7",		"SRCCLK_OE7#",		"n/a",				"n/a",	"n/a",		"n/a",	"USB_C_GPP_A7",
	"GPP_A8",		"SRCCLKREQ7#",		"n/a",				"n/a",	"n/a",		"n/a",	"USB_C_GPP_A8",
	"GPP_A9",		"ESPI_CLK",		"n/a",				"n/a",	"n/a",		"n/a",	"USB_C_GPP_A9",
	"GPP_A10",		"ESPI_RESET#",		"n/a",				"n/a",	"n/a",		"n/a",	"USB_C_GPP_A10",
	"GPP_A11",		"n/a",			"n/a",				"n/a",	"n/a",		"n/a",	"USB_C_GPP_A11",
	"GPP_A12",		"SATAXPCIE1",		"SATAGP1",			"n/a",	"SRCCLKREQ9B#",	"n/a",	"USB_C_GPP_A12",
	"GPP_A13",		"n/a",			"n/a",				"n/a",	"n/a",		"n/a",	"USB_C_GPP_A13",
	"GPP_A14",		"USB_OC1#",		"DDSP_HPD3",			"n/a",	"DISP_MISC3",	"n/a",	"USB_C_GPP_A14",
	"GPP_A15",		"USB_OC2#",		"DDSP_HPD4",			"n/a",	"DISP_MISC4",	"n/a",	"USB_C_GPP_A15",
	"GPP_A16",		"USB_OC3#",		"n/a",				"n/a",	"ISH_GP5",	"n/a",	"USB_C_GPP_A16",
	"GPP_A17",		"n/a",			"n/a",				"n/a",	"DISP_MISCC",	"n/a",	"USB_C_GPP_A17",
	"GPP_A18",		"DDSP_HPDB",		"n/a",				"n/a",	"DISP_MISCB",	"n/a",	"USB_C_GPP_A18",
	"GPP_A19",		"DDSP_HPD1",		"n/a",				"n/a",	"DISP_MISC1",	"n/a",	"USB_C_GPP_A19",
	"GPP_A20",		"DDSP_HPD2",		"n/a",				"n/a",	"DISP_MISC2",	"n/a",	"USB_C_GPP_A20",
	"GPP_A21",		"DDPC_CTRLCLK",		"n/a",				"n/a",	"n/a",		"n/a",	"USB_C_GPP_A21",
	"GPP_A22",		"DDPC_CTRLDATA",	"n/a",				"n/a",	"n/a",		"n/a",	"USB_C_GPP_A22",
	"GPP_A23",		"ESPI_CS1#",		"n/a",				"n/a",	"n/a",		"n/a",	"USB_C_GPP_A23",
	"GPP_ESPI_CLK_LOOPBK",	"GPP_ESPI_CLK_LOOPBK",	"n/a",				"n/a",	"n/a",		"n/a",	"n/a",
};

const struct gpio_group alderlake_pch_p_group_a = {
	.display	= "------- GPIO Group GPP_A -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_p_group_a_names) / 7,
	.func_count	= 7,
	.pad_names	= alderlake_pch_p_group_a_names,
};

const char *const alderlake_pch_p_group_b_names[] = {
	"GPP_B0",	"CORE_VID0",		"n/a",		"n/a",	"n/a",		"n/a",		"USB_C_GPP_B0",
	"GPP_B1",	"CORE_VID1",		"n/a",		"n/a",	"n/a",		"n/a",		"USB_C_GPP_B1",
	"GPP_B2",	"VRALERT#",		"n/a",		"n/a",	"n/a",		"n/a",		"USB_C_GPP_B2",
	"GPP_B3",	"PROC_GP2",		"n/a",		"n/a",	"ISH_GP4B",	"n/a",		"USB_C_GPP_B3",
	"GPP_B4",	"PROC_GP3",		"n/a",		"n/a",	"ISH_GP5B",	"n/a",		"USB_C_GPP_B4",
	"GPP_B5",	"ISH_I2C0_SDA",		"I2C2_SDA",	"n/a",	"n/a",		"n/a",		"USB_C_GPP_B5",
	"GPP_B6",	"ISH_I2C0_SCL",		"I2C2_SCL",	"n/a",	"n/a",		"n/a",		"USB_C_GPP_B6",
	"GPP_B7",	"ISH_I2C1_SDA",		"I2C3_SDA",	"n/a",	"n/a",		"n/a",		"USB_C_GPP_B7",
	"GPP_B8",	"ISH_I2C1_SCL",		"I2C3_SCL",	"n/a",	"n/a",		"n/a",		"USB_C_GPP_B8",
	"GPP_B9",	"n/a",			"n/a",		"n/a",	"n/a",		"n/a",		"n/a",
	"GPP_B10",	"n/a",			"n/a",		"n/a",	"n/a",		"n/a",		"n/a",
	"GPP_B11",	"PMCALERT#",		"n/a",		"n/a",	"n/a",		"n/a",		"USB_C_GPP_B11",
	"GPP_B12",	"SLP_S0#",		"n/a",		"n/a",	"n/a",		"n/a",		"USB_C_GPP_B12",
	"GPP_B13",	"PLTRST#",		"n/a",		"n/a",	"n/a",		"n/a",		"USB_C_GPP_B13",
	"GPP_B14",	"SPKR",			"TIME_SYNC1",	"n/a",	"SATA_LED#",	"ISH_GP6",	"USB_C_GPP_B14",
	"GPP_B15",	"n/a",			"TIME_SYNC0",	"n/a",	"n/a",		"ISH_GP7",	"USB_C_GPP_B15",
	"GPP_B16",	"n/a",			"I2C5_SDA",	"n/a",	"ISH_I2C2_SDA",	"n/a",		"USB_C_GPP_B16",
	"GPP_B17",	"n/a",			"I2C5_SCL",	"n/a",	"ISH_I2C2_SCL",	"n/a",		"USB_C_GPP_B17",
	"GPP_B18",	"n/a",			"n/a",		"n/a",	"n/a",		"n/a",		"USB_C_GPP_B18",
	"GPP_B19",	"n/a",			"n/a",		"n/a",	"n/a",		"n/a",		"n/a",
	"GPP_B20",	"n/a",			"n/a",		"n/a",	"n/a",		"n/a",		"n/a",
	"GPP_B21",	"n/a",			"n/a",		"n/a",	"n/a",		"n/a",		"n/a",
	"GPP_B22",	"n/a",			"n/a",		"n/a",	"n/a",		"n/a",		"n/a",
	"GPP_B23",	"SML1ALERT#",		"PCHHOT#",	"n/a",	"n/a",		"n/a",		"USB_C_GPP_B23",
	"GPP_B24",	"GSPI0_CLK_LOOPBK",	"n/a",		"n/a",	"n/a",		"n/a",		"n/a",
	"GPP_B25",	"GSPI1_CLK_LOOPBK",	"n/a",		"n/a",	"n/a",		"n/a",		"n/a",
};

const struct gpio_group alderlake_pch_p_group_b = {
	.display	= "------- GPIO Group GPP_B -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_p_group_b_names) / 7,
	.func_count	= 7,
	.pad_names	= alderlake_pch_p_group_b_names,
};

const char *const alderlake_pch_p_group_c_names[] = {
	"GPP_C0",	"SMBCLK",		"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_C0",
	"GPP_C1",	"SMBDATA",		"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_C1",
	"GPP_C2",	"SMBALERT#",		"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_C2",
	"GPP_C3",	"SML0CLK",		"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_C3",
	"GPP_C4",	"SML0DATA",		"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_C4",
	"GPP_C5",	"SML0ALERT#",		"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_C5",
	"GPP_C6",	"SML1CLK",		"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_C6",
	"GPP_C7",	"SML1DATA",		"n/a",	"n/a",	"n/a",	"n/a",	"USB_C_GPP_C7",
	"GPP_C8",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C9",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C10",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C11",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C12",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C13",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C14",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C15",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C16",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C17",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C18",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C19",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C20",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C21",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C22",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C23",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
};

const struct gpio_group alderlake_pch_p_group_c = {
	.display	= "------- GPIO Group GPP_C -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_p_group_c_names) / 7,
	.func_count	= 7,
	.pad_names	= alderlake_pch_p_group_c_names,
};

const char *const alderlake_pch_p_group_d_names[] = {
	"GPP_D0",		"ISH_GP0",		"BK0",			"n/a",		"n/a",		"SBK0",		"USB_C_GPP_D0",		"n/a",
	"GPP_D1",		"ISH_GP1",		"BK1",			"n/a",		"n/a",		"SBK1",		"USB_C_GPP_D1",		"n/a",
	"GPP_D2",		"ISH_GP2",		"BK2",			"n/a",		"n/a",		"SBK2",		"USB_C_GPP_D2",		"n/a",
	"GPP_D3",		"ISH_GP3",		"BK3",			"n/a",		"n/a",		"SBK3",		"USB_C_GPP_D3",		"n/a",
	"GPP_D4",		"IMGCLKOUT0",		"BK4",			"n/a",		"n/a",		"SBK4",		"USB_C_GPP_D4",		"n/a",
	"GPP_D5",		"SRCCLKREQ0#",		"n/a",			"n/a",		"n/a",		"n/a",		"USB_C_GPP_D5",		"n/a",
	"GPP_D6",		"SRCCLKREQ1#",		"n/a",			"n/a",		"n/a",		"n/a",		"USB_C_GPP_D6",		"n/a",
	"GPP_D7",		"SRCCLKREQ2#",		"n/a",			"n/a",		"n/a",		"n/a",		"USB_C_GPP_D7",		"n/a",
	"GPP_D8",		"SRCCLKREQ3#",		"n/a",			"n/a",		"n/a",		"n/a",		"USB_C_GPP_D8",		"n/a",
	"GPP_D9",		"ISH_SPI_CS#",		"DDP3_CTRLCLK",		"n/a",		"TBT_LSX2_TXD",	"BSSB_LS2_RX",	"USB_C_GPP_D9",		"GSPI2_CS0#",
	"GPP_D10",		"ISH_SPI_CLK",		"DDP3_CTRLDATA",	"n/a",		"TBT_LSX2_RXD",	"BSSB_LS2_TX",	"USB_C_GPP_D10",	"GSPI2_CLK",
	"GPP_D11",		"ISH_SPI_MISO",		"DDP4_CTRLCLK",		"n/a",		"TBT_LSX3_TXD",	"BSSB_LS3_RX",	"USB_C_GPP_D11",	"GSPI2_MISO",
	"GPP_D12",		"ISH_SPI_MOSI",		"DDP4_CTRLDATA",	"n/a",		"TBT_LSX3_RXD",	"BSSB_LS3_TX",	"USB_C_GPP_D12",	"GSPI2_MOSI",
	"GPP_D13",		"ISH_UART0_RXD",	"n/a",			"I2C6_SDA",	"n/a",		"n/a",		"USB_C_GPP_D13",	"n/a",
	"GPP_D14",		"ISH_UART0_TXD",	"n/a",			"I2C6_SCL",	"n/a",		"n/a",		"USB_C_GPP_D14",	"n/a",
	"GPP_D15",		"ISH_UART0_RTS#",	"n/a",			"n/a",		"I2C7B_SDA",	"n/a",		"USB_C_GPP_D15",	"n/a",
	"GPP_D16",		"ISH_UART0_CTS#",	"n/a",			"n/a",		"I2C7B_SCL",	"n/a",		"USB_C_GPP_D16",	"n/a",
	"GPP_D17",		"UART1_RXD",		"ISH_UART1_RXD",	"n/a",		"n/a",		"n/a",		"USB_C_GPP_D17",	"n/a",
	"GPP_D18",		"UART1_TXD",		"ISH_UART1_TXD",	"n/a",		"n/a",		"n/a",		"USB_C_GPP_D18",	"n/a",
	"GPP_D19",		"I2S_MCLK1_OUT",	"n/a",			"n/a",		"n/a",		"n/a",		"USB_C_GPP_D19",	"n/a",
	"GPP_GSPI2_CLK_LOOPBK",	"GPP_GSPI2_CLK_LOOPBK",	"n/a",			"n/a",		"n/a",		"n/a",		"n/a",			"n/a",
};

const struct gpio_group alderlake_pch_p_group_d = {
	.display	= "------- GPIO Group GPP_D -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_p_group_d_names) / 8,
	.func_count	= 8,
	.pad_names	= alderlake_pch_p_group_d_names,
};

const char *const alderlake_pch_p_group_e_names[] = {
	"GPP_E0",		"SATAXPCIE0",		"SATAGP0",		"n/a",	"n/a",		"n/a",		"USB_C_GPP_E0",		"SRCCLKREQ9#",
	"GPP_E1",		"n/a",			"THC0_SPI1_IO2",	"n/a",	"n/a",		"n/a",		"USB_C_GPP_E1",		"n/a",
	"GPP_E2",		"n/a",			"THC0_SPI1_IO3",	"n/a",	"n/a",		"n/a",		"USB_C_GPP_E2",		"n/a",
	"GPP_E3",		"PROC_GP0",		"n/a",			"n/a",	"n/a",		"n/a",		"USB_C_GPP_E3",		"n/a",
	"GPP_E4",		"DEVSLP0",		"n/a",			"n/a",	"n/a",		"n/a",		"USB_C_GPP_E4",		"SRCCLK_OE9#",
	"GPP_E5",		"DEVSLP1",		"n/a",			"n/a",	"n/a",		"n/a",		"USB_C_GPP_E5",		"SRCCLK_OE6#",
	"GPP_E6",		"n/a",			"THC0_SPI1_RST#",	"n/a",	"n/a",		"n/a",		"USB_C_GPP_E6",		"n/a",
	"GPP_E7",		"PROC_GP1",		"n/a",			"n/a",	"n/a",		"n/a",		"USB_C_GPP_E7",		"n/a",
	"GPP_E8",		"n/a",			"n/a",			"n/a",	"n/a",		"n/a",		"USB_C_GPP_E8",		"n/a",
	"GPP_E9",		"USB_OC0#",		"ISH_GP4",		"n/a",	"n/a",		"n/a",		"USB_C_GPP_E9",		"n/a",
	"GPP_E10",		"n/a",			"THC0_SPI1_CS#",	"n/a",	"n/a",		"n/a",		"USB_C_GPP_E10",	"GSPI0_CS0#",
	"GPP_E11",		"n/a",			"THC0_SPI1_CLK",	"n/a",	"n/a",		"n/a",		"USB_C_GPP_E11",	"GSPI0_CLK",
	"GPP_E12",		"n/a",			"THC0_SPI1_IO1",	"n/a",	"n/a",		"I2C0A_SDA",	"USB_C_GPP_E12",	"GSPI0_MISO",
	"GPP_E13",		"n/a",			"THC0_SPI1_IO0",	"n/a",	"n/a",		"I2C0A_SCL",	"USB_C_GPP_E13",	"GSPI0_MOSI",
	"GPP_E14",		"DDSP_HPDA",		"DISP_MISC_A",		"n/a",	"n/a",		"n/a",		"USB_C_GPP_E14",	"n/a",
	"GPP_E15",		"n/a",			"Reserved",		"n/a",	"n/a",		"n/a",		"USB_C_GPP_E15",	"SRCCLK_OE8#",
	"GPP_E16",		"n/a",			"Reserved",		"n/a",	"n/a",		"n/a",		"USB_C_GPP_E16",	"SRCCLKREQ8#",
	"GPP_E17",		"n/a",			"THC0_SPI1_INT#",	"n/a",	"n/a",		"n/a",		"USB_C_GPP_E17",	"n/a",
	"GPP_E18",		"DDP1_CTRLCLK",		"n/a",			"n/a",	"TBT_LSX0_TXD",	"BSSB_LS0_RX",	"USB_C_GPP_E18",	"n/a",
	"GPP_E19",		"DDP1_CTRLDATA",	"n/a",			"n/a",	"TBT_LSX0_RXD",	"BSSB_LS0_TX",	"USB_C_GPP_E19",	"n/a",
	"GPP_E20",		"DDP2_CTRLCLK",		"n/a",			"n/a",	"TBT_LSX1_TXD",	"BSSB_LS1_RX",	"USB_C_GPP_E20",	"n/a",
	"GPP_E21",		"DDP2_CTRLDATA",	"n/a",			"n/a",	"TBT_LSX1_RXD",	"BSSB_LS1_TX",	"USB_C_GPP_E21",	"n/a",
	"GPP_E22",		"DDPA_CTRLCLK",		"DNX_FORCE_RELOAD",	"n/a",	"n/a",		"n/a",		"USB_C_GPP_E22",	"n/a",
	"GPP_E23",		"DDPA_CTRLDATA",	"n/a",			"n/a",	"n/a",		"n/a",		"USB_C_GPP_E23",	"n/a",
	"GPP_E_CLK_LOOPBK",	"GPP_E_CLK_LOOPBK",	"n/a",			"n/a",	"n/a",		"n/a",		"n/a",			"n/a",
};

const struct gpio_group alderlake_pch_p_group_e = {
	.display	= "------- GPIO Group GPP_E -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_p_group_e_names) / 8,
	.func_count	= 8,
	.pad_names	= alderlake_pch_p_group_e_names,
};

const char *const alderlake_pch_p_group_f_names[] = {
	"GPP_F0",		"CNV_BRI_DT",		"UART2_RTS#",	"n/a",			"n/a",		"n/a",		"USB_C_GPP_F0",
	"GPP_F1",		"CNV_BRI_RSP",		"UART2_RXD",	"n/a",			"n/a",		"n/a",		"USB_C_GPP_F1",
	"GPP_F2",		"CNV_RGI_DT",		"UART2_TXD",	"n/a",			"n/a",		"n/a",		"USB_C_GPP_F2",
	"GPP_F3",		"CNV_RGI_RSP",		"UART2_CTS#",	"n/a",			"n/a",		"n/a",		"USB_C_GPP_F3",
	"GPP_F4",		"CNV_RF_RESET#",	"n/a",		"n/a",			"n/a",		"n/a",		"USB_C_GPP_F4",
	"GPP_F5",		"n/a",			"MODEM_CLKREQ",	"CRF_XTAL_CLKREQ",	"n/a",		"n/a",		"USB_C_GPP_F5",
	"GPP_F6",		"CNV_PA_BLANKING",	"n/a",		"n/a",			"n/a",		"n/a",		"USB_C_GPP_F6",
	"GPP_F7",		"n/a",			"n/a",		"n/a",			"n/a",		"n/a",		"USB_C_GPP_F7",
	"GPP_F8",		"n/a",			"n/a",		"n/a",			"n/a",		"n/a",		"n/a",
	"GPP_F9",		"BOOTMPC",		"n/a",		"n/a",			"n/a",		"n/a",		"USB_C_GPP_F9",
	"GPP_F10",		"n/a",			"n/a",		"n/a",			"n/a",		"n/a",		"USB_C_GPP_F10",
	"GPP_F11",		"n/a",			"n/a",		"THC1_SPI2_CLK",	"GSPI1_CLK",	"n/a",		"USB_C_GPP_F11",
	"GPP_F12",		"GSXDOUT",		"n/a",		"THC1_SPI2_IO0",	"GSPI1_MOSI",	"I2C1A_SCL",	"USB_C_GPP_F12",
	"GPP_F13",		"GSXSLOAD"	,	"n/a",		"THC1_SPI2_IO1",	"GSPI1_MISO",	"I2C1A_SDA",	"USB_C_GPP_F13",
	"GPP_F14",		"GSXDIN",		"n/a",		"THC1_SPI2_IO2",	"n/a",		"n/a",		"USB_C_GPP_F14",
	"GPP_F15",		"GSXSRESET#",		"n/a",		"THC1_SPI2_IO3",	"n/a",		"n/a",		"USB_C_GPP_F15",
	"GPP_F16",		"GSXCLK",		"n/a",		"THC1_SPI2_CS#",	"GSPI_CS0#",	"n/a",		"USB_C_GPP_F16",
	"GPP_F17",		"n/a",			"n/a",		"THC1_SPI2_RST#",	"n/a",		"n/a",		"USB_C_GPP_F17",
	"GPP_F18",		"n/a",			"n/a",		"THC1_SPI2_INT#",	"n/a",		"n/a",		"USB_C_GPP_F18",
	"GPP_F19",		"SRCCLKREQ6#",		"n/a",		"n/a",			"n/a",		"n/a",		"USB_C_GPP_F19",
	"GPP_F20",		"Reserved",		"n/a",		"n/a",			"n/a",		"n/a",		"USB_C_GPP_F20",
	"GPP_F21",		"Reserved",		"n/a",		"n/a",			"n/a",		"n/a",		"USB_C_GPP_F21",
	"GPP_F22",		"VNN_CTRL",		"n/a",		"n/a",			"n/a",		"n/a",		"USB_C_GPP_F22",
	"GPP_F23",		"V1P05_CTRL",		"n/a",		"n/a",			"n/a",		"n/a",		"USB_C_GPP_F23",
	"GPP_F_CLK_LOOPBK",	"GPP_F_CLK_LOOPBK",	"n/a",		"n/a",			"n/a",		"n/a",		"n/a",
};

const struct gpio_group alderlake_pch_p_group_f = {
	.display	= "------- GPIO Group GPP_F -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_p_group_f_names) / 7,
	.func_count	= 7,
	.pad_names	= alderlake_pch_p_group_f_names,
};

const char *const alderlake_pch_p_group_h_names[] = {
	"GPP_H0",	"n/a",			"n/a",			"n/a",			"n/a",		"n/a",		"USB_C_GPP_H0",
	"GPP_H1",	"n/a",			"n/a",			"n/a",			"n/a",		"n/a",		"USB_C_GPP_H1",
	"GPP_H2",	"n/a",			"n/a",			"n/a",			"n/a",		"n/a",		"USB_C_GPP_H2",
	"GPP_H3",	"SX_EXIT_HOLDOFF#",	"n/a",			"n/a",			"n/a",		"n/a",		"USB_C_GPP_H3",
	"GPP_H4",	"I2C0_SDA",		"n/a",			"n/a",			"n/a",		"n/a",		"USB_C_GPP_H4",
	"GPP_H5",	"I2C0_SCL",		"n/a",			"n/a",			"n/a",		"n/a",		"USB_C_GPP_H5",
	"GPP_H6",	"I2C1_SDA",		"n/a",			"n/a",			"n/a",		"n/a",		"USB_C_GPP_H6",
	"GPP_H7",	"I2C1_SCL",		"n/a",			"n/a",			"n/a",		"n/a",		"USB_C_GPP_H7",
	"GPP_H8",	"I2C4_SDA",		"CNV_MFUART2_RXD",	"n/a",			"n/a",		"n/a",		"USB_C_GPP_H8",
	"GPP_H9",	"I2C4_SCL",		"CNV_MFUART2_TXD",	"n/a",			"n/a",		"n/a",		"USB_C_GPP_H9",
	"GPP_H10",	"n/a",			"UART0_RXD",		"M2_SKT2_CFG0",		"n/a",		"n/a",		"USB_C_GPP_H10",
	"GPP_H11",	"n/a",			"UART0_TXD",		"M2_SKT2_CFG1",		"n/a",		"n/a",		"USB_C_GPP_H11",
	"GPP_H12",	"I2C7_SDA",		"UART0_RTS#",		"M2_SKT2_CFG2",		"ISH_GP6B",	"DEVSLP0B",	"USB_C_GPP_H12",
	"GPP_H13",	"I2C7_SCL",		"UART0_CTS#",		"M2_SKT2_CFG3",		"ISH_GP7B",	"DEVSLP1B",	"USB_C_GPP_H13",
	"GPP_H14",	"n/a",			"n/a",			"n/a",			"n/a",		"n/a",		"n/a",
	"GPP_H15",	"DDPB_CTRLCLK",		"n/a",			"PCIE_LINK_DOWN",	"n/a",		"n/a",		"USB_C_GPP_H15",
	"GPP_H16",	"n/a",			"n/a",			"n/a",			"n/a",		"n/a",		"n/a",
	"GPP_H17",	"DDPB_CTRLDATA",	"n/a",			"n/a",			"n/a",		"n/a",		"USB_C_GPP_H17",
	"GPP_H18",	"PROC_C10_GATE#",	"n/a",			"n/a",			"n/a",		"n/a",		"USB_C_GPP_H18",
	"GPP_H19",	"SRCCLKREQ4#",		"n/a",			"n/a",			"n/a",		"n/a",		"USB_C_GPP_H19",
	"GPP_H20",	"IMGCLKOUT1",		"n/a",			"n/a",			"n/a",		"n/a",		"USB_C_GPP_H20",
	"GPP_H21",	"IMGCLKOUT2",		"n/a",			"n/a",			"n/a",		"n/a",		"USB_C_GPP_H21",
	"GPP_H22",	"IMGCLKOUT3",		"n/a",			"n/a",			"n/a",		"n/a",		"USB_C_GPP_H22",
	"GPP_H23",	"n/a",			"SRCCLKREQ5#",		"n/a",			"n/a",		"n/a",		"USB_C_GPP_H23",
};

const struct gpio_group alderlake_pch_p_group_h = {
	.display	= "------- GPIO Group GPP_H -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_p_group_h_names) / 7,
	.func_count	= 7,
	.pad_names	= alderlake_pch_p_group_h_names,
};

const char *const alderlake_pch_p_group_r_names[] = {
	"GPP_R0",	"HDA_BCLK",	"I2S0_SCLK",	"DMIC_CLK_B0",	"HDAPROC_BCLK",
	"GPP_R1",	"HDA_SYNC",	"I2S0_SFRM",	"DMIC_CLK_B1",	"n/a",
	"GPP_R2",	"HDA_SDO",	"I2S0_TXD",	"n/a",		"HDAPROC_SDO",
	"GPP_R3",	"HDA_SDI0",	"I2S0_RXD",	"n/a",		"HDAPROC_SDI",
	"GPP_R4",	"HDA_RST#",	"I2S2_SCLK",	"DMIC_CLK_A0",	"n/a",
	"GPP_R5",	"HDA_SDI1",	"I2S2_SFRM",	"DMIC_DATA0",	"n/a",
	"GPP_R6",	"n/a",		"I2S2_TXD",	"DMIC_CLK_A1",	"n/a",
	"GPP_R7",	"n/a",		"I2S2_RXD",	"DMIC_DATA1",	"n/a",
};

const struct gpio_group alderlake_pch_p_group_r = {
	.display	= "------- GPIO Group GPP_R -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_p_group_r_names) / 5,
	.func_count	= 5,
	.pad_names	= alderlake_pch_p_group_r_names,
};

const char *const alderlake_pch_p_group_s_names[] = {
	"GPP_S0",	"SNDW0_CLK",	"n/a",		"n/a",	"I2S1_SCLK",
	"GPP_S1",	"SNDW0_DATA",	"n/a",		"n/a",	"I2S1_SFRM",
	"GPP_S2",	"SNDW1_CLK",	"DMIC_CKL_A0",	"n/a",	"I2S1_TXD",
	"GPP_S3",	"SNDW1_DATA",	"DMIC_DATA0",	"n/a",	"I2S1_RXD",
	"GPP_S4",	"SNDW2_CLK",	"DMIC_CLK_B0",	"n/a",	"n/a",
	"GPP_S5",	"SNDW2_DATA",	"DMIC_CLK_B1",	"n/a",	"n/a",
	"GPP_S6",	"SNDW3_CLK",	"DMIC_CLK_A1",	"n/a",	"n/a",
	"GPP_S7",	"SNDW3_DATA",	"DMIC_DATA1",	"n/a",	"n/a",
};

const struct gpio_group alderlake_pch_p_group_s = {
	.display	= "------- GPIO Group GPP_S -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_p_group_s_names) / 5,
	.func_count	= 5,
	.pad_names	= alderlake_pch_p_group_s_names,
};

const char *const alderlake_pch_p_group_t_names[] = {
	"GPP_T0",	"n/a",	"n/a",
	"GPP_T1",	"n/a",	"n/a",
	"GPP_T2",	"n/a",	"Reserved",
	"GPP_T3",	"n/a",	"Reserved",
	"GPP_T4",	"n/a",	"n/a",
	"GPP_T5",	"n/a",	"n/a",
	"GPP_T6",	"n/a",	"n/a",
	"GPP_T7",	"n/a",	"n/a",
	"GPP_T8",	"n/a",	"n/a",
	"GPP_T9",	"n/a",	"n/a",
	"GPP_T10",	"n/a",	"n/a",
	"GPP_T11",	"n/a",	"n/a",
	"GPP_T12",	"n/a",	"n/a",
	"GPP_T13",	"n/a",	"n/a",
	"GPP_T14",	"n/a",	"n/a",
	"GPP_T15",	"n/a",	"n/a",
	};


const struct gpio_group alderlake_pch_p_group_t = {
	.display	= "------- GPIO Group GPP_T -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_p_group_t_names) / 3,
	.func_count	= 3,
	.pad_names	= alderlake_pch_p_group_t_names,
};

const char *const alderlake_pch_p_group_gpd_names[] = {
	"GPD0",			"BATLOW#",
	"GPD1",			"ACPRESENT",
	"GPD2",			"LAN_WAKE#",
	"GPD3",			"PWRBTN#",
	"GPD4",			"SLP_S3#",
	"GPD5",			"SLP_S4#",
	"GPD6",			"SLP_A#",
	"GPD7",			 "n/a",
	"GPD8",			"SUSCLK",
	"GPD9",			"SLP_WLAN#",
	"GPD10",		"SLP_S5#",
	"GPD11",		"LANPHYPC",
	"GPD_INPUT3VSEL",	"GPD_INPUT3VSEL",
	"GPD_SLP_LANB",		"GPD_SLP_LANB",
	"GPD_SLP_SUSB",		"GPD_SLP_SUSB",
	"GPD_WAKEB",		"GPD_WAKEB",
	"GPD_DRAM_RESETB",	"GPD_DRAM_RESETB",
};

const struct gpio_group alderlake_pch_p_group_gpd = {
	.display	= "------- GPIO Group GPP_GPD -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_p_group_gpd_names) / 2,
	.func_count	= 2,
	.pad_names	= alderlake_pch_p_group_gpd_names,
};

const char *const alderlake_pch_p_group_hvcmos_names[] = {
	"GPP_L_BKLTEN",		"n/a",
	"GPP_L_BKLTCTL",	"n/a",
	"GPP_L_VDDEN",		"n/a",
	"GPP_SYS_PWROK",	"n/a",
	"GPP_SYS_RESETB",	"n/a",
	"GPP_MLK_RSTB",		"n/a",
};

const struct gpio_group alderlake_pch_p_group_hvcmos = {
	.display	= "------- GPIO Group GPP_HVCMOS -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_p_group_hvcmos_names) / 2,
	.func_count	= 2,
	.pad_names	= alderlake_pch_p_group_hvcmos_names,
};

const char *const alderlake_pch_p_group_reserved_names[] = {
	"GPP_CPU_RSVD_1",	"GPP_CPU_RSVD_1",
	"GPP_CPU_RSVD_2",	"GPP_CPU_RSVD_2",
	"GPP_CPU_RSVD_3",	"GPP_CPU_RSVD_3",
	"GPP_CPU_RSVD_4",	"GPP_CPU_RSVD_4",
	"GPP_CPU_RSVD_5",	"GPP_CPU_RSVD_5",
	"GPP_CPU_RSVD_6",	"GPP_CPU_RSVD_6",
	"GPP_CPU_RSVD_7",	"GPP_CPU_RSVD_7",
	"GPP_CPU_RSVD_8",	"GPP_CPU_RSVD_8",
	"GPP_CPU_RSVD_9",	"GPP_CPU_RSVD_9",
	"GPP_CPU_RSVD_10",	"GPP_CPU_RSVD_10",
	"GPP_CPU_RSVD_11",	"GPP_CPU_RSVD_11",
	"GPP_CPU_RSVD_12",	"GPP_CPU_RSVD_12",
	"GPP_CPU_RSVD_13",	"GPP_CPU_RSVD_13",
	"GPP_CPU_RSVD_14",	"GPP_CPU_RSVD_14",
	"GPP_CPU_RSVD_15",	"GPP_CPU_RSVD_15",
	"GPP_CPU_RSVD_16",	"GPP_CPU_RSVD_16",
	"GPP_CPU_RSVD_17",	"GPP_CPU_RSVD_17",
	"GPP_CPU_RSVD_18",	"GPP_CPU_RSVD_18",
	"GPP_CPU_RSVD_19",	"GPP_CPU_RSVD_19",
	"GPP_CPU_RSVD_20",	"GPP_CPU_RSVD_20",
	"GPP_CPU_RSVD_21",	"GPP_CPU_RSVD_21",
	"GPP_CPU_RSVD_22",	"GPP_CPU_RSVD_22",
	"GPP_CPU_RSVD_23",	"GPP_CPU_RSVD_23",
	"GPP_CPU_RSVD_24",	"GPP_CPU_RSVD_24",
};

const struct gpio_group alderlake_pch_p_group_reserved = {
	.display	= "------- GPIO Group RESERVED -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_p_group_reserved_names) / 2,
	.func_count	= 2,
	.pad_names	= alderlake_pch_p_group_reserved_names,
};

const char *const alderlake_pch_p_group_vgpio_names[] = {
	"GPP_VGPIO_0",	"GPP_VGPIO_0",
	"GPP_VGPIO_4",	"GPP_VGPIO_4",
	"GPP_VGPIO_5",	"GPP_VGPIO_5",
	"GPP_VGPIO_6",	"GPP_VGPIO_6",
	"GPP_VGPIO_7",	"GPP_VGPIO_7",
	"GPP_VGPIO_8",	"GPP_VGPIO_8",
	"GPP_VGPIO_9",	"GPP_VGPIO_9",
	"GPP_VGPIO_10",	"GPP_VGPIO_10",
	"GPP_VGPIO_11",	"GPP_VGPIO_11",
	"GPP_VGPIO_12",	"GPP_VGPIO_12",
	"GPP_VGPIO_13",	"GPP_VGPIO_13",
	"GPP_VGPIO_18",	"GPP_VGPIO_18",
	"GPP_VGPIO_19",	"GPP_VGPIO_19",
	"GPP_VGPIO_20",	"GPP_VGPIO_20",
	"GPP_VGPIO_21",	"GPP_VGPIO_21",
	"GPP_VGPIO_22",	"GPP_VGPIO_22",
	"GPP_VGPIO_23",	"GPP_VGPIO_23",
	"GPP_VGPIO_24",	"GPP_VGPIO_24",
	"GPP_VGPIO_25",	"GPP_VGPIO_25",
	"GPP_VGPIO_30",	"GPP_VGPIO_30",
	"GPP_VGPIO_31",	"GPP_VGPIO_31",
	"GPP_VGPIO_32",	"GPP_VGPIO_32",
	"GPP_VGPIO_33",	"GPP_VGPIO_33",
	"GPP_VGPIO_34",	"GPP_VGPIO_34",
	"GPP_VGPIO_35",	"GPP_VGPIO_35",
	"GPP_VGPIO_36",	"GPP_VGPIO_36",
	"GPP_VGPIO_37",	"GPP_VGPIO_37",
};

const struct gpio_group alderlake_pch_p_group_vgpio = {
	.display	= "------- GPIO Group vGPIO -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_p_group_vgpio_names) / 2,
	.func_count	= 2,
	.pad_names	= alderlake_pch_p_group_vgpio_names,
};


const char *const alderlake_pch_p_group_pcie_vgpio_names[] = {
	"GPP_CPU_RSVD_25",	"GPP_CPU_RSVD_25",
	"GPP_CPU_RSVD_26",	"GPP_CPU_RSVD_26",
	"GPP_CPU_RSVD_27",	"GPP_CPU_RSVD_27",
	"GPP_CPU_RSVD_28",	"GPP_CPU_RSVD_28",
	"GPP_CPU_RSVD_29",	"GPP_CPU_RSVD_29",
	"GPP_CPU_RSVD_30",	"GPP_CPU_RSVD_30",
	"GPP_CPU_RSVD_31",	"GPP_CPU_RSVD_31",
	"GPP_CPU_RSVD_32",	"GPP_CPU_RSVD_32",
	"GPP_CPU_RSVD_33",	"GPP_CPU_RSVD_33",
	"GPP_CPU_RSVD_34",	"GPP_CPU_RSVD_34",
	"GPP_CPU_RSVD_35",	"GPP_CPU_RSVD_35",
	"GPP_CPU_RSVD_36",	"GPP_CPU_RSVD_36",
	"GPP_CPU_RSVD_37",	"GPP_CPU_RSVD_37",
	"GPP_CPU_RSVD_38",	"GPP_CPU_RSVD_38",
	"GPP_CPU_RSVD_39",	"GPP_CPU_RSVD_39",
	"GPP_vGPIO_PCIE_0",	"GPP_vGPIO_PCIE_0",
	"GPP_vGPIO_PCIE_1",	"GPP_vGPIO_PCIE_1",
	"GPP_vGPIO_PCIE_2",	"GPP_vGPIO_PCIE_2",
	"GPP_vGPIO_PCIE_3",	"GPP_vGPIO_PCIE_3",
	"GPP_vGPIO_PCIE_4",	"GPP_vGPIO_PCIE_4",
	"GPP_vGPIO_PCIE_5",	"GPP_vGPIO_PCIE_5",
	"GPP_vGPIO_PCIE_6",	"GPP_vGPIO_PCIE_6",
	"GPP_vGPIO_PCIE_7",	"GPP_vGPIO_PCIE_7",
	"GPP_vGPIO_PCIE_8",	"GPP_vGPIO_PCIE_8",
	"GPP_vGPIO_PCIE_9",	"GPP_vGPIO_PCIE_9",
	"GPP_vGPIO_PCIE_10",	"GPP_vGPIO_PCIE_10",
	"GPP_vGPIO_PCIE_11",	"GPP_vGPIO_PCIE_11",
	"GPP_vGPIO_PCIE_12",	"GPP_vGPIO_PCIE_12",
	"GPP_vGPIO_PCIE_13",	"GPP_vGPIO_PCIE_13",
	"GPP_vGPIO_PCIE_14",	"GPP_vGPIO_PCIE_14",
	"GPP_vGPIO_PCIE_15",	"GPP_vGPIO_PCIE_15",
	"GPP_vGPIO_PCIE_16",	"GPP_vGPIO_PCIE_16",
	"GPP_vGPIO_PCIE_17",	"GPP_vGPIO_PCIE_17",
	"GPP_vGPIO_PCIE_18",	"GPP_vGPIO_PCIE_18",
	"GPP_vGPIO_PCIE_19",	"GPP_vGPIO_PCIE_19",
	"GPP_vGPIO_PCIE_20",	"GPP_vGPIO_PCIE_20",
	"GPP_vGPIO_PCIE_21",	"GPP_vGPIO_PCIE_21",
	"GPP_vGPIO_PCIE_22",	"GPP_vGPIO_PCIE_22",
	"GPP_vGPIO_PCIE_23",	"GPP_vGPIO_PCIE_23",
	"GPP_vGPIO_PCIE_24",	"GPP_vGPIO_PCIE_24",
	"GPP_vGPIO_PCIE_25",	"GPP_vGPIO_PCIE_25",
	"GPP_vGPIO_PCIE_26",	"GPP_vGPIO_PCIE_26",
	"GPP_vGPIO_PCIE_27",	"GPP_vGPIO_PCIE_27",
	"GPP_vGPIO_PCIE_28",	"GPP_vGPIO_PCIE_28",
	"GPP_vGPIO_PCIE_29",	"GPP_vGPIO_PCIE_29",
	"GPP_vGPIO_PCIE_30",	"GPP_vGPIO_PCIE_30",
	"GPP_vGPIO_PCIE_31",	"GPP_vGPIO_PCIE_31",
	"GPP_vGPIO_PCIE_32",	"GPP_vGPIO_PCIE_32",
	"GPP_vGPIO_PCIE_33",	"GPP_vGPIO_PCIE_33",
	"GPP_vGPIO_PCIE_34",	"GPP_vGPIO_PCIE_34",
	"GPP_vGPIO_PCIE_35",	"GPP_vGPIO_PCIE_35",
	"GPP_vGPIO_PCIE_36",	"GPP_vGPIO_PCIE_36",
	"GPP_vGPIO_PCIE_37",	"GPP_vGPIO_PCIE_37",
	"GPP_vGPIO_PCIE_38",	"GPP_vGPIO_PCIE_38",
	"GPP_vGPIO_PCIE_39",	"GPP_vGPIO_PCIE_39",
	"GPP_vGPIO_PCIE_40",	"GPP_vGPIO_PCIE_40",
	"GPP_vGPIO_PCIE_41",	"GPP_vGPIO_PCIE_41",
	"GPP_vGPIO_PCIE_42",	"GPP_vGPIO_PCIE_42",
	"GPP_vGPIO_PCIE_43",	"GPP_vGPIO_PCIE_43",
	"GPP_vGPIO_PCIE_44",	"GPP_vGPIO_PCIE_44",
	"GPP_vGPIO_PCIE_45",	"GPP_vGPIO_PCIE_45",
	"GPP_vGPIO_PCIE_46",	"GPP_vGPIO_PCIE_46",
	"GPP_vGPIO_PCIE_47",	"GPP_vGPIO_PCIE_47",
	"GPP_vGPIO_PCIE_48",	"GPP_vGPIO_PCIE_48",
	"GPP_vGPIO_PCIE_49",	"GPP_vGPIO_PCIE_49",
	"GPP_vGPIO_PCIE_50",	"GPP_vGPIO_PCIE_50",
	"GPP_vGPIO_PCIE_51",	"GPP_vGPIO_PCIE_51",
	"GPP_vGPIO_PCIE_52",	"GPP_vGPIO_PCIE_52",
	"GPP_vGPIO_PCIE_53",	"GPP_vGPIO_PCIE_53",
	"GPP_vGPIO_PCIE_54",	"GPP_vGPIO_PCIE_54",
	"GPP_vGPIO_PCIE_55",	"GPP_vGPIO_PCIE_55",
	"GPP_vGPIO_PCIE_56",	"GPP_vGPIO_PCIE_56",
	"GPP_vGPIO_PCIE_57",	"GPP_vGPIO_PCIE_57",
	"GPP_vGPIO_PCIE_58",	"GPP_vGPIO_PCIE_58",
	"GPP_vGPIO_PCIE_59",	"GPP_vGPIO_PCIE_59",
	"GPP_vGPIO_PCIE_60",	"GPP_vGPIO_PCIE_60",
	"GPP_vGPIO_PCIE_61",	"GPP_vGPIO_PCIE_61",
	"GPP_vGPIO_PCIE_62",	"GPP_vGPIO_PCIE_62",
	"GPP_vGPIO_PCIE_63",	"GPP_vGPIO_PCIE_63",
	"GPP_vGPIO_PCIE_64",	"GPP_vGPIO_PCIE_64",
	"GPP_vGPIO_PCIE_65",	"GPP_vGPIO_PCIE_65",
	"GPP_vGPIO_PCIE_66",	"GPP_vGPIO_PCIE_66",
	"GPP_vGPIO_PCIE_67",	"GPP_vGPIO_PCIE_67",
	"GPP_vGPIO_PCIE_68",	"GPP_vGPIO_PCIE_68",
	"GPP_vGPIO_PCIE_69",	"GPP_vGPIO_PCIE_69",
	"GPP_vGPIO_PCIE_70",	"GPP_vGPIO_PCIE_70",
	"GPP_vGPIO_PCIE_71",	"GPP_vGPIO_PCIE_71",
	"GPP_vGPIO_PCIE_72",	"GPP_vGPIO_PCIE_72",
	"GPP_vGPIO_PCIE_73",	"GPP_vGPIO_PCIE_73",
	"GPP_vGPIO_PCIE_74",	"GPP_vGPIO_PCIE_74",
	"GPP_vGPIO_PCIE_75",	"GPP_vGPIO_PCIE_75",
	"GPP_vGPIO_PCIE_76",	"GPP_vGPIO_PCIE_76",
	"GPP_vGPIO_PCIE_77",	"GPP_vGPIO_PCIE_77",
	"GPP_vGPIO_PCIE_78",	"GPP_vGPIO_PCIE_78",
	"GPP_vGPIO_PCIE_79",	"GPP_vGPIO_PCIE_79",
	"GPP_CPU_RSVD_40",	"GPP_CPU_RSVD_40",
	"GPP_CPU_RSVD_41",	"GPP_CPU_RSVD_41",
	"GPP_CPU_RSVD_42",	"GPP_CPU_RSVD_42",
	"GPP_CPU_RSVD_43",	"GPP_CPU_RSVD_43",
	"GPP_CPU_RSVD_44",	"GPP_CPU_RSVD_44",
	"GPP_CPU_RSVD_45",	"GPP_CPU_RSVD_45",
	"GPP_CPU_RSVD_46",	"GPP_CPU_RSVD_46",
	"GPP_CPU_RSVD_47",	"GPP_CPU_RSVD_47",
	"GPP_vGPIO_PCIE_80",	"GPP_vGPIO_PCIE_80",
	"GPP_vGPIO_PCIE_81",	"GPP_vGPIO_PCIE_81",
	"GPP_vGPIO_PCIE_82",	"GPP_vGPIO_PCIE_82",
	"GPP_vGPIO_PCIE_83",	"GPP_vGPIO_PCIE_83",
};

const struct gpio_group alderlake_pch_p_group_pcie_vgpio = {
	.display	= "------- GPIO Group PCIe vGPIO -------",
	.pad_count	= ARRAY_SIZE(alderlake_pch_p_group_pcie_vgpio_names) / 2,
	.func_count	= 2,
	.pad_names	= alderlake_pch_p_group_pcie_vgpio_names,
};

const struct gpio_group *const alderlake_pch_p_community_0_groups[] = {
	&alderlake_pch_p_group_b,
	&alderlake_pch_p_group_t,
	&alderlake_pch_p_group_a,
};

const struct gpio_community alderlake_pch_p_community_0 = {
	.name		= "------- GPIO Community 0 -------",
	.pcr_port_id	= 0x6e,
	.group_count	= ARRAY_SIZE(alderlake_pch_p_community_0_groups),
	.groups		= alderlake_pch_p_community_0_groups,
};

const struct gpio_group *const alderlake_pch_p_community_1_groups[] = {
	&alderlake_pch_p_group_s,
	&alderlake_pch_p_group_h,
	&alderlake_pch_p_group_d,
	&alderlake_pch_p_group_reserved,
	&alderlake_pch_p_group_vgpio,
};

const struct gpio_community alderlake_pch_p_community_1 = {
	.name		= "------- GPIO Community 1 -------",
	.pcr_port_id	= 0x6d,
	.group_count	= ARRAY_SIZE(alderlake_pch_p_community_1_groups),
	.groups		= alderlake_pch_p_community_1_groups,
};

const struct gpio_group *const alderlake_pch_p_community_2_groups[] = {
	&alderlake_pch_p_group_gpd,
	&alderlake_pch_p_group_pcie_vgpio,
};

const struct gpio_community alderlake_pch_p_community_2 = {
	.name		= "------- GPIO Community 2 -------",
	.pcr_port_id	= 0x6c,
	.group_count	= ARRAY_SIZE(alderlake_pch_p_community_2_groups),
	.groups		= alderlake_pch_p_community_2_groups,
};

const struct gpio_group *const alderlake_pch_p_community_4_groups[] = {
	&alderlake_pch_p_group_c,
	&alderlake_pch_p_group_f,
	&alderlake_pch_p_group_hvcmos,
	&alderlake_pch_p_group_e,
};

const struct gpio_community alderlake_pch_p_community_4 = {
	.name		= "------- GPIO Community 4 -------",
	.pcr_port_id	= 0x6a,
	.group_count	= ARRAY_SIZE(alderlake_pch_p_community_4_groups),
	.groups		= alderlake_pch_p_community_4_groups,
};

const struct gpio_group *const alderlake_pch_p_community_5_groups[] = {
	&alderlake_pch_p_group_r,
};

const struct gpio_community alderlake_pch_p_community_5 = {
	.name		= "------- GPIO Community 5 -------",
	.pcr_port_id	= 0x69,
	.group_count	= ARRAY_SIZE(alderlake_pch_p_community_5_groups),
	.groups		= alderlake_pch_p_community_5_groups,
};

const struct gpio_community *const alderlake_pch_p_communities[] = {
	&alderlake_pch_p_community_0,
	&alderlake_pch_p_community_1,
	&alderlake_pch_p_community_2,
	&alderlake_pch_p_community_4,
	&alderlake_pch_p_community_5,
};

#endif
