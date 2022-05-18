#ifndef GPIO_NAMES_TIGERLAKE_LP
#define GPIO_NAMES_TIGERLAKE_LP

#include "gpio_groups.h"

/* ----------------------------- Tiger Lake LP ----------------------------- */

const char *const tigerlake_pch_lp_group_a_names[] = {
	"GPP_A0",		"ESPI_IO0",		"n/a",			"n/a",			"n/a",		"n/a",
	"GPP_A1",		"ESPI_IO1",		"n/a",			"n/a",			"n/a",		"n/a",
	"GPP_A2",		"ESPI_IO2",		"SUSWARN#/SUSPWRDNACK",	"n/a",			"n/a",		"n/a",
	"GPP_A3",		"ESPI_IO3",		"SUSACK#",		"n/a",			"n/a",		"n/a",
	"GPP_A4",		"ESPI_CS#",		"n/a",			"n/a",			"n/a",		"n/a",
	"GPP_A5",		"ESPI_CLK",		"n/a",			"n/a",			"n/a",		"n/a",
	"GPP_A6",		"ESPI_RESET#",		"n/a",			"n/a",			"n/a",		"n/a",
	"GPP_A7",		"I2S2_SCLK",		"n/a",			"n/a",			"n/a",		"DMIC_CLK_A0",
	"GPP_A8",		"I2S2_SFRM",		"CNV_RF_RESET#",	"n/a",			"n/a",		"DMIC_DATA0",
	"GPP_A9",		"I2S2_TXD",		"MODEM_CLKREQ",		"CRF_XTAL_CLKREQ",	"n/a",		"DMIC_CLK_A1",
	"GPP_A10",		"I2S2_RXD",		"n/a",			"n/a",			"n/a",		"DMIC_DATA1",
	"GPP_A11",		"PMC_I2C_SDA",		"n/a",			"I2S3_SCLK",		"n/a",		"n/a",
	"GPP_A12",		"SATAXPCIE1",		"SATAGP1",		"I2S3_SFRM",		"n/a",		"n/a",
	"GPP_A13",		"PMC_I2C_SCL",		"n/a",			"I2S3_TXD",		"n/a",		"DMIC_CLK_B0",
	"GPP_A14",		"USB_OC1#",		"DDSP_HPD3",		"I2S3_RXD",		"DISP_MISC3",	"DMIC_CLK_B1",
	"GPP_A15",		"USB_OC2#",		"DDSP_HPD4",		"I2S4_SCLK",		"DISP_MISC4",	"n/a",
	"GPP_A16",		"USB_OC3#",		"n/a",			"I2S4_SFRM",		"n/a",		"n/a",
	"GPP_A17",		"DDSP_HDPC",		"DISP_MISCC",		"I2S4_TXD",		"n/a",		"n/a",
	"GPP_A18",		"DDSP_HPDB",		"DISP_MISCB",		"I2S4_RXD",		"n/a",		"n/a",
	"GPP_A19",		"DDSP_HPD1",		"DISP_MISC1",		"I2S5_SCLK",		"n/a",		"n/a",
	"GPP_A20",		"DDSP_HPD2",		"DISP_MISC2",		"I2S5_SFRM",		"n/a",		"n/a",
	"GPP_A21",		"BKLTEN_SEC",		"DDPC_CTRLCLK",		"I2S5_TXD",		"n/a",		"n/a",
	"GPP_A22",		"BKLTCTL_SEC",		"DDPC_CTRLDATA",	"I2S5_RXD",		"n/a",		"n/a",
	"GPP_A23",		"I2S1_SCLK",		"n/a",			"n/a",			"n/a",		"n/a",
	"ESPI_CLK_LOOPBK",	"ESPI_CLK_LOOPBK",	"n/a",			"n/a",			"n/a",		"n/a",
};

const struct gpio_group tigerlake_pch_lp_group_a = {
	.display	= "------- GPIO Group GPP_A -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_lp_group_a_names) / 6,
	.func_count	= 6,
	.pad_names	= tigerlake_pch_lp_group_a_names,
};

const char *const tigerlake_pch_lp_group_b_names[] = {
	"GPP_B0",		"CORE_VID0",		"n/a",		"n/a",
	"GPP_B1",		"CORE_VID1",		"n/a",		"n/a",
	"GPP_B2",		"VRALERT#",		"n/a",		"n/a",
	"GPP_B3",		"CPU_GP2",		"n/a",		"n/a",
	"GPP_B4",		"CPU_GP3",		"n/a",		"n/a",
	"GPP_B5",		"ISH_I2C0_SDA",		"n/a",		"n/a",
	"GPP_B6",		"ISH_I2C0_SCL",		"n/a",		"n/a",
	"GPP_B7",		"ISH_I2C1_SDA",		"n/a",		"n/a",
	"GPP_B8",		"ISH_I2C1_SCL",		"n/a",		"n/a",
	"GPP_B9",		"I2C5_SDA",		"ISH_I2C2_SDA",	"n/a",
	"GPP_B10",		"I2C5_SCL",		"ISH_I2C2_SDL",	"n/a",
	"GPP_B11",		"PMCALERT#",		"n/a",		"n/a",
	"GPP_B12",		"SLP_S0#",		"n/a",		"n/a",
	"GPP_B13",		"PLTRST#",		"n/a",		"n/a",
	"GPP_B14",		"SPKR",			"TIME_SYNC1",	"GSPI0_CS1#",
	"GPP_B15",		"GSPI0_CS0#",		"n/a",		"n/a",
	"GPP_B16",		"GSPI0_CLK",		"n/a",		"n/a",
	"GPP_B17",		"GSPI0_MISO",		"n/a",		"n/a",
	"GPP_B18",		"GSPI0_MOSI",		"n/a",		"n/a",
	"GPP_B19",		"GSPI1_CS0#",		"n/a",		"n/a",
	"GPP_B20",		"GSPI1_CLK",		"n/a",		"n/a",
	"GPP_B21",		"GSPI1_MISO",		"n/a",		"n/a",
	"GPP_B22",		"GSPI1_MOSI",		"n/a",		"n/a",
	"GPP_B23",		"SML1ALERT#",		"PCHHOT#",	"GSPI1_CS1#",
	"GSPI0_CLK_LOOPBK",	"GSPI0_CLK_LOOPBK",	"n/a",		"n/a",
	"GSPI1_CLK_LOOPBK",	"GSPI1_CLK_LOOPBK",	"n/a",		"n/a",
};

const struct gpio_group tigerlake_pch_lp_group_b = {
	.display	= "------- GPIO Group GPP_B -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_lp_group_b_names) / 4,
	.func_count	= 4,
	.pad_names	= tigerlake_pch_lp_group_b_names,
};

const char *const tigerlake_pch_lp_group_c_names[] = {
	"GPP_C0",	"SMBCLK",	"n/a",
	"GPP_C1",	"SMBDATA",	"n/a",
	"GPP_C2",	"SMBALERT#",	"n/a",
	"GPP_C3",	"SML0CLK",	"n/a",
	"GPP_C4",	"SML0DATA",	"n/a",
	"GPP_C5",	"SML0ALERT#",	"n/a",
	"GPP_C6",	"SML1CLK",	"n/a",
	"GPP_C7",	"SML1DATA",	"n/a",
	"GPP_C8",	"UART0_RXD",	"n/a",
	"GPP_C9",	"UART0_TXD",	"n/a",
	"GPP_C10",	"UART0_RTS#",	"n/a",
	"GPP_C11",	"UART0_CTS#",	"n/a",
	"GPP_C12",	"UART1_RXD",	"ISH_UART1_RXD",
	"GPP_C13",	"UART1_TXD",	"ISH_UART1_TXD",
	"GPP_C14",	"UART1_RTS#",	"ISH_UART1_RTS#",
	"GPP_C15",	"UART1_CTS#",	"ISH_UART1_CTS#",
	"GPP_C16",	"I2C0_SDA",	"n/a",
	"GPP_C17",	"I2C0_SCL",	"n/a",
	"GPP_C18",	"I2C1_SDA",	"n/a",
	"GPP_C19",	"I2C1_SCL",	"n/a",
	"GPP_C20",	"UART2_RXD",	"n/a",
	"GPP_C21",	"UART2_TXD",	"n/a",
	"GPP_C22",	"UART2_RTS#",	"n/a",
	"GPP_C23",	"UART2_CTS#",	"n/a",
};

const struct gpio_group tigerlake_pch_lp_group_c = {
	.display	= "------- GPIO Group GPP_C -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_lp_group_c_names) / 3,
	.func_count	= 3,
	.pad_names	= tigerlake_pch_lp_group_c_names,
};

const char *const tigerlake_pch_lp_group_d_names[] = {
	"GPP_D0",		"ISH_GP0",		"BK0",			"n/a",		"n/a",		"SBK0",		"n/a",	"n/a",
	"GPP_D1",		"ISH_GP1",		"BK1",			"n/a",		"n/a",		"SBK1",		"n/a",	"n/a",
	"GPP_D2",		"ISH_GP2",		"BK2",			"n/a",		"n/a",		"SBK2",		"n/a",	"n/a",
	"GPP_D3",		"ISH_GP3",		"BK3",			"n/a",		"n/a",		"SBK3",		"n/a",	"n/a",
	"GPP_D4",		"IMGCLKOUT0",		"BK4",			"n/a",		"n/a",		"SBK4",		"n/a",	"n/a",
	"GPP_D5",		"SRCCLKREQ0#",		"n/a",			"n/a",		"n/a",		"n/a",		"n/a",	"n/a",
	"GPP_D6",		"SRCCLKREQ1#",		"n/a",			"n/a",		"n/a",		"n/a",		"n/a",	"n/a",
	"GPP_D7",		"SRCCLKREQ2#",		"n/a",			"n/a",		"n/a",		"n/a",		"n/a",	"n/a",
	"GPP_D8",		"SRCCLKREQ3#",		"n/a",			"n/a",		"n/a",		"n/a",		"n/a",	"n/a",
	"GPP_D9",		"ISH_SPI_CS#",		"DDP3_CTRLCLK",		"n/a",		"TBT_LSX2_TXD",	"BSSB_LS2_RX",	"n/a",	"GSPI2_CS0#",
	"GPP_D10",		"ISH_SPI_CLK",		"DDP3_CTRLDATA",	"n/a",		"TBT_LSX2_RXD",	"BSSB_LS2_TX",	"n/a",	"GSPI2_CLK",
	"GPP_D11",		"ISH_SPI_MISO",		"DDP4_CTRLCLK",		"n/a",		"TBT_LSX3_TXD",	"BSSB_LS3_RX",	"n/a",	"GSPI2_MISO",
	"GPP_D12",		"ISH_SPI_MOSI",		"DDP4_CTRLDATA",	"n/a",		"TBT_LSX3_RXD",	"BSSB_LS3_TX",	"n/a",	"GSPI2_MOSI",
	"GPP_D13",		"ISH_UART0_RXD",	"n/a",			"I2C4_SDA",	"n/a",		"n/a",		"n/a",	"n/a",
	"GPP_D14",		"ISH_UART0_TXD",	"n/a",			"I2C4_SCL",	"n/a",		"n/a",		"n/a",	"n/a",
	"GPP_D15",		"ISH_UART0_RTS#",	"GSPI2_CS1#",		"IMGCLKOUT5",	"n/a",		"n/a",		"n/a",	"n/a",
	"GPP_D16",		"ISH_UART0_CTS#",	"n/a",			"n/a",		"n/a",		"n/a",		"n/a",	"n/a",
	"GPP_D17",		"ISH_GP4",		"n/a",			"n/a",		"n/a",		"n/a",		"n/a",	"n/a",
	"GPP_D18",		"ISH_GP5",		"n/a",			"n/a",		"n/a",		"n/a",		"n/a",	"n/a",
	"GPP_D19",		"I2S_MCLK1",		"n/a",			"n/a",		"n/a",		"n/a",		"n/a",	"n/a",
	"GSPI2_CLK_LOOPBK",	"GSPI2_CLK_LOOPBK",	"n/a",			"n/a",		"n/a",		"n/a",		"n/a",	"n/a",
};

const struct gpio_group tigerlake_pch_lp_group_d = {
	.display	= "------- GPIO Group GPP_D -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_lp_group_d_names) / 8,
	.func_count	= 8,
	.pad_names	= tigerlake_pch_lp_group_d_names,
};

const char *const tigerlake_pch_lp_group_e_names[] = {
	"GPP_E0",		"SATAXPCIE0",		"SATAGP0",		"n/a",	"n/a",		"n/a",
	"GPP_E1",		"n/a",			"THC0_SPI1_IO2",	"n/a",	"n/a",		"n/a",
	"GPP_E2",		"n/a",			"THC0_SPI1_IO3",	"n/a",	"n/a",		"n/a",
	"GPP_E3",		"CPU_GP0",		"n/a",			"n/a",	"n/a",		"n/a",
	"GPP_E4",		"SATA_DEVSLP0",		"n/a",			"n/a",	"n/a",		"n/a",
	"GPP_E5",		"SATA_DEVSLP1",		"n/a",			"n/a",	"n/a",		"n/a",
	"GPP_E6",		"n/a",			"THC0_SPI1_RST#",	"n/a",	"n/a",		"n/a",
	"GPP_E7",		"CPU_GP1",		"n/a",			"n/a",	"n/a",		"n/a",
	"GPP_E8",		"n/a",			"SATA_LED#",		"n/a",	"n/a",		"n/a",
	"GPP_E9",		"USB2_OC0#",		"n/a",			"n/a",	"n/a",		"n/a",
	"GPP_E10",		"n/a",			"THC0_SPI1_CS#",	"n/a",	"n/a",		"n/a",
	"GPP_E11",		"n/a",			"THC0_SPI1_CLK",	"n/a",	"n/a",		"n/a",
	"GPP_E12",		"n/a",			"THC0_SPI1_IO1",	"n/a",	"n/a",		"n/a",
	"GPP_E13",		"n/a",			"THC0_SPI1_IO0",	"n/a",	"n/a",		"n/a",
	"GPP_E14",		"DDSP_HPDA",		"DISP_MISCA",		"n/a",	"n/a",		"n/a",
	"GPP_E15",		"ISH_GP6",		"Reserved",		"n/a",	"n/a",		"n/a",
	"GPP_E16",		"ISH_GP7",		"Reserved",		"n/a",	"n/a",		"n/a",
	"GPP_E17",		"n/a",			"THC0_SPI1_INT#",	"n/a",	"n/a",		"n/a",
	"GPP_E18",		"DDP1_CTRLCLK",		"n/a",			"n/a",	"TBT_LSX0_TXD",	"BSSB_LS0_RX",
	"GPP_E19",		"DPP1_CTRLDATA",	"n/a",			"n/a",	"TBT_LSX0_RXD",	"BSSB_LS0_TX",
	"GPP_E20",		"DPP2_CTRLCLK",		"n/a",			"n/a",	"TBT_LSX1_TXD",	"BSSB_LS1_RX",
	"GPP_E21",		"DPP2_CTRLDATA",	"n/a",			"n/a",	"TBT_LSX1_RXD",	"BSSB_LS1_TX",
	"GPP_E22",		"DPAA_CTRLCLK",		"DNX_FORCE_RELOAD",	"n/a",	"n/a",		"n/a",
	"GPP_E23",		"DPPA_CTRLDATA",	"n/a",			"n/a",	"n/a",		"n/a",
	"GPPE_CLK_LOOPBK", 	"n/a",			 "THC0_CLK_LOOPBACK",	"n/a",	"n/a",		"n/a",
};

const struct gpio_group tigerlake_pch_lp_group_e = {
	.display	= "------- GPIO Group GPP_E -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_lp_group_e_names) / 6,
	.func_count	= 6,
	.pad_names	= tigerlake_pch_lp_group_e_names,
};

const char *const tigerlake_pch_lp_group_hvmos_names[] = {
	"L_BKLTEN",	 "L_BKLTEN",
	"L_BKLTCTL",	 "L_BKLTCTL",
	"L_VDDEN",	 "L_VDDEN",
	"SYS_PWROK",	 "SYS_PWROK",
	"SYS_RESET#",	 "SYS_RESET#",
	"MLK_RST#",	 "MLK_RST#",
};

const struct gpio_group tigerlake_pch_lp_group_hvmos = {
	.display	= "------- GPIO Group HVMOS -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_lp_group_hvmos_names) / 2,
	.func_count	= 2,
	.pad_names	= tigerlake_pch_lp_group_hvmos_names,
};

const char *const tigerlake_pch_lp_group_f_names[] = {
	"GPP_F0",		"CNV_BRI_DT",		"UART0_RTS#",		"n/a",
	"GPP_F1",		"CNV_BRI_RSP",		"UART0_RXD",		"n/a",
	"GPP_F2",		"CNV_RGI_DT",		"UART0_TXD",		"n/a",
	"GPP_F3",		"CNV_RGI_RSP",		"UART0_CTS#",		"n/a",
	"GPP_F4",		"CNV_RF_RESET#",	"n/a",			"n/a",
	"GPP_F5",		"n/a",			"MODEM_CLKREQ",		"CRF_XTAL_CLKREQ",
	"GPP_F6",		"CNV_PA_BLANKING",	"n/a",			"n/a",
	"GPP_F7",		"n/a",			"n/a",			"n/a",
	"GPP_F8",		"I2S_MCLK2_INOUT",	"n/a",			"n/a",
	"GPP_F9",		"Reserved",		"n/a",			"n/a",
	"GPP_F10",		"n/a",			"n/a",			"n/a",
	"GPP_F11",		"n/a",			"n/a",			"THC1_SPI2_CLK",
	"GPP_F12",		"GSXDOUT",		"n/a",			"THC1_SPI2_IO0",
	"GPP_F13",		"GSXSLOAD",		"n/a",			"THC1_SPI2_IO1",
	"GPP_F14",		"GSXDIN",		"n/a",			"THC1_SPI2_IO2",
	"GPP_F15",		"GSXSRESET#",		"n/a",			"THC1_SPI2_IO3",
	"GPP_F16",		"GSXCLK",		"n/a",			"THC1_SPI2_CS#",
	"GPP_F17",		"n/a",			"n/a",			"THC1_SPI2_RST#",
	"GPP_F18",		"n/a",			"n/a",			"THC1_SPI2_INT#",
	"GPP_F19",		"SRCCLKREQ6#",		"n/a",			"n/a",
	"GPP_F20",		"EXT_PWR_GATE#",	"n/a",			"n/a",
	"GPP_F21",		"EXT_PWR_GATE2#",	"n/a",			"n/a",
	"GPP_F22",		"VNN_CTRL",		"n/a",			"n/a",
	"GPP_F23",		"V1P05_CTRL",		"n/a",			"n/a",
	"GPPF_CLK_LOOPBK", 	"n/a",			"THC1_CLK_LOOPBACK",	"n/a",
};

const struct gpio_group tigerlake_pch_lp_group_f = {
	.display	= "------- GPIO Group GPP_F -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_lp_group_f_names) / 4,
	.func_count	= 4,
	.pad_names	= tigerlake_pch_lp_group_f_names,
};

const char *const tigerlake_pch_lp_group_h_names[] = {
	"GPP_H0",	"n/a",			"n/a",			"n/a",
	"GPP_H1",	"n/a",			"n/a",			"n/a",
	"GPP_H2",	"n/a",			"n/a",			"n/a",
	"GPP_H3",	"SX_EXIT_HOLDOFF",	"n/a",			"n/a",
	"GPP_H4",	"I2C2_SDA",		"n/a",			"n/a",
	"GPP_H5",	"I2C2_SCL",		"n/a",			"n/a",
	"GPP_H6",	"I2C3_SDA",		"n/a",			"n/a",
	"GPP_H7",	"I2C3_SCL",		"n/a",			"n/a",
	"GPP_H8",	"I2C4_SDA",		"CNV_MFUART2_RXD",	"n/a",
	"GPP_H9",	"I2C4_SCL",		"CNV_MFUART2_TXD",	"n/a",
	"GPP_H10",	"SRCCLKREQ4#",		"n/a",			"n/a",
	"GPP_H11",	"SRCCLKREQ5#",		"n/a",			"n/a",
	"GPP_H12",	"M2_SKT2_CFG0",		"n/a",			"n/a",
	"GPP_H13",	"M2_SKT2_CFG1",		"n/a",			"n/a",
	"GPP_H14",	"M2_SKT2_CFG2",		"n/a",			"n/a",
	"GPP_H15",	"M2_SKT2_CFG3",		"n/a",			"n/a",
	"GPP_H16",	"DDPB_CTRLCLK",		"n/a",			"PCIE_LNK_DOWN",
	"GPP_H17",	"DDPB_CTRLDATA",	"n/a",			"n/a",
	"GPP_H18",	"CPU_C10_GATE#",	"n/a",			"n/a",
	"GPP_H19",	"TIME_SYNC0",		"n/a",			"n/a",
	"GPP_H20",	"IMGCLKOUT1",		"n/a",			"n/a",
	"GPP_H21",	"IMGCLKOUT2",		"n/a",			"n/a",
	"GPP_H22",	"IMGCLKOUT3",		"n/a",			"n/a",
	"GPP_H23",	"IMGCLKOUT4",		"n/a",			"n/a",
};

const struct gpio_group tigerlake_pch_lp_group_h = {
	.display	= "------- GPIO Group GPP_H -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_lp_group_h_names) / 4,
	.func_count	= 4,
	.pad_names	= tigerlake_pch_lp_group_h_names,
};

const char *const tigerlake_pch_lp_group_r_names[] = {
	"GPP_R0",	"HDA_BCLK",	"I2S0_SCLK",
	"GPP_R1",	"HDA_SYNC",	"I2S0_SFRM",
	"GPP_R2",	"HDA_SDO",	"I2S0_TXD",
	"GPP_R3",	"HDA_SDI0",	"I2S0_RXD",
	"GPP_R4",	"HDA_RST#",	"n/a",
	"GPP_R5",	"HDA_SDI1",	"I2S1_RXD",
	"GPP_R6",	"n/a",		"I2S1_TXD",
	"GPP_R7",	"n/a",		"I2S1_SFRM",
};

const struct gpio_group tigerlake_pch_lp_group_r = {
	.display	= "------- GPIO Group GPP_R -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_lp_group_r_names) / 3,
	.func_count	= 3,
	.pad_names	= tigerlake_pch_lp_group_r_names,
};

const char *const tigerlake_pch_lp_group_s_names[] = {
	"GPP_S0",	"SNDW0_CLK",	"N/A",
	"GPP_S1",	"SNDW0_DATA",	"N/A",
	"GPP_S2",	"SNDW1_CLK",	"DMIC_CLK_B0",
	"GPP_S3",	"SNDW1_DATA",	"DMIC_CLK_B1",
	"GPP_S4",	"SNDW2_CLK#",	"DMIC_CLK_A1",
	"GPP_S5",	"SNDW2_DATA",	"DMIC_DATA1",
	"GPP_S6",	"SNDW3_CLK",	"DMIC_CLK_A0",
	"GPP_S7",	"SNDW3_DATA",	"DMIC_DATA0",
};

const struct gpio_group tigerlake_pch_lp_group_s = {
	.display	= "------- GPIO Group GPP_S -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_lp_group_s_names) / 3,
	.func_count	= 3,
	.pad_names	= tigerlake_pch_lp_group_s_names,
};

const char *const tigerlake_pch_lp_group_gpp_t_names[] = {
	"GPP_T0",
	"GPP_T1",
	"GPP_T2",
	"GPP_T3",
	"GPP_T4",
	"GPP_T5",
	"GPP_T6",
	"GPP_T7",
	"GPP_T8",
	"GPP_T9",
	"GPP_T10",
	"GPP_T11",
	"GPP_T12",
	"GPP_T13",
	"GPP_T14",
	"GPP_T15",
};

const struct gpio_group tigerlake_pch_lp_group_t = {
	.display	= "------- GPIO Group GPP_T (TGL UP3 only) -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_lp_group_gpp_t_names),
	.func_count	= 1,
	.pad_names	= tigerlake_pch_lp_group_gpp_t_names,
};

const char *const tigerlake_pch_lp_group_u_names[] = {
	"GPP_U0",
	"GPP_U1",
	"GPP_U2",
	"GPP_U3",
	"GPP_U4",
	"GPP_U5",
	"GPP_U6",
	"GPP_U7",
	"GPP_U8",
	"GPP_U9",
	"GPP_U10",
	"GPP_U11",
	"GPP_U12",
	"GPP_U13",
	"GPP_U14",
	"GPP_U15",
	"GPP_U16",
	"GPP_U17",
	"GPP_U18",
	"GPP_U19",
	"GSPI3_CLK_LOOPBK",
	"GSPI4_CLK_LOOPBK",
	"GSPI5_CLK_LOOPBK",
	"GSPI6_CLK_LOOPBK",
};

const struct gpio_group tigerlake_pch_lp_group_u = {
	.display	= "------- GPIO Group GPP_U (TGL UP3 only) -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_lp_group_u_names),
	.func_count	= 1,
	.pad_names	= tigerlake_pch_lp_group_u_names,
};

const char *const tigerlake_pch_lp_group_vgpio_names[] = {
	"VGPIO0",	"VGPIO0",
	"VGPIO4",	"VGPIO4",
	"VGPIO5",	"VGPIO5",
	"VGPIO6",	"VGPIO6",
	"VGPIO7",	"VGPIO7",
	"VGPIO8",	"VGPIO8",
	"VGPIO9",	"VGPIO9",
	"VGPIO10",	"VGPIO10",
	"VGPIO11",	"VGPIO11",
	"VGPIO12",	"VGPIO12",
	"VGPIO13",	"VGPIO13",
	"VGPIO18",	"VGPIO18",
	"VGPIO19",	"VGPIO19",
	"VGPIO20",	"VGPIO20",
	"VGPIO21",	"VGPIO21",
	"VGPIO22",	"VGPIO22",
	"VGPIO23",	"VGPIO23",
	"VGPIO24",	"VGPIO24",
	"VGPIO25",	"VGPIO25",
	"VGPIO30",	"VGPIO30",
	"VGPIO31",	"VGPIO31",
	"VGPIO32",	"VGPIO32",
	"VGPIO33",	"VGPIO33",
	"VGPIO34",	"VGPIO34",
	"VGPIO35",	"VGPIO35",
	"VGPIO36",	"VGPIO36",
	"VGPIO37",	"VGPIO37",
};

const struct gpio_group tigerlake_pch_lp_group_vgpio = {
	.display	= "------- GPIO Group VGPIO -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_lp_group_vgpio_names) / 2,
	.func_count	= 2,
	.pad_names	= tigerlake_pch_lp_group_vgpio_names,
};

const char *const tigerlake_pch_lp_group_gpd_names[] = {
	"GPD0",		"BATLOW#",
	"GPD1",		"ACPRESENT",
	"GPD2",		"LAN_WAKE#",
	"GPD3",		"PRWBTN#",
	"GPD4",		"SLP_S3#",
	"GPD5",		"SLP_S4#",
	"GPD6",		"SLP_A#",
	"GPD7",		"n/a",
	"GPD8",		"SUSCLK",
	"GPD9",		"SLP_WLAN#",
	"GPD10",	"SLP_S5#",
	"GPD11",	"LANPHYPC",
	"INPUT3VSEL",	"INPUT3VSEL",
	"SLP_LAN#",	"SLP_LAN#",
	"SLP_SUS#",	"SLP_SUS#",
	"WAKE#",	"WAKE#",
	"DRAM_RESET#",	"DRAM_RESET#",
};

const struct gpio_group tigerlake_pch_lp_group_gpd = {
	.display	= "------- GPIO Group GPD -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_lp_group_gpd_names) / 2,
	.func_count	= 2,
	.pad_names	= tigerlake_pch_lp_group_gpd_names,
};

const char *const tigerlake_pch_lp_group_cpu_names[] = {
	"HDACPU_SDI",		"HDACPU_SDI",
	"HDACPU_SDO",		"HDACPU_SDO",
	"HDACPU_SCLK",		"HDACPU_SCLK",
	"PM_SYNC",		"PM_SYNC",
	"PECI",			"PECI",
	"CPUPWRGD",		"CPUPWRGD",
	"THRMTRIP#",		"THRMTRIP#",
	"PLTRST_CPU#",		"PLTRST_CPU#",
	"PM_DOWN",		"PM_DOWN",
	"TRIGGER_IN",		"TRIGGER_IN",
	"TRIGGER_OUT",		"TRIGGER_OUT",
	"UFS_RESET#",		"UFS_RESET#",
	"CLKOUT_CPURTC",	"CLKOUT_CPURTC",
	"VCCST_OVERRIDE",	"VCCST_OVERRIDE",
	"C10_WAKE",		"C10_WAKE",
};

const struct gpio_group tigerlake_pch_lp_group_cpu = {
	.display	= "------- GPIO Group CPU -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_lp_group_cpu_names) / 2,
	.func_count	= 2,
	.pad_names	= tigerlake_pch_lp_group_cpu_names,
};

const char *const tigerlake_pch_lp_group_vgpio3_names[] = {
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
	"VGPIO_USB_0",		"VGPIO_USB_0",
	"VGPIO_USB_1",		"VGPIO_USB_1",
	"VGPIO_USB_2",		"VGPIO_USB_2",
	"VGPIO_USB_3",		"VGPIO_USB_3",
	"VGPIO_USB_4",		"VGPIO_USB_4",
	"VGPIO_USB_5",		"VGPIO_USB_5",
	"VGPIO_USB_6",		"VGPIO_USB_6",
	"VGPIO_USB_7",		"VGPIO_USB_7",
	"VGPIO_PCIE_80",	"VGPIO_PCIE_80",
	"VGPIO_PCIE_81",	"VGPIO_PCIE_81",
	"VGPIO_PCIE_82",	"VGPIO_PCIE_82",
	"VGPIO_PCIE_83",	"VGPIO_PCIE_83",
};

const struct gpio_group tigerlake_pch_lp_group_vgpio3 = {
	.display	= "------- GPIO Group VGPIO3 -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_lp_group_vgpio3_names) / 2,
	.func_count	= 2,
	.pad_names	= tigerlake_pch_lp_group_vgpio3_names,
};

const char *const tigerlake_pch_lp_group_jtag_names[] = {
	"JTAG_TDO",	"JTAG_TDO",
	"JTAGX",	"JTAGX",
	"PRDY#",	"PRDY#",
	"PREQ#",	"PREQ#",
	"CPU_TRST#",	"CPU_TRST#",
	"JTAG_TDI",	"JTAG_TDI",
	"JTAG_TMS",	"JTAG_TMS",
	"JTAG_TCK",	"JTAG_TCK",
	"DBG_PMODE",	"DBG_PMODE",
	"MLK",		"MLK",
};

const struct gpio_group tigerlake_pch_lp_group_jtag = {
	.display	= "------- GPIO Group JTAG -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_lp_group_jtag_names) / 2,
	.func_count	= 2,
	.pad_names	= tigerlake_pch_lp_group_jtag_names,
};

const char *const tigerlake_pch_lp_group_spi_names[] = {
	"SPI0_IO_2",		"SPI0_IO_2",
	"SPI0_IO_3",		"SPI0_IO_3",
	"SPI0_MOSI_IO_0",	"SPI0_MOSI_IO_0",
	"SPI0_MISO_IO_1",	"SPI0_MISO_IO_1",
	"SPI0_TPM_CS2#",	"SPI0_TPM_CS2#",
	"SPI0_FLASH_CS0#",	"SPI0_FLASH_CS0#",
	"SPI0_FLASH_CS1#",	"SPI0_FLASH_CS1#",
	"SPI0_CLK",		"SPI0_CLK",
	"SPI0_CLK_LOOPBK",	"SPI0_CLK_LOOPBK",
};

const struct gpio_group tigerlake_pch_lp_group_spi = {
	.display	= "------- GPIO Group SPI -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_lp_group_spi_names) / 2,
	.func_count	= 2,
	.pad_names	= tigerlake_pch_lp_group_spi_names,
};

const struct gpio_group *const tigerlake_pch_lp_community_0_groups[] = {
	&tigerlake_pch_lp_group_b,
	&tigerlake_pch_lp_group_t,
	&tigerlake_pch_lp_group_a,
};

const struct gpio_community tigerlake_pch_lp_community_0 = {
	.name		= "------- GPIO Community 0 -------",
	.pcr_port_id	= 0x6e,
	.group_count	= ARRAY_SIZE(tigerlake_pch_lp_community_0_groups),
	.groups		= tigerlake_pch_lp_community_0_groups,
};

const struct gpio_group *const tigerlake_pch_lp_community_1_groups[] = {
	&tigerlake_pch_lp_group_s,
	&tigerlake_pch_lp_group_h,
	&tigerlake_pch_lp_group_d,
	&tigerlake_pch_lp_group_u,
	&tigerlake_pch_lp_group_vgpio,
};
const struct gpio_community tigerlake_pch_lp_community_1 = {
	.name		= "------- GPIO Community 1 -------",
	.pcr_port_id	= 0x6d,
	.group_count	= ARRAY_SIZE(tigerlake_pch_lp_community_1_groups),
	.groups		= tigerlake_pch_lp_community_1_groups,
};

const struct gpio_group *const tigerlake_pch_lp_community_2_groups[] = {
	&tigerlake_pch_lp_group_gpd,
};

const struct gpio_community tigerlake_pch_lp_community_2 = {
	.name		= "------- GPIO Community 2 -------",
	.pcr_port_id	= 0x6c,
	.group_count	= ARRAY_SIZE(tigerlake_pch_lp_community_2_groups),
	.groups		= tigerlake_pch_lp_community_2_groups,
};

const struct gpio_group *const tigerlake_pch_lp_community_3_groups[] = {
	&tigerlake_pch_lp_group_cpu,
	&tigerlake_pch_lp_group_vgpio3,
};

const struct gpio_community tigerlake_pch_lp_community_3 = {
	.name		= "------- GPIO Community 3 -------",
	.pcr_port_id	= 0x6b,
	.group_count	= ARRAY_SIZE(tigerlake_pch_lp_community_3_groups),
	.groups		= tigerlake_pch_lp_community_3_groups,
};

const struct gpio_group *const tigerlake_pch_lp_community_4_groups[] = {
	&tigerlake_pch_lp_group_c,
	&tigerlake_pch_lp_group_f,
	&tigerlake_pch_lp_group_hvmos,
	&tigerlake_pch_lp_group_e,
	&tigerlake_pch_lp_group_jtag,
};

const struct gpio_community tigerlake_pch_lp_community_4 = {
	.name		= "------- GPIO Community 4 -------",
	.pcr_port_id	= 0x6a,
	.group_count	= ARRAY_SIZE(tigerlake_pch_lp_community_4_groups),
	.groups		= tigerlake_pch_lp_community_4_groups,
};

const struct gpio_group *const tigerlake_pch_lp_community_5_groups[] = {
	&tigerlake_pch_lp_group_r,
	&tigerlake_pch_lp_group_spi,
};

const struct gpio_community tigerlake_pch_lp_community_5 = {
	.name		= "------- GPIO Community 5 -------",
	.pcr_port_id	= 0x69,
	.group_count	= ARRAY_SIZE(tigerlake_pch_lp_community_5_groups),
	.groups		= tigerlake_pch_lp_community_5_groups,
};

const struct gpio_community *const tigerlake_pch_lp_communities[] = {
	&tigerlake_pch_lp_community_0,
	&tigerlake_pch_lp_community_1,
	&tigerlake_pch_lp_community_2,
	&tigerlake_pch_lp_community_3,
	&tigerlake_pch_lp_community_4,
	&tigerlake_pch_lp_community_5,
};

/* ----------------------------- Tiger Lake H ----------------------------- */

const char *const tigerlake_pch_h_group_a_names[] = {
/*
 *	These pads start at offset 0x680, but according to EDS the PADBAR is 0x700.
 *	This would cause the tool to parse the GPIOs incorrectly.
 *	For informational purposes only.
 */
/*
	"SPI0_IO_2",		"SPI0_IO_2",		"n/a",
	"SPI0_IO_3",		"SPI0_IO_3",		"n/a",
	"SPI0_MOSI_IO_0",	"SPI0_MOSI_IO_0",	"n/a",
	"SPI0_MISO_IO_1",	"SPI0_MISO_IO_1",	"n/a",
	"SPI0_TPM_CS2#",	"SPI0_TPM_CS2#",	"n/a",
	"SPI0_FLASH_CS0#",	"SPI0_FLASH_CS0#",	"n/a",
	"SPI0_FLASH_CS1#",	"SPI0_FLASH_CS1#",	"n/a",
	"SPI0_CLK",		"SPI0_CLK",		"n/a",
*/
	"GPP_A0",		"ESPI_IO0",		"n/a",
	"GPP_A1",		"ESPI_IO1",		"n/a",
	"GPP_A2",		"ESPI_IO2",		"SUSWARN#/SUSPWRDNACK",
	"GPP_A3",		"ESPI_IO3",		"SUSACK#",
	"GPP_A4",		"ESPI_CS0#",		"n/a",
	"GPP_A5",		"ESPI_CLK",		"n/a",
	"GPP_A6",		"ESPI_RESET#",		"n/a",
	"GPP_A7",		"ESPI_CS1#",		"n/a",
	"GPP_A8",		"ESPI_CS2#",		"n/a",
	"GPP_A9",		"ESPI_CS3#",		"n/a",
	"GPP_A10",		"ESPI_ALERT0#",		"n/a",
	"GPP_A11",		"ESPI_ALERT1#",		"n/a",
	"GPP_A12",		"ESPI_ALERT2#",		"n/a",
	"GPP_A13",		"ESPI_ALERT3#",		"n/a",
	"GPP_A14",		"n/a",			"IMGCLKOUT0",
	"SPI0_CLK_LOOPBK",	"SPI0_CLK_LOOPBK",	"n/a",
	"ESPI_CLK_LOOPBK",	"ESPI_CLK_LOOPBK",	"n/a",
};

const struct gpio_group tigerlake_pch_h_group_a = {
	.display	= "------- GPIO Group GPP_A -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_h_group_a_names) / 3,
	.func_count	= 3,
	.pad_names	= tigerlake_pch_h_group_a_names,
};

const char *const tigerlake_pch_h_group_b_names[] = {
	"GPP_B0",		"GSPI0_CS1#",		"IMGCLKOUT1",
	"GPP_B1",		"GSPI1_CS1#",		"TIME_SYNC1",
	"GPP_B2",		"VRALERT#",		"n/a",
	"GPP_B3",		"CPU_GP2",		"n/a",
	"GPP_B4",		"CPU_GP3",		"n/a",
	"GPP_B5",		"SRCCLKREQ0#",		"n/a",
	"GPP_B6",		"SRCCLKREQ1#",		"n/a",
	"GPP_B7",		"SRCCLKREQ2#",		"n/a",
	"GPP_B8",		"SRCCLKREQ3#",		"n/a",
	"GPP_B9",		"SRCCLKREQ4#",		"n/a",
	"GPP_B10",		"SRCCLKREQ5#",		"n/a",
	"GPP_B11",		"I2S_MCLK",		"n/a",
	"GPP_B12",		"SLP_S0#",		"n/a",
	"GPP_B13",		"PLTRST#",		"n/a",
	"GPP_B14",		"SPKR",			"n/a",
	"GPP_B15",		"GSPI0_CS0#",		"n/a",
	"GPP_B16",		"GSPI0_CLK",		"n/a",
	"GPP_B17",		"GSPI0_MISO",		"n/a",
	"GPP_B18",		"GSPI0_MOSI",		"n/a",
	"GPP_B19",		"GSPI1_CS0#",		"n/a",
	"GPP_B20",		"GSPI1_CLK",		"n/a",
	"GPP_B21",		"GSPI1_MISO",		"n/a",
	"GPP_B22",		"GSPI1_MOSI",		"n/a",
	"GPP_B23",		"SML1ALERT#",		"PCHHOT#",
	"GSPI0_CLK_LOOPBK",	"GSPI0_CLK_LOOPBK",	"n/a",
	"GSPI1_CLK_LOOPBK",	"GSPI1_CLK_LOOPBK",	"n/a",
};

const struct gpio_group tigerlake_pch_h_group_b = {
	.display	= "------- GPIO Group GPP_B -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_h_group_b_names) / 3,
	.func_count	= 3,
	.pad_names	= tigerlake_pch_h_group_b_names,
};

const char *const tigerlake_pch_h_group_c_names[] = {
	"GPP_C0",	"SMBCLK",		"n/a",			"n/a",		"n/a",
	"GPP_C1",	"SMBDATA",		"n/a",			"n/a",		"n/a",
	"GPP_C2",	"SMBALERT#",		"n/a",			"n/a",		"n/a",
	"GPP_C3",	"ISH_UART0_RXD",	"n/a",			"I2C2_SDA",	"n/a",
	"GPP_C4",	"ISH_UART0_TXD",	"n/a",			"I2C2_SCL",	"n/a",
	"GPP_C5",	"SML0ALERT#",		"n/a",			"n/a",		"n/a",
	"GPP_C6",	"ISH_I2C2_SDA",		"I2C3_SDA",		"SBK4",		"BK4",
	"GPP_C7",	"ISH_I2C2_SCL",		"I2C3_SCL",		"n/a",		"n/a",
	"GPP_C8",	"UART0_RXD",		"n/a",			"n/a",		"n/a",
	"GPP_C9",	"UART0_TXD",		"n/a",			"n/a",		"n/a",
	"GPP_C10",	"UART0_RTS#",		"n/a",			"n/a",		"n/a",
	"GPP_C11",	"UART0_CTS#",		"n/a",			"n/a",		"n/a",
	"GPP_C12",	"UART1_RXD",		"ISH_UART1_RXD",	"n/a",		"n/a",
	"GPP_C13",	"UART1_TXD",		"ISH_UART1_TXD",	"n/a",		"n/a",
	"GPP_C14",	"UART1_RTS#",		"ISH_UART1_RTS#",	"n/a",		"n/a",
	"GPP_C15",	"UART1_CTS#",		"ISH_UART1_CTS#",	"n/a",		"n/a",
	"GPP_C16",	"I2C0_SDA",		"n/a",			"n/a",		"n/a",
	"GPP_C17",	"I2C0_SCL",		"n/a",			"n/a",		"n/a",
	"GPP_C18",	"I2C1_SDA",		"n/a",			"n/a",		"n/a",
	"GPP_C19",	"I2C1_SCL",		"n/a",			"n/a",		"n/a",
	"GPP_C20",	"UART2_RXD",		"n/a",			"n/a",		"n/a",
	"GPP_C21",	"UART2_TXD",		"n/a",			"n/a",		"n/a",
	"GPP_C22",	"UART2_RTS#",		"n/a",			"n/a",		"n/a",
	"GPP_C23",	"UART2_CTS#",		"n/a",			"n/a",		"n/a",
};

const struct gpio_group tigerlake_pch_h_group_c = {
	.display	= "------- GPIO Group GPP_C -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_h_group_c_names) / 5,
	.func_count	= 5,
	.pad_names	= tigerlake_pch_h_group_c_names,
};

const char *const tigerlake_pch_h_group_d_names[] = {
	"GPP_D0",		"n/a",			"THC0_SPI1_CS#",	"SBK0",			"BK0",
	"GPP_D1",		"n/a",			"THC0_SPI1_CLK",	"SBK1",			"BK1",
	"GPP_D2",		"n/a",			"THC0_SPI1_IO1",	"SBK2",			"BK2",
	"GPP_D3",		"n/a",			"THC0_SPI1_IO0",	"SBK3",			"BK3",
	"GPP_D4",		"SML1CLK",		"n/a",			"n/a",			"n/a",
	"GPP_D5",		"I2S2_SFRM",		"CNV_RF_RESET#",	"n/a",			"n/a",
	"GPP_D6",		"I2S2_TXD",		"MODEM_CLKREQ",		"CRF_XTAL_CLKREQ",	"n/a",
	"GPP_D7",		"I2S2_RXD",		"THC0_SPI1_RST#",	"n/a",			"n/a",
	"GPP_D8",		"I2S2_SCLK",		"THC0_SPI1_INT#",	"n/a",			"n/a",
	"GPP_D9",		"SML0CLK",		"n/a",			"n/a",			"n/a",
	"GPP_D10",		"SML0DATA",		"n/a",			"n/a",			"n/a",
	"GPP_D11",		"n/a",			"n/a",			"n/a",			"n/a",
	"GPP_D12",		"ISH_UART0_CTS#",	"n/a",			"n/a",			"n/a",
	"GPP_D13",		"n/a",			"THC0_SPI1_IO2",	"n/a",			"n/a",
	"GPP_D14",		"n/a",			"THC0_SPI1_IO3",	"n/a",			"n/a",
	"GPP_D15",		"SML1DATA",		"n/a",			"n/a",			"n/a",
	"GPP_D16",		"GSPI3_CS0#",		"THC1_SPI2_CS#",	"n/a",			"n/a",
	"GPP_D17",		"GSPI3_CLK",		"THC1_SPI2_CLK",	"n/a",			"n/a",
	"GPP_D18",		"GSPI3_MISO",		"THC1_SPI2_IO0",	"n/a",			"n/a",
	"GPP_D19",		"GSPI3_MOSI",		"THC1_SPI2_IO1",	"n/a",			"n/a",
	"GPP_D20",		"UART3_RXD",		"THC1_SPI2_IO2",	"n/a",			"n/a",
	"GPP_D21",		"UART3_TXD",		"THC1_SPI2_IO3",	"n/a",			"n/a",
	"GPP_D22",		"UART3_RTS#", 		"THC1_SPI2_RST#",	"n/a",			"n/a",
	"GPP_D23",		"UART3_CTS#",		"THC1_SPI2_INT#",	"n/a",			"n/a",
	/* Below are just guesses */
	"SPI1_CLK_LOOPBK",	"n/a",			"THC0_CLK_LOOPBK",	"n/a",		"n/a",
	"GPI3_CLK_LOOPBK",	"n/a",			"THC1_CLK_LOOPBK",	"n/a",		"n/a",
};

const struct gpio_group tigerlake_pch_h_group_d = {
	.display	= "------- GPIO Group GPP_D -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_h_group_d_names) / 5,
	.func_count	= 5,
	.pad_names	= tigerlake_pch_h_group_d_names,
};

const char *const tigerlake_pch_h_group_e_names[] = {
	"GPP_E0",	"SATAXPCIE0",	"SATAGP0",
	"GPP_E1",	"SATAXPCIE1",	"SATAGP1",
	"GPP_E2",	"SATAXPCIE2",	"SATAGP2",
	"GPP_E3",	"CPU_GP0",	"n/a",
	"GPP_E4",	"SATA_DEVSLP0",	"n/a",
	"GPP_E5",	"SATA_DEVSLP1",	"n/a",
	"GPP_E6",	"SATA_DEVSLP2",	"n/a",
	"GPP_E7",	"CPU_GP1",	"n/a",
	"GPP_E8",	"SATALED#",	"n/a",
	"GPP_E9",	"USB_OC0#",	"n/a",
	"GPP_E10",	"USB_OC1#",	"n/a",
	"GPP_E11",	"USB_OC2#",	"n/a",
	"GPP_E12",	"USB_OC3#",	"n/a",
};

const struct gpio_group tigerlake_pch_h_group_e = {
	.display	= "------- GPIO Group GPP_E -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_h_group_e_names) / 3,
	.func_count	= 3,
	.pad_names	= tigerlake_pch_h_group_e_names,
};

const char *const tigerlake_pch_h_group_f_names[] = {
	"GPP_F0",		"SATAXPCIE3",		"SATAGP3",
	"GPP_F1",		"SATAXPCIE4",		"SATAGP4",
	"GPP_F2",		"SATAXPCIE5",		"SATAGP5",
	"GPP_F3",		"SATAXPCIE6",		"SATAGP6",
	"GPP_F4",		"SATAXPCIE7",		"SATAGP7",
	"GPP_F5",		"SATA_DEVSLP3",		"n/a",
	"GPP_F6",		"SATA_DEVSLP4",		"n/a",
	"GPP_F7",		"SATA_DEVSLP5",		"n/a",
	"GPP_F8",		"SATA_DEVSLP6",		"n/a",
	"GPP_F9",		"SATA_DEVSLP7",		"n/a",
	"GPP_F10",		"SATA_SCLOCK",		"n/a",
	"GPP_F11",		"SATA_SLOAD",		"n/a",
	"GPP_F12",		"SATA_SDATAOUT1",	"n/a",
	"GPP_F13",		"SATA_SDATAOUT0",	"n/a",
	"GPP_F14",		"PS_ON#",		"n/a",
	"GPP_F15",		"M2_SKT2_CFG0",		"n/a",
	"GPP_F16",		"M2_SKT2_CFG1",		"n/a",
	"GPP_F17",		"M2_SKT2_CFG2",		"n/a",
	"GPP_F18",		"M2_SKT2_CFG3",		"n/a",
	"GPP_F19",		"eDP_VDDEN",		"n/a",
	"GPP_F20",		"eDP_BKLTEN",		"n/a",
	"GPP_F21",		"eDP_BKLTCTL",		"n/a",
	"GPP_F22",		"VNN_CTRL",		"n/a",
	"GPP_F23",		"n/a",			"n/a",
};

const struct gpio_group tigerlake_pch_h_group_f = {
	.display	= "------- GPIO Group GPP_F -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_h_group_f_names) / 3,
	.func_count	= 3,
	.pad_names	= tigerlake_pch_h_group_f_names,
};

const char *const tigerlake_pch_h_group_g_names[] = {
	"GPP_G0",	"DDPA_CTRLCLK",		"n/a",			"n/a",		"n/a",		"n/a",
	"GPP_G1",	"DDPA_CTRLDATA",	"n/a",			"n/a",		"n/a",		"n/a",
	"GPP_G2",	"DNX_FORCE_RELOAD",	"n/a",			"n/a",		"n/a",		"n/a",
	"GPP_G3",	"n/a",			"n/a",			"n/a",		"n/a",		"n/a",
	"GPP_G4",	"n/a",			"n/a",			"n/a",		"n/a",		"n/a",
	"GPP_G5",	"SLP_DRAM#",		"n/a",			"n/a",		"n/a",		"n/a",
	"GPP_G6",	"n/a",			"Reserved",		"n/a",		"n/a",		"n/a",
	"GPP_G7",	"n/a",			"Reserved",		"n/a",		"n/a",		"n/a",
	"GPP_G8",	"ISH_SPI_CS#",		"DDP3_CTRLCLK",		"GSPI2_CS0#",	"TBT_LSX2_TXD",	"BSSB_LS2_RX",
	"GPP_G9",	"ISH_SPI_CLK",		"DDP3_CTRLDATA",	"GSPI2_CLK",	"TBT_LSX2_RXD",	"BSSB_LS2_TX",
	"GPP_G10",	"ISH_SPI_MISO",		"DDP4_CTRLCLK",		"GSPI2_MISO",	"TBT_LSX3_TXD",	"BSSB_LS3_RX",
	"GPP_G11",	"ISH_SPI_MOSI",		"DDP4_CTRLDATA",	"GSPI2_MOSI",	"TBT_LSX3_RXD",	"BSSB_LS3_TX",
	"GPP_G12",	"DDP1_CTRLCLK",		"n/a",			"TBT_LSX0_TXD",	"BSSB_LS0_RX",	"n/a",
	"GPP_G13",	"DDP1_CTRLDATA",	"n/a",			"TBT_LSX0_RXD",	"BSSB_LS0_TX",	"n/a",
	"GPP_G14",	"DDP2_CTRLCLK",		"n/a",			"TBT_LSX1_TXD",	"BSSB_LS1_RX",	"n/a",
	"GPP_G15",	"DDP2_CTRLDATA",	"n/a",			"TBT_LSX1_RXD",	"BSSB_LS1_TX",	"n/a",
};

const struct gpio_group tigerlake_pch_h_group_g = {
	.display	= "------- GPIO Group GPP_G -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_h_group_g_names) / 6,
	.func_count	= 6,
	.pad_names	= tigerlake_pch_h_group_g_names,
};

const char *const tigerlake_pch_h_group_h_names[] = {
	"GPP_H0",		"SRCCLKREQ6#",		"n/a",
	"GPP_H1",		"SRCCLKREQ7#",		"n/a",
	"GPP_H2",		"SRCCLKREQ8#",		"n/a",
	"GPP_H3",		"SRCCLKREQ9#",		"n/a",
	"GPP_H4",		"SRCCLKREQ10#",		"n/a",
	"GPP_H5",		"SRCCLKREQ11#",		"n/a",
	"GPP_H6",		"SRCCLKREQ12#",		"n/a",
	"GPP_H7",		"SRCCLKREQ13#",		"n/a",
	"GPP_H8",		"SRCCLKREQ14#",		"n/a",
	"GPP_H9",		"SRCCLKREQ15#",		"n/a",
	"GPP_H10",		"SML2CLK",		"n/a",
	"GPP_H11",		"SML2DATA",		"n/a",
	"GPP_H12",		"SML2ALERT#",		"n/a",
	"GPP_H13",		"SML3CLK",		"n/a",
	"GPP_H14",		"SML3DATA",		"n/a",
	"GPP_H15",		"SML3ALERT#",		"n/a",
	"GPP_H16",		"SML4CLK",		"n/a",
	"GPP_H17",		"SML4DATA",		"n/a",
	"GPP_H18",		"SML4ALERT#",		"n/a",
	"GPP_H19",		"ISH_I2C0_SDA",		"n/a",
	"GPP_H20",		"ISH_I2C0_SCL",		"n/a",
	"GPP_H21",		"ISH_I2C1_SDA",		"SMI#",
	"GPP_H22",		"ISH_I2C1_SCL",		"NMI#",
	"GPP_H23",		"TIME_SYNC0",		"n/a",
};

const struct gpio_group tigerlake_pch_h_group_h = {
	.display	= "------- GPIO Group GPP_H -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_h_group_h_names) / 3,
	.func_count	= 3,
	.pad_names	= tigerlake_pch_h_group_h_names,
};

const char *const tigerlake_pch_h_group_i_names[] = {
	"GPP_I0",	"PMCALERT#",		"n/a",
	"GPP_I1",	"DDSP_HPD1",		"DISP_MISC1",
	"GPP_I2",	"DDSP_HPD2",		"DISP_MISC2",
	"GPP_I3",	"DDSP_HPD3",		"DISP_MISC3",
	"GPP_I4",	"DDSP_HPD4",		"DISP_MISC4",
	"GPP_I5",	"DDPB_CTRLCLK",		"n/a",
	"GPP_I6",	"DDPB_CTRLDATA",	"n/a",
	"GPP_I7",	"DDPC_CTRLCLK",		"n/a",
	"GPP_I8",	"DDPC_CTRLDATA",	"n/a",
	"GPP_I9",	"Reserved",		"n/a",
	"GPP_I10",	"Reserved",		"n/a",
	"GPP_I11",	"USB_OC4#",		"I2C4_SDA",
	"GPP_I12",	"USB_OC5#",		"I2C4_SCL",
	"GPP_I13",	"USB_OC6#",		"I2C5_SDA",
	"GPP_I14",	"USB_OC7#",		"I2C5_SCL",
};

const struct gpio_group tigerlake_pch_h_group_i = {
	.display	= "------- GPIO Group GPP_I -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_h_group_i_names) / 3,
	.func_count	= 3,
	.pad_names	= tigerlake_pch_h_group_i_names,
};

const char *const tigerlake_pch_h_group_j_names[] = {
	"GPP_J0",	"CNV_PA_BLANKING",	"n/a",
	"GPP_J1",	"CPU_C10_GATE#",	"n/a",
	"GPP_J2",	"CNV_BRI_DT",		"UART0_RTS#",
	"GPP_J3",	"CNV_BRI_RSP",		"UART0_RXD",
	"GPP_J4",	"CNV_RGI_DT",		"UART0_TXD",
	"GPP_J5",	"CNV_RGI_RSP",		"UART0_CTS#",
	"GPP_J6",	"CNV_MFUART2_RXD",	"n/a",
	"GPP_J7",	"CNV_MFUART2_TXD",	"n/a",
	"GPP_J8",	"n/a",			"n/a",
	"GPP_J9",	"n/a",			"n/a",
};

const struct gpio_group tigerlake_pch_h_group_j = {
	.display	= "------- GPIO Group GPP_J -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_h_group_j_names) / 3,
	.func_count	= 3,
	.pad_names	= tigerlake_pch_h_group_j_names,
};


const char *const tigerlake_pch_h_group_k_names[] = {
	"GPP_K0",	"GSXDOUT",	"n/a",
	"GPP_K1",	"GSXSLOAD",	"n/a",
	"GPP_K2",	"GSXDIN",	"n/a",
	"GPP_K3",	"GSXSRESET#",	"n/a",
	"GPP_K4",	"GSXCLK",	"n/a",
	"GPP_K5",	"ADR_COMPLETE",	"n/a",
	"GPP_K6",	"DDSP_HPDA",	"DISP_MISCA",
	"GPP_K7",	"DDSP_HPDB",	"DISP_MISCB",
	"GPP_K8",	"CORE_VID0",	"n/a",
	"GPP_K9",	"CORE_VID1",	"n/a",
	"GPP_K10",	"DDSP_HPDC",	"DISP_MISCC",
	"GPP_K11",	"n/a",		"n/a",
	"SYS_PWROK",	"SYS_PWROK",	"n/a",
	"SYS_RESET#",	"SYS_RESET#",	"n/a",
	"MLK_RST#",	"MLK_RST#",	"n/a",
};

const struct gpio_group tigerlake_pch_h_group_k = {
	.display	= "------- GPIO Group GPP_K -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_h_group_k_names) / 3,
	.func_count	= 3,
	.pad_names	= tigerlake_pch_h_group_k_names,
};

const char *const tigerlake_pch_h_group_r_names[] = {
	"GPP_R0",	"HDA_BCLK",		"I2S0_SCLK",	"n/a",	"HDACPU_BCLK",
	"GPP_R1",	"HDA_SYNC",		"I2S0_SFRM",	"n/a",	"n/a",
	"GPP_R2",	"HDA_SDO",		"I2S0_TXD",	"n/a",	"HDACPU_SDO",
	"GPP_R3",	"HDA_SDI0",		"I2S0_RXD",	"n/a",	"HDACPU_SDI",
	"GPP_R4",	"HDA_RST#",		"n/a",		"n/a",	"n/a",
	"GPP_R5",	"HDA_SDI1",		"I2S1_RXD",	"n/a",	"n/a",
	"GPP_R6",	"n/a",			"I2S1_TXD",	"n/a",	"n/a",
	"GPP_R7",	"n/a",			"I2S1_SFRM",	"n/a",	"n/a",
	"GPP_R8",	"n/a",			"I2S1_SCLK",	"n/a",	"n/a",
	"GPP_R9",	"PCIE_LNK_DOWN",	"n/a",		"n/a",	"n/a",
	"GPP_R10",	"ISH_UART0_RTS#",	"GSPI2_CS1#",	"n/a",	"n/a",
	"GPP_R11",	"SX_EXIT_HOLDOFF#",	"ISH_GP6",	"n/a",	"n/a",
	"GPP_R12",	"CLKOUT_48",		"n/a",		"n/a",	"n/a",
	"GPP_R13",	"ISH_GP7",		"n/a",		"n/a",	"n/a",
	"GPP_R14",	"ISH_GP0",		"n/a",		"n/a",	"n/a",
	"GPP_R15",	"ISH_GP1",		"n/a",		"n/a",	"n/a",
	"GPP_R16",	"ISH_GP2",		"n/a",		"n/a",	"n/a",
	"GPP_R17",	"ISH_GP3",		"n/a",		"n/a",	"n/a",
	"GPP_R18",	"ISH_GP4",		"n/a",		"n/a",	"n/a",
	"GPP_R19",	"ISH_GP5",		"n/a",		"n/a",	"n/a",
};

const struct gpio_group tigerlake_pch_h_group_r = {
	.display	= "------- GPIO Group GPP_R -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_h_group_r_names) / 5,
	.func_count	= 5,
	.pad_names	= tigerlake_pch_h_group_r_names,
};

const char *const tigerlake_pch_h_group_s_names[] = {
	"GPP_S0",	"SNDW1_CLK",	"n/a",
	"GPP_S1",	"SNDW1_DATA",	"n/a",
	"GPP_S2",	"SNDW2_CLK",	"DMIC_CKLB0",
	"GPP_S3",	"SNDW2_DATA",	"DMIC_CLKB1",
	"GPP_S4",	"SNDW3_CLK",	"DMIC_CLKA1",
	"GPP_S5",	"SNDW3_DATA",	"DMIC_DATA1",
	"GPP_S6",	"SNDW4_CLK",	"DMIC_CLKA0",
	"GPP_S7",	"SNDW4_DATA",	"DMIC_DATA0",
};

const struct gpio_group tigerlake_pch_h_group_s = {
	.display	= "------- GPIO Group GPP_S -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_h_group_s_names) / 3,
	.func_count	= 3,
	.pad_names	= tigerlake_pch_h_group_s_names,
};

const char *const tigerlake_pch_h_group_gpd_names[] = {
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
	"SLP_LAN#",	"SLP_LAN#",
	"SLP_SUS#",	"SLP_SUS#",
	"WAKE#",	"WAKE#",
	"DRAM_RESET#",	"DRAM_RESET#",
};

const struct gpio_group tigerlake_pch_h_group_gpd = {
	.display	= "------- GPIO Group GPD -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_h_group_gpd_names) / 2,
	.func_count	= 2,
	.pad_names	= tigerlake_pch_h_group_gpd_names,
};

const char *const tigerlake_pch_h_group_vgpio0_names[] = {
	"VGPIO_USB_0",	"VGPIO_USB_0",
	"VGPIO_USB_1",	"VGPIO_USB_1",
	"VGPIO_USB_2",	"VGPIO_USB_2",
	"VGPIO_USB_3",	"VGPIO_USB_3",
	"VGPIO_USB_8",	"VGPIO_USB_8",
	"VGPIO_USB_9",	"VGPIO_USB_9",
	"VGPIO_USB_10",	"VGPIO_USB_10",
	"VGPIO_USB_11",	"VGPIO_USB_11",
};

const struct gpio_group tigerlake_pch_h_group_vgpio0 = {
	.display	= "------- GPIO Group VGPIO0 -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_h_group_vgpio0_names) / 2,
	.func_count	= 2,
	.pad_names	= tigerlake_pch_h_group_vgpio0_names,
};

const char *const tigerlake_pch_h_group_vgpio_names[] = {
	"VGPIO0",	"VGPIO0",
	"VGPIO4",	"VGPIO4",
	"VGPIO5",	"VGPIO5",
	"VGPIO6",	"VGPIO6",
	"VGPIO7",	"VGPIO7",
	"VGPIO8",	"VGPIO8",
	"VGPIO9",	"VGPIO9",
	"VGPIO10",	"VGPIO10",
	"VGPIO11",	"VGPIO11",
	"VGPIO12",	"VGPIO12",
	"VGPIO13",	"VGPIO13",
	"VGPIO18",	"VGPIO18",
	"VGPIO19",	"VGPIO19",
	"VGPIO20",	"VGPIO20",
	"VGPIO21",	"VGPIO21",
	"VGPIO22",	"VGPIO22",
	"VGPIO23",	"VGPIO23",
	"VGPIO24",	"VGPIO24",
	"VGPIO25",	"VGPIO25",
	"VGPIO30",	"VGPIO30",
	"VGPIO31",	"VGPIO31",
	"VGPIO32",	"VGPIO32",
	"VGPIO33",	"VGPIO33",
	"VGPIO34",	"VGPIO34",
	"VGPIO35",	"VGPIO35",
	"VGPIO36",	"VGPIO36",
	"VGPIO37",	"VGPIO37",
};

const struct gpio_group tigerlake_pch_h_group_vgpio = {
	.display	= "------- GPIO Group VGPIO -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_h_group_vgpio_names) / 2,
	.func_count	= 2,
	.pad_names	= tigerlake_pch_h_group_vgpio_names,
};

const char *const tigerlake_pch_h_group_cpu_names[] = {
	"HDACPU_SDI",		"HDACPU_SDI",
	"HDACPU_SDO",		"HDACPU_SDO",
	"HDACPU_BCLK",		"HDACPU_BCLK",
	"PM_SYNC",		"PM_SYNC",
	"PECI",			"PECI",
	"CPUPWRGD",		"CPUPWRGD",
	"THRMTRIP#",		"THRMTRIP#",
	"PLTRST_CPU#",		"PLTRST_CPU#",
	"PM_DOWN",		"PM_DOWN",
	"TRIGGER_IN",		"TRIGGER_IN",
	"TRIGGER_OUT",		"TRIGGER_OUT",
	"CLKOUT_CPURTC",	"CLKOUT_CPURTC",
	"VCCST_OVERRIDE",	"VCCST_OVERRIDE",
	"C10_WAKE",		"C10_WAKE",
};

const struct gpio_group tigerlake_pch_h_group_cpu = {
	.display	= "------- GPIO Group CPU -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_h_group_cpu_names) / 2,
	.func_count	= 2,
	.pad_names	= tigerlake_pch_h_group_cpu_names,
};

const char *const tigerlake_pch_h_group_vgpio3_names[] = {
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

const struct gpio_group tigerlake_pch_h_group_vgpio3 = {
	.display	= "------- GPIO Group VGPIO3 -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_h_group_vgpio3_names) / 2,
	.func_count	= 2,
	.pad_names	= tigerlake_pch_h_group_vgpio3_names,
};

const char *const tigerlake_pch_h_group_jtag_names[] = {
	"JTAG_TDO",	"JTAG_TDO",
	"JTAGX",	"JTAGX",
	"PRDY#",	"PRDY#",
	"PREQ#",	"PREQ#",
	"CPU_TRST#",	"CPU_TRST#",
	"JTAG_TDI",	"JTAG_TDI",
	"JTAG_TMS",	"JTAG_TMS",
	"JTAG_TCK",	"JTAG_TCK",
	"DBG_PMODE",	"DBG_PMODE",
	"MLK",		"MLK",
};

const struct gpio_group tigerlake_pch_h_group_jtag = {
	.display	= "------- GPIO Group JTAG -------",
	.pad_count	= ARRAY_SIZE(tigerlake_pch_h_group_jtag_names) / 2,
	.func_count	= 2,
	.pad_names	= tigerlake_pch_h_group_jtag_names,
};


const struct gpio_group *const tigerlake_pch_h_community_0_groups[] = {
	&tigerlake_pch_h_group_a,
	&tigerlake_pch_h_group_r,
	&tigerlake_pch_h_group_b,
	&tigerlake_pch_h_group_vgpio0,
};

const struct gpio_community tigerlake_pch_h_community_0 = {
	.name		= "------- GPIO Community 0 -------",
	.pcr_port_id	= 0x6e,
	.group_count	= ARRAY_SIZE(tigerlake_pch_h_community_0_groups),
	.groups		= tigerlake_pch_h_community_0_groups,
};

const struct gpio_group *const tigerlake_pch_h_community_1_groups[] = {
	&tigerlake_pch_h_group_d,
	&tigerlake_pch_h_group_c,
	&tigerlake_pch_h_group_s,
	&tigerlake_pch_h_group_g,
	&tigerlake_pch_h_group_vgpio,
};
const struct gpio_community tigerlake_pch_h_community_1 = {
	.name		= "------- GPIO Community 1 -------",
	.pcr_port_id	= 0x6d,
	.group_count	= ARRAY_SIZE(tigerlake_pch_h_community_1_groups),
	.groups		= tigerlake_pch_h_community_1_groups,
};

const struct gpio_group *const tigerlake_pch_h_community_2_groups[] = {
	&tigerlake_pch_h_group_gpd,
};

const struct gpio_community tigerlake_pch_h_community_2 = {
	.name		= "------- GPIO Community 2 -------",
	.pcr_port_id	= 0x6c,
	.group_count	= ARRAY_SIZE(tigerlake_pch_h_community_2_groups),
	.groups		= tigerlake_pch_h_community_2_groups,
};

const struct gpio_group *const tigerlake_pch_h_community_3_groups[] = {
	&tigerlake_pch_h_group_e,
	&tigerlake_pch_h_group_f,
	&tigerlake_pch_h_group_vgpio3,
};

const struct gpio_community tigerlake_pch_h_community_3 = {
	.name		= "------- GPIO Community 3 -------",
	.pcr_port_id	= 0x6b,
	.group_count	= ARRAY_SIZE(tigerlake_pch_h_community_3_groups),
	.groups		= tigerlake_pch_h_community_3_groups,
};

const struct gpio_group *const tigerlake_pch_h_community_4_groups[] = {
	&tigerlake_pch_h_group_h,
	&tigerlake_pch_h_group_j,
	&tigerlake_pch_h_group_k,
};

const struct gpio_community tigerlake_pch_h_community_4 = {
	.name		= "------- GPIO Community 4 -------",
	.pcr_port_id	= 0x6a,
	.group_count	= ARRAY_SIZE(tigerlake_pch_h_community_4_groups),
	.groups		= tigerlake_pch_h_community_4_groups,
};

const struct gpio_group *const tigerlake_pch_h_community_5_groups[] = {
	&tigerlake_pch_h_group_i,
	&tigerlake_pch_h_group_jtag,
	&tigerlake_pch_h_group_cpu,
};

const struct gpio_community tigerlake_pch_h_community_5 = {
	.name		= "------- GPIO Community 5 -------",
	.pcr_port_id	= 0x69,
	.group_count	= ARRAY_SIZE(tigerlake_pch_h_community_5_groups),
	.groups		= tigerlake_pch_h_community_5_groups,
};

const struct gpio_community *const tigerlake_pch_h_communities[] = {
	&tigerlake_pch_h_community_0,
	&tigerlake_pch_h_community_1,
	&tigerlake_pch_h_community_2,
	&tigerlake_pch_h_community_3,
	&tigerlake_pch_h_community_4,
	&tigerlake_pch_h_community_5,
};

#endif
