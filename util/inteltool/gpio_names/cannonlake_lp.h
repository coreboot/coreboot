#ifndef GPIO_NAMES_CANNONLAKE_LP
#define GPIO_NAMES_CANNONLAKE_LP

#include "gpio_groups.h"

const char *const cannonlake_pch_lp_group_a_names[] = {
	"GPP_A0",		"RCIN#",		"TIME_SYNC1",	"n/a",
	"GPP_A1",		"LAD0",			"ESPI_IO0",	"n/a",
	"GPP_A2",		"LAD1",			"ESPI_IO1",	"n/a",
	"GPP_A3",		"LAD2",			"ESPI_IO2",	"n/a",
	"GPP_A4",		"LAD3",			"ESPI_IO3",	"n/a",
	"GPP_A5",		"LFRAME#",		"ESPI_CS0#",	"n/a",
	"GPP_A6",		"SERIRQ",		"n/a",		"n/a",
	"GPP_A7",		"PIRQA#",		"GSPI0_CS1#",	"n/a",
	"GPP_A8",		"CLKRUN#",		"n/a",		"n/a",
	"GPP_A9",		"CLKOUT_LPC0",		"ESPI_CLK",	"n/a",
	"GPP_A10",		"CLKOUT_LPC1",		"n/a",		"n/a",
	"GPP_A11",		"PME#",			"GSPI1_CS1#",	"SD_VDD2_PWR_EN#",
	"GPP_A12",		"BM_BUSY#",		"ISH_GP6",	"SX_EXIT_HOLDOFF#",
	"GPP_A13",		"SUSWARN#/SUSPWRDNACK",	"n/a",		"n/a",
	"GPP_A14",		"SUS_STAT#",		"ESPI_RESET#",	"n/a",
	"GPP_A15",		"SUSACK#",		"n/a",		"n/a",
	"GPP_A16",		"SD_1P8_SEL",		"n/a",		"n/a",
	"GPP_A17",		"SD_VDD1_PWR_EN#",	"ISH_GP7",	"n/a",
	"GPP_A18",		"ISH_GP0",		"n/a",		"n/a",
	"GPP_A19",		"ISH_GP1",		"n/a",		"n/a",
	"GPP_A20",		"ISH_GP2",		"n/a",		"n/a",
	"GPP_A21",		"ISH_GP3",		"n/a",		"n/a",
	"GPP_A22",		"ISH_GP4",		"n/a",		"n/a",
	"GPP_A23",		"ISH_GP5",		"n/a",		"n/a",
	"ESPI_CLK_LOOPBK",	"ESPI_CLK_LOOPBK",	"n/a",		"n/a",
};

const struct gpio_group cannonlake_pch_lp_group_a = {
	.display	= "------- GPIO Group GPP_A -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_lp_group_a_names) / 4,
	.func_count	= 4,
	.pad_names	= cannonlake_pch_lp_group_a_names,
};

const char *const cannonlake_pch_lp_group_b_names[] = {
	"GPP_B0",		"CORE_VID0",		"n/a",
	"GPP_B1",		"CORE_VID1",		"n/a",
	"GPP_B2",		"VRALERT#",		"n/a",
	"GPP_B3",		"CPU_GP2",		"n/a",
	"GPP_B4",		"CPU_GP3",		"n/a",
	"GPP_B5",		"SRCCLKREQ0#",		"n/a",
	"GPP_B6",		"SRCCLKREQ1#",		"n/a",
	"GPP_B7",		"SRCCLKREQ2#",		"n/a",
	"GPP_B8",		"SRCCLKREQ3#",		"n/a",
	"GPP_B9",		"SRCCLKREQ4#",		"n/a",
	"GPP_B10",		"SRCCLKREQ5#",		"n/a",
	"GPP_B11",		"EXT_PWR_GATE#",	"n/a",
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

const struct gpio_group cannonlake_pch_lp_group_b = {
	.display	= "------- GPIO Group GPP_B -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_lp_group_b_names) / 3,
	.func_count	= 3,
	.pad_names	= cannonlake_pch_lp_group_b_names,
};

const char *const cannonlake_pch_lp_group_c_names[] = {
	"GPP_C0",	"SMBCLK",	"n/a",			"n/a",
	"GPP_C1",	"SMBDATA",	"n/a",			"n/a",
	"GPP_C2",	"SMBALERT#",	"n/a",			"n/a",
	"GPP_C3",	"SML0CLK",	"n/a",			"n/a",
	"GPP_C4",	"SML0DATA",	"n/a",			"n/a",
	"GPP_C5",	"SML0ALERT#",	"n/a",			"n/a",
	"GPP_C6",	"SML1CLK",	"n/a",			"n/a",
	"GPP_C7",	"SML1DATA",	"n/a",			"n/a",
	"GPP_C8",	"UART0_RXD",	"n/a",			"n/a",
	"GPP_C9",	"UART0_TXD",	"n/a",			"n/a",
	"GPP_C10",	"UART0_RTS#",	"n/a",			"n/a",
	"GPP_C11",	"UART0_CTS#",	"n/a",			"n/a",
	"GPP_C12",	"UART1_RXD",	"ISH_UART1_RXD",	"CNV_MFUART1_RXD",
	"GPP_C13",	"UART1_TXD",	"ISH_UART1_TXD",	"CNV_MFUART1_TXD",
	"GPP_C14",	"UART1_RTS#",	"ISH_UART1_RTS#",	"CNV_MFUART1_RTS",
	"GPP_C15",	"UART1_CTS#",	"ISH_UART1_CTS#",	"CNV_MFUART1_CTS",
	"GPP_C16",	"I2C0_SDA",	"n/a",			"n/a",
	"GPP_C17",	"I2C0_SCL",	"n/a",			"n/a",
	"GPP_C18",	"I2C1_SDA",	"n/a",			"n/a",
	"GPP_C19",	"I2C1_SCL",	"n/a",			"n/a",
	"GPP_C20",	"UART2_RXD",	"n/a",			"n/a",
	"GPP_C21",	"UART2_TXD",	"n/a",			"n/a",
	"GPP_C22",	"UART2_RTS#",	"n/a",			"n/a",
	"GPP_C23",	"UART2_CTS#",	"n/a",			"n/a",
};

const struct gpio_group cannonlake_pch_lp_group_c = {
	.display	= "------- GPIO Group GPP_C -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_lp_group_c_names) / 4,
	.func_count	= 4,
	.pad_names	= cannonlake_pch_lp_group_c_names,
};

const char *const cannonlake_pch_lp_group_d_names[] = {
	"GPP_D0",		"SPI1_CS#",		"BK0",		"SBK0",
	"GPP_D1",		"SPI1_CLK",		"BK1",		"SBK1",
	"GPP_D2",		"SPI1_MISO",		"BK2",		"SBK2",
	"GPP_D3",		"SPI1_MOSI",		"BK3",		"SBK3",
	"GPP_D4",		"IMGCLKOUT0",		"BK4",		"SBK4",
	"GPP_D5",		"ISH_I2C0_SDA",		"n/a",		"n/a",
	"GPP_D6",		"ISH_I2C0_SCL",		"n/a",		"n/a",
	"GPP_D7",		"ISH_I2C1_SDA",		"n/a",		"n/a",
	"GPP_D8",		"ISH_I2C1_SCL",		"n/a",		"n/a",
	"GPP_D9",		"ISH_SPI_CS#",		"n/a",		"GSPI2_CS0#",
	"GPP_D10",		"ISH_SPI_CLK",		"n/a",		"GSPI2_CLK",
	"GPP_D11",		"ISH_SPI_MISO",		"n/a",		"GSPI2_MISO",
	"GPP_D12",		"ISH_SPI_MOSI",		"n/a",		"GSPI2_MOSI",
	"GPP_D13",		"ISH_UART0_RXD",	"SML0BDATA",	"I2C4B_SDA",
	"GPP_D14",		"ISH_UART0_TXD",	"SML0BCLK",	"I2C4B_SCL",
	"GPP_D15",		"ISH_UART0_RTS#",	"GSPI2_CS1#",	"n/a",
	"GPP_D16",		"ISH_UART0_CTS#",	"SML0BALERT",	"n/a",
	"GPP_D17",		"DMIC_CLK1",		"SNDW3_CLK",	"n/a",
	"GPP_D18",		"DMIC_DATA1",		"SNDW3_DATA",	"n/a",
	"GPP_D19",		"DMIC_CLK0",		"SNDW4_CLK",	"n/a",
	"GPP_D20",		"DMIC_DATA0",		"SNDW4_DATA",	"n/a",
	"GPP_D21",		"SPI1_IO2",		"n/a",		"n/a",
	"GPP_D22",		"SPI1_IO3",		"n/a",		"n/a",
	"GPP_D23",		"I2S_MCLK",		"n/a",		"n/a",
	"GSPI2_CLK_LOOPBK",	"GSPI2_CLK_LOOPBK",	"n/a",		"n/a",
};

const struct gpio_group cannonlake_pch_lp_group_d = {
	.display	= "------- GPIO Group GPP_D -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_lp_group_d_names) / 4,
	.func_count	= 4,
	.pad_names	= cannonlake_pch_lp_group_d_names,
};

const char *const cannonlake_pch_lp_group_e_names[] = {
	"GPP_E0",	"SATAXPCIE0",		"SATAGP0",	"n/a",
	"GPP_E1",	"SATAXPCIE1",		"n/a",		"n/a",
	"GPP_E2",	"SATAXPCIE2",		"n/a",		"n/a",
	"GPP_E3",	"CPU_GP0",		"n/a",		"n/a",
	"GPP_E4",	"SATA_DEVSLP0",		"n/a",		"n/a",
	"GPP_E5",	"SATA_DEVSLP1",		"n/a",		"n/a",
	"GPP_E6",	"SATA_DEVSLP2",		"n/a",		"n/a",
	"GPP_E7",	"CPU_GP1",		"n/a",		"n/a",
	"GPP_E8",	"SATALED#",		"n/a",		"n/a",
	"GPP_E9",	"USB2_OC0#",		"n/a",		"n/a",
	"GPP_E10",	"USB2_OC1#",		"n/a",		"n/a",
	"GPP_E11",	"USB2_OC2#",		"n/a",		"n/a",
	"GPP_E12",	"USB2_OC3#",		"n/a",		"n/a",
	"GPP_E13",	"DDPB_HPD0",		"DISP_MISC0",	"n/a",
	"GPP_E14",	"DDPC_HPD1",		"DISP_MISC1",	"n/a",
	"GPP_E15",	"DDPD_HPD2",		"DISP_MISC2",	"n/a",
	"GPP_E16",	"n/a",			"DISP_MISC3",	"n/a",
	"GPP_E17",	"EDP_HPD",		"DISP_MISC4",	"n/a",
	"GPP_E18",	"DPPB_CTRLCLK",		"n/a",		"CNV_BT_HOST_WAKE#",
	"GPP_E19",	"DPPB_CTRLDATA",	"n/a",		"CNV_BT_IF_SELECT",
	"GPP_E20",	"DPPC_CTRLCLK",		"n/a",		"n/a",
	"GPP_E21",	"DPPC_CTRLDATA",	"n/a",		"n/a",
	"GPP_E22",	"DPPD_CTRLCLK",		"n/a",		"n/a",
	"GPP_E23",	"DPPD_CTRLDATA",	"n/a",		"n/a",
};

const struct gpio_group cannonlake_pch_lp_group_e = {
	.display	= "------- GPIO Group GPP_E -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_lp_group_e_names) / 4,
	.func_count	= 4,
	.pad_names	= cannonlake_pch_lp_group_e_names,
};

const char *const cannonlake_pch_lp_group_f_names[] = {
	"GPP_F0",	"CNV_PA_BLANKING",	"n/a",
	"GPP_F1",	"n/a",			"n/a",
	"GPP_F2",	"n/a",			"n/a",
	"GPP_F3",	"n/a",			"n/a",
	"GPP_F4",	"CNV_BRI_DT",		"UART0_RTS#",
	"GPP_F5",	"CNV_BRI_RSP",		"UART0_RXD",
	"GPP_F6",	"CNV_RGI_DT",		"UART0_TXD",
	"GPP_F7",	"CNV_RGI_RSP",		"UART0_CTS#",
	"GPP_F8",	"CNV_MFUART2_RXD",	"n/a",
	"GPP_F9",	"CNV_MFUART2_TXD",	"n/a",
	"GPP_F10",	"n/a",			"n/a",
	"GPP_F11",	"EMMC_CMD",		"n/a",
	"GPP_F12",	"EMMC_DATA0",		"n/a",
	"GPP_F13",	"EMMC_DATA1",		"n/a",
	"GPP_F14",	"EMMC_DATA2",		"n/a",
	"GPP_F15",	"EMMC_DATA3",		"n/a",
	"GPP_F16",	"EMMC_DATA4",		"n/a",
	"GPP_F17",	"EMMC_DATA5",		"n/a",
	"GPP_F18",	"EMMC_DATA6",		"n/a",
	"GPP_F19",	"EMMC_DATA7",		"n/a",
	"GPP_F20",	"EMMC_RCLK",		"n/a",
	"GPP_F21",	"EMMC_CLK",		"n/a",
	"GPP_F22",	"EMMC_RESET#",		"n/a",
	"GPP_F23",	"A4WP_PRESENT",		"n/a",
};

const struct gpio_group cannonlake_pch_lp_group_f = {
	.display	= "------- GPIO Group GPP_F -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_lp_group_f_names) / 3,
	.func_count	= 3,
	.pad_names	= cannonlake_pch_lp_group_f_names,
};

const char *const cannonlake_pch_lp_group_g_names[] = {
	"GPP_G0",	"SD_CMD",
	"GPP_G1",	"SD_DATA0",
	"GPP_G2",	"SD_DATA1",
	"GPP_G3",	"SD_DATA2",
	"GPP_G4",	"SD_DATA3",
	"GPP_G5",	"SD3_CD#",
	"GPP_G6",	"SD3_CLK",
	"GPP_G7",	"SD3_WP",
};

const struct gpio_group cannonlake_pch_lp_group_g = {
	.display	= "------- GPIO Group GPP_G -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_lp_group_g_names) / 2,
	.func_count	= 2,
	.pad_names	= cannonlake_pch_lp_group_g_names,
};

const char *const cannonlake_pch_lp_group_h_names[] = {
	"GPP_H0",	"I2S2_SCLK",		"CNV_BT_I2S_SCLK",	"n/a",
	"GPP_H1",	"I2S2_SFRM",		"CNV_BT_I2S_BCLK",	"CNV_RF_RESET#",
	"GPP_H2",	"I2S2_TXD",		"CNV_BT_I2S_SDI",	"MODEM_CLKREQ",
	"GPP_H3",	"I2S2_RXD",		"CNV_BT_I2S_SDO",	"n/a",
	"GPP_H4",	"I2C2_SDA",		"n/a",			"n/a",
	"GPP_H5",	"I2C2_SCL",		"n/a",			"n/a",
	"GPP_H6",	"I2C3_SDA",		"n/a",			"n/a",
	"GPP_H7",	"I2C3_SCL",		"n/a",			"n/a",
	"GPP_H8",	"I2C4_SDA",		"n/a",			"n/a",
	"GPP_H9",	"I2C4_SCL",		"n/a",			"n/a",
	"GPP_H10",	"I2C5_SDA",		"ISH_I2C2_SDA",		"n/a",
	"GPP_H11",	"I2C5_SCL",		"ISH_I2C2_SCL",		"n/a",
	"GPP_H12",	"M2_SKT2_CFG0",		"n/a",			"n/a",
	"GPP_H13",	"M2_SKT2_CFG1",		"n/a",			"n/a",
	"GPP_H14",	"M2_SKT2_CFG2",		"n/a",			"n/a",
	"GPP_H15",	"M2_SKT2_CFG3",		"n/a",			"n/a",
	"GPP_H16",	"n/a",			"n/a",			"n/a",
	"GPP_H17",	"n/a",			"n/a",			"n/a",
	"GPP_H18",	"CPU_C10_GATE#",	"n/a",			"n/a",
	"GPP_H19",	"TIME_SYNC0",		"n/a",			"n/a",
	"GPP_H20",	"IMGCLKOUT1",		"n/a",			"n/a",
	"GPP_H21",	"n/a",			"n/a",			"n/a",
	"GPP_H22",	"n/a",			"n/a",			"n/a",
	"GPP_H23",	"n/a",			"n/a",			"n/a",
};

const struct gpio_group cannonlake_pch_lp_group_h = {
	.display	= "------- GPIO Group GPP_H -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_lp_group_h_names) / 4,
	.func_count	= 4,
	.pad_names	= cannonlake_pch_lp_group_h_names,
};

const char *const cannonlake_pch_lp_group_gpd_names[] = {
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
	"SLP_LAN_B",	"SLP_LAN#",
	"SLP_SUS_B",	"SLP_SUS#",
	"WAKE_B",	"WAKE#",
	"DRAM_RESET_B",	"DRAM_RESET#",
};

const struct gpio_group cannonlake_pch_lp_group_gpd = {
	.display	= "------- GPIO Group GPD -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_lp_group_gpd_names) / 2,
	.func_count	= 2,
	.pad_names	= cannonlake_pch_lp_group_gpd_names,
};

const char *const cannonlake_pch_lp_group_vgpio_names[] = {
	"CNV_BTEN",			"n/a",		"n/a",		"n/a",
	"CNV_GNEN",			"n/a",		"n/a",		"n/a",
	"CNV_WFEN",			"n/a",		"n/a",		"n/a",
	"CNV_WCEN",			"n/a",		"n/a",		"n/a",
	"CNV_BT_HOST_WAKE_B",		"n/a",		"n/a",		"n/a",
	"CNV_BT_IF_SELECT",		"n/a",		"n/a",		"n/a",
	"vCNV_BT_UART_TXD",		"ISH UART0",	"SIo UART2",	"n/a",
	"vCNV_BT_UART_RXD",		"ISH UART0",	"SIo UART2",	"n/a",
	"vCNV_BT_UART_CTS_B",		"ISH UART0",	"SIo UART2",	"n/a",
	"vCNV_BT_UART_RTS_B",		"ISH UART0",	"SIo UART2",	"n/a",
	"vCNV_MFUART1_TXD",		"ISH UART0",	"SIo UART2",	"n/a",
	"vCNV_MFUART1_RXD",		"ISH UART0",	"SIo UART2",	"n/a",
	"vCNV_MFUART1_CTS_B",		"ISH UART0",	"SIo UART2",	"n/a",
	"vCNV_MFUART1_RTS_B",		"ISH UART0",	"SIo UART2",	"n/a",
	"vCNV_GNSS_UART_TXD",		"n/a",		"n/a",		"n/a",
	"vCNV_GNSS_UART_RXD",		"n/a",		"n/a",		"n/a",
	"vCNV_GNSS_UART_CTS_B",		"n/a",		"n/a",		"n/a",
	"vCNV_GNSS_UART_RTS_B",		"n/a",		"n/a",		"n/a",
	"vUART0_TXD",			"mapped",	"n/a",		"n/a",
	"vUART0_RXD",			"mapped",	"n/a",		"n/a",
	"vUART0_CTS_B",			"mapped",	"n/a",		"n/a",
	"vUART0_RTS_B",			"mapped",	"n/a",		"n/a",
	"vISH_UART0_TXD",		"mapped",	"n/a",		"n/a",
	"vISH_UART0_RXD", 		"mapped",	"n/a",		"n/a",
	"vISH_UART0_CTS_B",		"mapped",	"n/a",		"n/a",
	"vISH_UART0_RTS_B",		"mapped",	"n/a",		"n/a",
	"vISH_UART1_TXD",		"mapped",	"n/a",		"n/a",
	"vISH_UART1_RXD", 		"mapped",	"n/a",		"n/a",
	"vISH_UART1_CTS_B",		"mapped",	"n/a",		"n/a",
	"vISH_UART1_RTS_B",		"mapped",	"n/a",		"n/a",
	"vCNV_BT_I2S_BCLK",		"SSP0",		"SSP1",		"SSP2",
	"vCNV_BT_I2S_WS_SYNC",		"SSP0",		"SSP1",		"SSP2",
	"vCNV_BT_I2S_SDO",		"SSP0",		"SSP1",		"SSP2",
	"vCNV_BT_I2S_SDI",		"SSP0",		"SSP1",		"SSP2",
	"vSSP2_SCLK",			"mapped",	"n/a",		"n/a",
	"vSSP2_SFRM",			"mapped",	"n/a",		"n/a",
	"vSSP2_TXD",			"mapped",	"n/a",		"n/a",
	"vSSP2_RXD",			"n/a",		"n/a",		"n/a",
	"vCNV_GNSS_HOST_WAKE_B",	"n/a",		"n/a",		"n/a",
	"vSD3_CD_B",			"n/a",		"n/a",		"n/a",
};

const struct gpio_group cannonlake_pch_lp_group_vgpio = {
	.display	= "------- GPIO Group VGPIO -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_lp_group_vgpio_names) / 4,
	.func_count	= 4,
	.pad_names	= cannonlake_pch_lp_group_vgpio_names,
};

const char *const cannonlake_pch_lp_group_spi_names[] = {
	"SPI0_IO_2",       	"SPI0_IO_2",
	"SPI0_IO_3",       	"SPI0_IO_3",
	"SPI0_MISO",		"SPI0_MISO",
	"SPI0_MOSI",		"SPI0_MOSI",
	"SPI0_CS2_B",    	"SPI0_CS2#",
	"SPI0_CS0_B",		"SPI0_CS0#",
	"SPI0_CS1_B",		"SPI0_CS1#",
	"SPI0_CLK",        	"SPI0_CLK",
	"SPI0_CLK_LOOPBK", 	"SPI0_CLK_LOOPBK",
};

const struct gpio_group cannonlake_pch_lp_group_spi = {
	.display	= "------- GPIO Group SPI -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_lp_group_spi_names) / 2,
	.func_count	= 2,
	.pad_names	= cannonlake_pch_lp_group_spi_names,
};

const char *const cannonlake_pch_lp_group_aza_names[] = {
	"HDA_BCLK",	"HDA_BCLK",	"I2S0_SCLK",	"n/a",
	"HDA_RST_B",	"HDA_RST#",	"I2S1_SCLK",	"SNDW1_CLK",
	"HDA_SYNC",	"HDA_SYNC",	"I2S0_SFRM",	"n/a",
	"HDA_SDO",	"HDA_SDO",	"I2S0_TXD",	"n/a",
	"HDA_SDI0",	"HDA_SDI0",	"I2S0_RXD",	"n/a",
	"HDA_SDI1",	"HDA_SDI1",	"I2S1_RXD",	"SNDW1_DATA",
	"I2S1_SFRM",	"I2S1_SFRM",	"SNDW2_CLK",	"n/a",
	"I2S1_TXD",	"I2S1_TXD",	"SNDW2_DATA",	"n/a",
};

const struct gpio_group cannonlake_pch_lp_group_aza = {
	.display	= "------- GPIO Group AZA -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_lp_group_aza_names) / 4,
	.func_count	= 4,
	.pad_names	= cannonlake_pch_lp_group_aza_names,
};

const char *const cannonlake_pch_lp_group_cpu_names[] = {
	"HDACPU_SDI", 	"HDACPU_SDI",
	"HDACPU_SDO", 	"HDACPU_SDO",
	"HDACPU_SCLK",	"HDACPU_SCLK",
	"PM_SYNC",    	"PM_SYNC",
	"PECI",       	"PECI",
	"CPUPWRGD",   	"CPUPWRGD",
	"THRMTRIP_B",  	"THRMTRIP#",
	"PLTRST_CPU_B",	"PLTRST_CPU#",
	"PM_DOWN",    	"PM_DOWN",
	"TRIGGER_IN", 	"TRIGGER_IN",
	"TRIGGER_OUT",	"TRIGGER_OUT",
};

const struct gpio_group cannonlake_pch_lp_group_cpu = {
	.display	= "------- GPIO Group CPU -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_lp_group_cpu_names) / 2,
	.func_count	= 2,
	.pad_names	= cannonlake_pch_lp_group_cpu_names,
};

const char *const cannonlake_pch_lp_group_jtag_names[] = {
	"PCH_TDO", 	"PCH_TDO",
	"PCH_JTAGX",    "PCH_JTAGX",
	"PROC_PRDY_B",  "PROC_PRDY#",
	"PROC_PREQ_B",  "PROC_PREQ#",
	"CPU_TRST_B",	"CPU_TRST#",
	"PCH_TDI", 	"PCH_TDI",
	"PCH_TMS", 	"PCH_TMS",
	"PCH_TCK", 	"PCH_TCK",
	"ITP_PMODE",	"ITP_PMODE",
};

const struct gpio_group cannonlake_pch_lp_group_jtag = {
	.display	= "------- GPIO Group JTAG -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_lp_group_jtag_names) / 2,
	.func_count	= 2,
	.pad_names	= cannonlake_pch_lp_group_jtag_names,
};

const char *const cannonlake_pch_lp_group_hvmos_names[] = {
	"EDP_VDDEN",	"EDP_VDDEN",
	"EDP_BKLTEN",	"EDP_BKLTEN",
	"EDP_BKLTCTL",	"EDP_BKLTCTL",
	"SYS_PWROK",	"SYS_PWROK",
	"SYS_RESET_B",	"SYS_RESET#",
	"CL_RST_B",	"CL_RST#",
};

const struct gpio_group cannonlake_pch_lp_group_hvmos = {
	.display	= "------- GPIO Group HVMOS -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_lp_group_hvmos_names) / 2,
	.func_count	= 2,
	.pad_names	= cannonlake_pch_lp_group_hvmos_names,
};

const struct gpio_group *const cannonlake_pch_lp_community_0_groups[] = {
	&cannonlake_pch_lp_group_a,
	&cannonlake_pch_lp_group_b,
	&cannonlake_pch_lp_group_g,
	&cannonlake_pch_lp_group_spi,
};

const struct gpio_community cannonlake_pch_lp_community_0 = {
	.name		= "------- GPIO Community 0 -------",
	.pcr_port_id	= 0x6e,
	.group_count	= ARRAY_SIZE(cannonlake_pch_lp_community_0_groups),
	.groups		= cannonlake_pch_lp_community_0_groups,
};

const struct gpio_group *const cannonlake_pch_lp_community_1_groups[] = {
	&cannonlake_pch_lp_group_d,
	&cannonlake_pch_lp_group_f,
	&cannonlake_pch_lp_group_h,
	&cannonlake_pch_lp_group_vgpio,
};
const struct gpio_community cannonlake_pch_lp_community_1 = {
	.name		= "------- GPIO Community 1 -------",
	.pcr_port_id	= 0x6d,
	.group_count	= ARRAY_SIZE(cannonlake_pch_lp_community_1_groups),
	.groups		= cannonlake_pch_lp_community_1_groups,
};

const struct gpio_group *const cannonlake_pch_lp_community_2_groups[] = {
	&cannonlake_pch_lp_group_gpd,
};

const struct gpio_community cannonlake_pch_lp_community_2 = {
	.name		= "------- GPIO Community 2 -------",
	.pcr_port_id	= 0x6c,
	.group_count	= ARRAY_SIZE(cannonlake_pch_lp_community_2_groups),
	.groups		= cannonlake_pch_lp_community_2_groups,
};

const struct gpio_group *const cannonlake_pch_lp_community_3_groups[] = {
	&cannonlake_pch_lp_group_aza,
	&cannonlake_pch_lp_group_cpu,
};

const struct gpio_community cannonlake_pch_lp_community_3 = {
	.name		= "------- GPIO Community 3 -------",
	.pcr_port_id	= 0x6b,
	.group_count	= ARRAY_SIZE(cannonlake_pch_lp_community_3_groups),
	.groups		= cannonlake_pch_lp_community_3_groups,
};

const struct gpio_group *const cannonlake_pch_lp_community_4_groups[] = {
	&cannonlake_pch_lp_group_c,
	&cannonlake_pch_lp_group_e,
	&cannonlake_pch_lp_group_jtag,
	&cannonlake_pch_lp_group_hvmos,
};

const struct gpio_community cannonlake_pch_lp_community_4 = {
	.name		= "------- GPIO Community 4 -------",
	.pcr_port_id	= 0x6a,
	.group_count	= ARRAY_SIZE(cannonlake_pch_lp_community_4_groups),
	.groups		= cannonlake_pch_lp_community_4_groups,
};

const struct gpio_community *const cannonlake_pch_lp_communities[] = {
	&cannonlake_pch_lp_community_0,
	&cannonlake_pch_lp_community_1,
	&cannonlake_pch_lp_community_2,
	&cannonlake_pch_lp_community_3,
	&cannonlake_pch_lp_community_4,
};

#endif
