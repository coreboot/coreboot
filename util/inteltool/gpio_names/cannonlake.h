#ifndef GPIO_NAMES_CANNONLAKE_H
#define GPIO_NAMES_CANNONLAKE_H

#include "gpio_groups.h"

static const char *const cannonlake_pch_h_group_a_names[] = {
	"GPP_A0",		"RCIN#",		"n/a",			"ESPI_ALERT1#",
	"GPP_A1",		"LAD0",			"n/a",			"ESPI_IO0",
	"GPP_A2",		"LAD1",			"n/a",			"ESPI_IO1",
	"GPP_A3",		"LAD2",			"n/a",			"ESPI_IO2",
	"GPP_A4",		"LAD3",			"n/a",			"ESPI_IO3",
	"GPP_A5",		"LFRAME#",		"n/a",			"ESPI_CS0#",
	"GPP_A6",		"SERIRQ",		"n/a",			"ESPI_CS1#",
	"GPP_A7",		"PIRQA#",		"n/a",			"ESPI_ALERT0#",
	"GPP_A8",		"CLKRUN#",		"n/a",			"n/a",
	"GPP_A9",		"CLKOUT_LPC0",		"n/a",			"ESPI_CLK",
	"GPP_A10",		"CLKOUT_LPC1",		"n/a",			"n/a",
	"GPP_A11",		"PME#",			"SD_VDD2_PWR_EN#",	"n/a",
	"GPP_A12",		"BM_BUSY#",		"ISH_GP6",		"SX_EXIT_HOLDOFF#",
	"GPP_A13",		"SUSWARN#/SUSPWRDNACK",	"n/a",			"n/a",
	"GPP_A14",		"SUS_STAT#",		"n/a",			"ESPI_RESET#",
	"GPP_A15",		"SUSACK#",		"n/a",			"n/a",
	"GPP_A16",		"CLKOUT_48",		"n/a",			"n/a",
	"GPP_A17",		"SD_VDD1_PWR_EN#",	"ISH_GP7",		"n/a",
	"GPP_A18",		"ISH_GP0",		"n/a",			"n/a",
	"GPP_A19",		"ISH_GP1",		"n/a",			"n/a",
	"GPP_A20",		"ISH_GP2",		"n/a",			"n/a",
	"GPP_A21",		"ISH_GP3",		"n/a",			"n/a",
	"GPP_A22",		"ISH_GP4",		"n/a",			"n/a",
	"GPP_A23",		"ISH_GP5",		"n/a",			"n/a",
	"ESPI_CLK_LOOPBK",	"ESPI_CLK_LOOPBK",	"n/a",			"n/a",
};

static const char *const cannonlake_pch_h_group_b_names[] = {
	"GPP_B0",		"GSPI0_CS1#",		"n/a",
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

static const char *const cannonlake_pch_h_group_c_names[] = {
	"GPP_C0",	"SMBCLK",	"n/a",
	"GPP_C1",	"SMBDATA",	"n/a",
	"GPP_C2",	"SMBALERT#",	"n/a",
	"GPP_C3",	"SML0CLK",	"n/a",
	"GPP_C4",	"SML0DATA",	"n/a",
	"GPP_C5",	"SML0ALERT#",	"n/a",
	"GPP_C6",	"SML1CLK",	"n/a",
	"GPP_C7",	"SML1DATA",	"n/a",
	"GPP_C8",	"UART0A_RXD",	"n/a",
	"GPP_C9",	"UART0A_TXD",	"n/a",
	"GPP_C10",	"UART0A_RTS#",	"n/a",
	"GPP_C11",	"UART0A_CTS#",	"n/a",
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

static const char *const cannonlake_pch_h_group_d_names[] = {
	"GPP_D0",	"SPI1_CS#",		"n/a",		"SBK0",			"BK0",
	"GPP_D1",	"SPI1_CLK",		"n/a",		"SBK1",			"BK1",
	"GPP_D2",	"SPI1_MISO",		"n/a",		"SBK2",			"BK2",
	"GPP_D3",	"SPI1_MOSI",		"n/a",		"SBK3",			"BK3",
	"GPP_D4",	"I2C2_SDA",		"I2C3_SDA",	"SBK4",			"BK4",
	"GPP_D5",	"I2S2_SFRM",		"n/a",		"CNV_RF_RESET#",	"n/a",
	"GPP_D6",	"I2S2_TXD",		"n/a",		"MODEM_CLKREQ",		"n/a",
	"GPP_D7",	"I2S2_RXD",		"n/a",		"n/a",			"n/a",
	"GPP_D8",	"I2S2_SCLK",		"n/a",		"n/a",			"n/a",
	"GPP_D9",	"ISH_SPI_CS#",		"n/a",		"GSPI2_CS0#",		"n/a",
	"GPP_D10",	"ISH_SPI_CLK",		"n/a",		"GSPI2_CLK",		"n/a",
	"GPP_D11",	"ISH_SPI_MISO",		"GP_BSSB_CLK",	"GSPI2_MISO",		"n/a",
	"GPP_D12",	"ISH_SPI_MOSI",		"GP_BSSB_DI",	"GSPI2_MOSI",		"n/a",
	"GPP_D13",	"ISH_UART0_RXD",	"n/a",		"I2C2_SDA",		"n/a",
	"GPP_D14",	"ISH_UART0_TXD",	"n/a",		"I2C2_SCL",		"n/a",
	"GPP_D15",	"ISH_UART0_RTS#",	"GSPI2_CS1#",	"n/a",			"CNV_WFEN",
	"GPP_D16",	"ISH_UART0_CTS#",	"n/a",		"n/a",			"CNV_WCEN",
	"GPP_D17",	"DMIC_CLK1",		"SNDW3_CLK",	"n/a",			"n/a",
	"GPP_D18",	"DMIC_DATA1",		"SNDW3_DATA",	"n/a",			"n/a",
	"GPP_D19",	"DMIC_CLK0",		"SNDW4_CLK",	"n/a",			"n/a",
	"GPP_D20",	"DMIC_DATA0",		"SNDW4_DATA",	"n/a",			"n/a",
	"GPP_D21",	"SPI1_IO2",		"n/a",		"n/a",			"n/a",
	"GPP_D22",	"SPI1_IO3",		"n/a",		"n/a",			"n/a",
	"GPP_D23",	"ISH_I2C2_SCL",		"I2C3_SCL",	"n/a",			"n/a",
};

static const char *const cannonlake_pch_h_group_e_names[] = {
	"GPP_E0",	"SATAXPCIE0",	"SATAGP0",
	"GPP_E1",	"SATAXPCIE1",	"SATAGP1",
	"GPP_E2",	"SATAXPCIE2",	"SATAGP2",
	"GPP_E3",	"CPU_GP0",	"n/a",
	"GPP_E4",	"SATA_DEVSLP0",	"n/a",
	"GPP_E5",	"SATA_DEVSLP1",	"n/a",
	"GPP_E6",	"SATA_DEVSLP2",	"n/a",
	"GPP_E7",	"CPU_GP1",	"n/a",
	"GPP_E8",	"SATALED#",	"n/a",
	"GPP_E9",	"USB2_OC0#",	"n/a",
	"GPP_E10",	"USB2_OC1#",	"n/a",
	"GPP_E11",	"USB2_OC2#",	"n/a",
	"GPP_E12",	"USB2_OC3#",	"n/a",
};

static const char *const cannonlake_pch_h_group_f_names[] = {
	"GPP_F0",	"SATAXPCIE3",		"SATAGP3",
	"GPP_F1",	"SATAXPCIE4",		"SATAGP4",
	"GPP_F2",	"SATAXPCIE5",		"SATAGP5",
	"GPP_F3",	"SATAXPCIE6",		"SATAGP6",
	"GPP_F4",	"SATAXPCIE7",		"SATAGP7",
	"GPP_F5",	"SATA_DEVSLP3",		"n/a",
	"GPP_F6",	"SATA_DEVSLP4",		"n/a",
	"GPP_F7",	"SATA_DEVSLP5",		"n/a",
	"GPP_F8",	"SATA_DEVSLP6",		"n/a",
	"GPP_F9",	"SATA_DEVSLP7",		"n/a",
	"GPP_F10",	"SATA_SCLOCK",		"n/a",
	"GPP_F11",	"SATA_SLOAD",		"n/a",
	"GPP_F12",	"SATA_SDATAOUT1",	"n/a",
	"GPP_F13",	"SATA_SDATAOUT0",	"n/a",
	"GPP_F14",	"n/a",			"PS_ON#",
	"GPP_F15",	"USB2_OC4#",		"n/a",
	"GPP_F16",	"USB2_OC5#",		"n/a",
	"GPP_F17",	"USB2_OC6#",		"n/a",
	"GPP_F18",	"USB2_OC7#",		"n/a",
	"GPP_F19",	"eDP_VDDEN",		"n/a",
	"GPP_F20",	"eDP_BKLTEN",		"n/a",
	"GPP_F21",	"eDP_BKLTCTL",		"n/a",
	"GPP_F22",	"DDPF_CTRLCLK",		"n/a",
	"GPP_F23",	"DDPF_CTRLDATA",	"n/a",
};

static const char *const cannonlake_pch_h_group_spi_names[] = {
	"SPI0_IO_2",		"SPI0_IO_2",
	"SPI0_IO_3",		"SPI0_IO_3",
	"SPI0_MISO",		"SPI0_MISO",
	"SPI0_MOSI",		"SPI0_MOSI",
	"SPI0_CS2_B",		"SPI0_CS2#",
	"SPI0_CS0_B",		"SPI0_CS0#",
	"SPI0_CS1_B",		"SPI0_CS1#",
	"SPI0_CLK",		"SPI0_CLK",
	"SPI0_CLK_LOOPBK",	"SPI0_CLK_LOOPBK",
};

static const char *const cannonlake_pch_h_group_g_names[] = {
	"GPP_G0",	"SD_CMD",
	"GPP_G1",	"SD_DATA0",
	"GPP_G2",	"SD_DATA1",
	"GPP_G3",	"SD_DATA2",
	"GPP_G4",	"SD_DATA3",
	"GPP_G5",	"SD_CD#",
	"GPP_G6",	"SD_CLK",
	"GPP_G7",	"SD_WP",
};

static const char *const cannonlake_pch_h_group_aza_names[] = {
	"HDA_BCLK",	"HDA_BCLK",	"I2S0_SCLK",	"n/a",
	"HDA_RST_B",	"HDA_RST#",	"I2S1_SCLK",	"SNDW1_CLK",
	"HDA_SYNC",	"HDA_SYNC",	"I2S0_SFRM",	"n/a",
	"HDA_SDO",	"HDA_SDO",	"I2S0_TXD",	"n/a",
	"HDA_SDI0",	"HDA_SDI0",	"I2S0_RXD",	"n/a",
	"HDA_SDI1",	"HDA_SDI1",	"I2S1_RXD",	"SNDW1_DATA",
	"I2S1_SFRM",	"I2S1_SFRM",	"SNDW2_CLK",	"n/a",
	"I2S1_TXD",	"I2S1_TXD",	"SNDW2_DATA",	"n/a",
};

static const char *const cannonlake_pch_h_group_vgpio_0_names[] = {
	"CNV_BTEN",			"n/a",		"n/a",		"n/a",
	"CNV_GNEN",			"n/a",		"n/a",		"n/a",
	"CNV_WFEN",			"n/a",		"n/a",		"n/a",
	"CNV_WCEN",			"n/a",		"n/a",		"n/a",
	"vCNV_GNSS_HOST_WAKE_B",	"n/a",		"n/a",		"n/a",
	"vSD3_CD_B",			"n/a",		"n/a",		"n/a",
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
};

static const char *const cannonlake_pch_h_group_vgpio_1_names[] = {
	"vCNV_BT_I2S_BCLK",		"SSP0",		"SSP1",		"SSP2",
	"vCNV_BT_I2S_WS_SYNC",		"SSP0",		"SSP1",		"SSP2",
	"vCNV_BT_I2S_SDO",		"SSP0",		"SSP1",		"SSP2",
	"vCNV_BT_I2S_SDI",		"SSP0",		"SSP1",		"SSP2",
	"vSSP2_SCLK",			"mapped",	"n/a",		"n/a",
	"vSSP2_SFRM",			"mapped",	"n/a",		"n/a",
	"vSSP2_TXD",			"mapped",	"n/a",		"n/a",
	"vSSP2_RXD",			"n/a",		"n/a",		"n/a",
};

static const char *const cannonlake_pch_h_group_h_names[] = {
	"GPP_H0",	"SRCCLKREQ6#",
	"GPP_H1",	"SRCCLKREQ7#",
	"GPP_H2",	"SRCCLKREQ8#",
	"GPP_H3",	"SRCCLKREQ9#",
	"GPP_H4",	"SRCCLKREQ10#",
	"GPP_H5",	"SRCCLKREQ11#",
	"GPP_H6",	"SRCCLKREQ12#",
	"GPP_H7",	"SRCCLKREQ13#",
	"GPP_H8",	"SRCCLKREQ14#",
	"GPP_H9",	"SRCCLKREQ15#",
	"GPP_H10",	"SML2CLK",
	"GPP_H11",	"SML2DATA",
	"GPP_H12",	"SML2ALERT#",
	"GPP_H13",	"SML3CLK",
	"GPP_H14",	"SML3DATA",
	"GPP_H15",	"SML3ALERT#",
	"GPP_H16",	"SML4CLK",
	"GPP_H17",	"SML4DATA",
	"GPP_H18",	"SML4ALERT#",
	"GPP_H19",	"ISH_I2C0_SDA",
	"GPP_H20",	"ISH_I2C0_SCL",
	"GPP_H21",	"ISH_I2C1_SDA",
	"GPP_H22",	"ISH_I2C1_SCL",
	"GPP_H23",	"TIME_SYNC0",
};

const char *const cannonlake_pch_h_group_cpu_names[] = {
	"HDACPU_SDI",	"HDACPU_SDI",
	"HDACPU_SDO",	"HDACPU_SDO",
	"HDACPU_SCLK",	"HDACPU_SCLK",
	"PM_SYNC",	"PM_SYNC",
	"PECI",		"PECI",
	"CPUPWRGD",	"CPUPWRG#",
	"THRMTRIP_B",	"THRMTRIP#",
	"PLTRST_CPU_B",	"PLTRST_CPU#",
	"PM_DOWN",	"PM_DOWN",
	"TRIGGER_IN",	"TRIGGER_IN",
	"TRIGGER_OUT",	"TRIGGER_OUT",
};

const char *const cannonlake_pch_h_group_jtag_names[] = {
	"PCH_TDO",	"PCH_TDO",
	"PCH_JTAGX",	"PCH_JTAGX",
	"PROC_PRDY_B",	"PROC_RDY#",
	"PROC_PREQ_B",	"PROC_REQ#",
	"CPU_TRST_B",	"CPU_TRST#",
	"PCH_TDI", 	"PCH_TDI",
	"PCH_TMS", 	"PCH_TMS",
	"PCH_TCK", 	"PCH_TCK",
	"ITP_PMODE",	"ITP_PMODE",
};

static const char *const cannonlake_pch_h_group_i_names[] = {
	"GPP_I0",	"DDPB_HPD0",		"DISP_MISC0",
	"GPP_I1",	"DDPB_HPD1",		"DISP_MISC1",
	"GPP_I2",	"DDPB_HPD2",		"DISP_MISC2",
	"GPP_I3",	"DDPB_HPD3",		"DISP_MISC3",
	"GPP_I4",	"EDP_HPD",		"DISP_MISC4",
	"GPP_I5",	"DDPB_CTRLCLK",		"n/a",
	"GPP_I6",	"DDPB_CTRLDATA",	"n/a",
	"GPP_I7",	"DDPC_CTRLCLK",		"n/a",
	"GPP_I8",	"DDPC_CTRLDATA",	"n/a",
	"GPP_I9",	"DDPD_CTRLCLK",		"n/a",
	"GPP_I10",	"DDPD_CTRLDATA",	"n/a",
	"GPP_I11",	"M2_SKT2_CFG0",		"n/a",
	"GPP_I12",	"M2_SKT2_CFG1",		"n/a",
	"GPP_I13",	"M2_SKT2_CFG2",		"n/a",
	"GPP_I14",	"M2_SKT2_CFG3",		"n/a",
	"SYS_PWROK",	"SYS_PWROK",		"n/a",
	"SYS_RESET_B",	"SYS_RESET#",		"n/a",
	"CL_RST_B", 	"CL_RST#",		"n/a",
};

static const char *const cannonlake_pch_h_group_j_names[] = {
	"GPP_J0",	"CNV_PA_BLANKING",	"n/a",
	"GPP_J1",	"n/a",			"CPU_C10_GATE#",
	"GPP_J2",	"n/a",			"n/a",
	"GPP_J3",	"n/a",			"n/a",
	"GPP_J4",	"CNV_BRI_DT",		"UART0B_RTS#",
	"GPP_J5",	"CNV_BRI_RSP",		"UART0B_RXD",
	"GPP_J6",	"CNV_RGI_DT",		"UART0B_TXD",
	"GPP_J7",	"CNV_RGI_RSP",		"UART0B_CTS#",
	"GPP_J8",	"CNV_MFUART2_RXD",	"n/a",
	"GPP_J9",	"CNV_MFUART2_TXD",	"n/a",
	"GPP_J10",	"n/a",			"n/a",
	"GPP_J11",	"A4WP_PRESENT",		"n/a",
};

static const char *const cannonlake_pch_h_group_k_names[] = {
	"GPP_K0",	"n/a",
	"GPP_K1",	"n/a",
	"GPP_K2",	"n/a",
	"GPP_K3",	"n/a",
	"GPP_K4",	"n/a",
	"GPP_K5",	"n/a",
	"GPP_K6",	"n/a",
	"GPP_K7",	"n/a",
	"GPP_K8",	"Reserved",
	"GPP_K9",	"Reserved",
	"GPP_K10",	"Reserved",
	"GPP_K11",	"Reserved",
	"GPP_K12",	"GSXOUT",
	"GPP_K13",	"GSXSLOAD",
	"GPP_K14",	"GSXDIN",
	"GPP_K15",	"GSXSRESET#",
	"GPP_K16",	"GSXCLK",
	"GPP_K17",	"ADR_COMPLETE",
	"GPP_K18",	"NMI#",
	"GPP_K19",	"SMI#",
	"GPP_K20",	"Reserved",
	"GPP_K21",	"Reserved",
	"GPP_K22",	"IMGCLKOUT0",
	"GPP_K23",	"IMGCLKOUT1",
};

static const char *const cannonlake_pch_h_group_gpd_names[] = {
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

static const struct gpio_group cannonlake_pch_h_group_a = {
	.display	= "------- GPIO Group GPP_A -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_h_group_a_names) / 4,
	.func_count	= 4,
	.pad_names	= cannonlake_pch_h_group_a_names,
};

static const struct gpio_group cannonlake_pch_h_group_b = {
	.display	= "------- GPIO Group GPP_B -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_h_group_b_names) / 3,
	.func_count	= 3,
	.pad_names	= cannonlake_pch_h_group_b_names,
};

static const struct gpio_group cannonlake_pch_h_group_c = {
	.display	= "------- GPIO Group GPP_C -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_h_group_c_names) / 3,
	.func_count	= 3,
	.pad_names	= cannonlake_pch_h_group_c_names,
};

static const struct gpio_group cannonlake_pch_h_group_d = {
	.display	= "------- GPIO Group GPP_D -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_h_group_d_names) / 5,
	.func_count	= 5,
	.pad_names	= cannonlake_pch_h_group_d_names,
};

static const struct gpio_group cannonlake_pch_h_group_e = {
	.display	= "------- GPIO Group GPP_E -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_h_group_e_names) / 3,
	.func_count	= 3,
	.pad_names	= cannonlake_pch_h_group_e_names,
};

static const struct gpio_group cannonlake_pch_h_group_f = {
	.display	= "------- GPIO Group GPP_F -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_h_group_f_names) / 3,
	.func_count	= 3,
	.pad_names	= cannonlake_pch_h_group_f_names,
};

static const struct gpio_group cannonlake_pch_h_group_spi = {
	.display	= "------- GPIO Group SPI -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_h_group_spi_names) / 2,
	.func_count	= 2,
	.pad_names	= cannonlake_pch_h_group_spi_names,
};

static const struct gpio_group cannonlake_pch_h_group_g = {
	.display	= "------- GPIO Group GPP_G -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_h_group_g_names) / 2,
	.func_count	= 2,
	.pad_names	= cannonlake_pch_h_group_g_names,
};

static const struct gpio_group cannonlake_pch_h_group_aza = {
	.display	= "------- GPIO Group AZA -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_h_group_aza_names) / 4,
	.func_count	= 4,
	.pad_names	= cannonlake_pch_h_group_aza_names,
};

static const struct gpio_group cannonlake_pch_h_group_vgpio_0 = {
	.display	= "------- GPIO Group VGPIO_0 -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_h_group_vgpio_0_names) / 4,
	.func_count	= 4,
	.pad_names	= cannonlake_pch_h_group_vgpio_0_names,
};

static const struct gpio_group cannonlake_pch_h_group_vgpio_1 = {
	.display	= "------- GPIO Group VGPIO_1 -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_h_group_vgpio_1_names) / 4,
	.func_count	= 4,
	.pad_names	= cannonlake_pch_h_group_vgpio_1_names,
};

static const struct gpio_group cannonlake_pch_h_group_h = {
	.display	= "------- GPIO Group GPP_H -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_h_group_h_names) / 2,
	.func_count	= 2,
	.pad_names	= cannonlake_pch_h_group_h_names,
};

static const struct gpio_group cannonlake_pch_h_group_cpu = {
	.display	= "------- GPIO Group CPU -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_h_group_cpu_names) / 2,
	.func_count	= 2,
	.pad_names	= cannonlake_pch_h_group_cpu_names,
};

static const struct gpio_group cannonlake_pch_h_group_jtag = {
	.display	= "------- GPIO Group JTAG -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_h_group_jtag_names) / 2,
	.func_count	= 2,
	.pad_names	= cannonlake_pch_h_group_jtag_names,
};

static const struct gpio_group cannonlake_pch_h_group_i = {
	.display	= "------- GPIO Group GPP_I -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_h_group_i_names) / 3,
	.func_count	= 3,
	.pad_names	= cannonlake_pch_h_group_i_names,
};

static const struct gpio_group cannonlake_pch_h_group_j = {
	.display	= "------- GPIO Group GPP_J -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_h_group_j_names) / 3,
	.func_count	= 3,
	.pad_names	= cannonlake_pch_h_group_j_names,
};

static const struct gpio_group cannonlake_pch_h_group_k = {
	.display	= "------- GPIO Group GPP_K -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_h_group_k_names) / 2,
	.func_count	= 2,
	.pad_names	= cannonlake_pch_h_group_k_names,
};

static const struct gpio_group cannonlake_pch_h_group_gpd = {
	.display	= "------- GPIO Group GPD -------",
	.pad_count	= ARRAY_SIZE(cannonlake_pch_h_group_gpd_names) / 2,
	.func_count	= 2,
	.pad_names	= cannonlake_pch_h_group_gpd_names,
};

static const struct gpio_group *const cannonlake_pch_h_community_0_groups[] = {
	&cannonlake_pch_h_group_a,
	&cannonlake_pch_h_group_b,
};
static	const struct gpio_community cannonlake_pch_h_community_0 = {
	.name		= "------- GPIO Community 0 -------",
	.pcr_port_id	= 0x6e,
	.group_count	= ARRAY_SIZE(cannonlake_pch_h_community_0_groups),
	.groups		= cannonlake_pch_h_community_0_groups,
};

static const struct gpio_group *const cannonlake_pch_h_community_1_groups[] = {
	&cannonlake_pch_h_group_c,
	&cannonlake_pch_h_group_d,
	&cannonlake_pch_h_group_g,
	&cannonlake_pch_h_group_aza,
	&cannonlake_pch_h_group_vgpio_0,
	&cannonlake_pch_h_group_vgpio_1,
};
static	const struct gpio_community cannonlake_pch_h_community_1 = {
	.name		= "------- GPIO Community 1 -------",
	.pcr_port_id	= 0x6d,
	.group_count	= ARRAY_SIZE(cannonlake_pch_h_community_1_groups),
	.groups		= cannonlake_pch_h_community_1_groups,
};

static const struct gpio_group *const cannonlake_pch_h_community_2_groups[] = {
	&cannonlake_pch_h_group_gpd,
};
static	const struct gpio_community cannonlake_pch_h_community_2 = {
	.name		= "------- GPIO Community 2 -------",
	.pcr_port_id	= 0x6c,
	.group_count	= ARRAY_SIZE(cannonlake_pch_h_community_2_groups),
	.groups		= cannonlake_pch_h_community_2_groups,
};

static const struct gpio_group *const cannonlake_pch_h_community_3_groups[] = {
	&cannonlake_pch_h_group_k,
	&cannonlake_pch_h_group_h,
	&cannonlake_pch_h_group_e,
	&cannonlake_pch_h_group_f,
	&cannonlake_pch_h_group_spi,
};
static	const struct gpio_community cannonlake_pch_h_community_3 = {
	.name		= "------- GPIO Community 3 -------",
	.pcr_port_id	= 0x6b,
	.group_count	= ARRAY_SIZE(cannonlake_pch_h_community_3_groups),
	.groups		= cannonlake_pch_h_community_3_groups,
};

static const struct gpio_group *const cannonlake_pch_h_community_4_groups[] = {
	&cannonlake_pch_h_group_cpu,
	&cannonlake_pch_h_group_jtag,
	&cannonlake_pch_h_group_i,
	&cannonlake_pch_h_group_j,
};
static	const struct gpio_community cannonlake_pch_h_community_4 = {
	.name		= "------- GPIO Community 4 -------",
	.pcr_port_id	= 0x6a,
	.group_count	= ARRAY_SIZE(cannonlake_pch_h_community_4_groups),
	.groups		= cannonlake_pch_h_community_4_groups,
};

static const struct gpio_community *const cannonlake_pch_h_communities[] = {
	&cannonlake_pch_h_community_0,
	&cannonlake_pch_h_community_1,
	&cannonlake_pch_h_community_2,
	&cannonlake_pch_h_community_3,
	&cannonlake_pch_h_community_4,
};

#endif
