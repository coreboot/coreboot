#ifndef GPIO_NAMES_SUNRISE_H
#define GPIO_NAMES_SUNRISE_H

#include "gpio_groups.h"

static const char *const sunrise_group_a_names[] = {
	"GPP_A0",	"RCIN#",		"n/a",		"ESPI_ALERT1#",
	"GPP_A1",	"LAD0",			"n/a",		"ESPI_IO0",
	"GPP_A2",	"LAD1",			"n/a",		"ESPI_IO1",
	"GPP_A3",	"LAD2",			"n/a",		"ESPI_IO2",
	"GPP_A4",	"LAD3",			"n/a",		"ESPI_IO3",
	"GPP_A5",	"LFRAME#",		"n/a",		"ESPI_CS#",
	"GPP_A6",	"SERIRQ",		"n/a",		"ESPI_CS1#",
	"GPP_A7",	"PIRQA#",		"n/a",		"ESPI_ALERT0#",
	"GPP_A8",	"CLKRUN#",		"n/a",		"n/a",
	"GPP_A9",	"CLKOUT_LPC0",		"n/a",		"ESPI_CLK",
	"GPP_A10",	"CLKOUT_LPC1",		"n/a",		"n/a",
	"GPP_A11",	"PME#",			"n/a",		"n/a",
	"GPP_A12",	"BM_BUSY#",		"ISH_GP6",	"SX_EXIT_HOLDOFF#",
	"GPP_A13",	"SUSWARN#/SUSPWRDNACK",	"n/a",		"n/a",
	"GPP_A14",	"SUS_STAT#",		"n/a",		"ESPI_RESET#",
	"GPP_A15",	"SUS_ACK#",		"n/a",		"n/a",
	"GPP_A16",	"CLKOUT_48",		"n/a",		"n/a",
	"GPP_A17",	"ISH_GP7",		"n/a",		"n/a",
	"GPP_A18",	"ISH_GP0",		"n/a",		"n/a",
	"GPP_A19",	"ISH_GP1",		"n/a",		"n/a",
	"GPP_A20",	"ISH_GP2",		"n/a",		"n/a",
	"GPP_A21",	"ISH_GP3",		"n/a",		"n/a",
	"GPP_A22",	"ISH_GP4",		"n/a",		"n/a",
	"GPP_A23",	"ISH_GP5",		"n/a",		"n/a",
};

static const char *const sunrise_lp_group_a_names[] = {
	"GPP_A0",	"RCIN#",		"n/a",		"n/a",
	"GPP_A1",	"LAD0",			"n/a",		"ESPI_IO0",
	"GPP_A2",	"LAD1",			"n/a",		"ESPI_IO1",
	"GPP_A3",	"LAD2",			"n/a",		"ESPI_IO2",
	"GPP_A4",	"LAD3",			"n/a",		"ESPI_IO3",
	"GPP_A5",	"LFRAME#",		"n/a",		"ESPI_CS#",
	"GPP_A6",	"SERIRQ",		"n/a",		"n/a",
	"GPP_A7",	"PIRQA#",		"n/a",		"n/a",
	"GPP_A8",	"CLKRUN#",		"n/a",		"n/a",
	"GPP_A9",	"CLKOUT_LPC0",		"n/a",		"ESPI_CLK",
	"GPP_A10",	"CLKOUT_LPC1",		"n/a",		"n/a",
	"GPP_A11",	"PME#",			"n/a",		"n/a",
	"GPP_A12",	"BM_BUSY#",		"ISH_GP6",	"SX_EXIT_HOLDOFF#",
	"GPP_A13",	"SUSWARN#/SUSPWRDNACK",	"n/a",		"n/a",
	"GPP_A14",	"SUS_STAT#",		"n/a",		"ESPI_RESET#",
	"GPP_A15",	"SUS_ACK#",		"n/a",		"n/a",
	"GPP_A16",	"SD_1P8_SEL",		"n/a",		"n/a",
	"GPP_A17",	"SD_PWR_EN#",		"ISH_GP7",	"n/a",
	"GPP_A18",	"ISH_GP0",		"n/a",		"n/a",
	"GPP_A19",	"ISH_GP1",		"n/a",		"n/a",
	"GPP_A20",	"ISH_GP2",		"n/a",		"n/a",
	"GPP_A21",	"ISH_GP3",		"n/a",		"n/a",
	"GPP_A22",	"ISH_GP4",		"n/a",		"n/a",
	"GPP_A23",	"ISH_GP5",		"n/a",		"n/a",
};

static const char *const sunrise_group_b_names[] = {
	"GPP_B0",	"n/a",		"n/a",		"n/a",
	"GPP_B1",	"n/a",		"n/a",		"n/a",
	"GPP_B2",	"VRALERT#",	"n/a",		"n/a",
	"GPP_B3",	"CPU_GP2",	"n/a",		"n/a",
	"GPP_B4",	"CPU_GP3",	"n/a",		"n/a",
	"GPP_B5",	"SRCCLKREQ0#",	"n/a",		"n/a",
	"GPP_B6",	"SRCCLKREQ1#",	"n/a",		"n/a",
	"GPP_B7",	"SRCCLKREQ2#",	"n/a",		"n/a",
	"GPP_B8",	"SRCCLKREQ3#",	"n/a",		"n/a",
	"GPP_B9",	"SRCCLKREQ4#",	"n/a",		"n/a",
	"GPP_B10",	"SRCCLKREQ5#",	"n/a",		"n/a",
	"GPP_B11",	"n/a",		"n/a",		"n/a",
	"GPP_B12",	"SLP_S0#",	"n/a",		"n/a",
	"GPP_B13",	"PLTRST#",	"n/a",		"n/a",
	"GPP_B14",	"SPKR",		"n/a",		"n/a",
	"GPP_B15",	"GSPIO_CS#",	"n/a",		"n/a",
	"GPP_B16",	"GSPIO_CLK",	"n/a",		"n/a",
	"GPP_B17",	"GSPIO_MISO",	"n/a",		"n/a",
	"GPP_B18",	"GSPIO_MOSI",	"n/a",		"n/a",
	"GPP_B19",	"GSPI1_CS#",	"n/a",		"n/a",
	"GPP_B20",	"GSPI1_CLK",	"n/a",		"n/a",
	"GPP_B21",	"GSPI1_MISO",	"n/a",		"n/a",
	"GPP_B22",	"GSPI1_MOSI",	"n/a",		"n/a",
	"GPP_B23",	"SML1ALERT#",	"PCHHOT#",	"n/a",
};

static const char *const sunrise_lp_group_b_names[] = {
	"GPP_B0",	"CORE_VID0",		"n/a",		"n/a",
	"GPP_B1",	"CORE_VID1",		"n/a",		"n/a",
	"GPP_B2",	"VRALERT#",		"n/a",		"n/a",
	"GPP_B3",	"CPU_GP2",		"n/a",		"n/a",
	"GPP_B4",	"CPU_GP3",		"n/a",		"n/a",
	"GPP_B5",	"SRCCLKREQ0#",		"n/a",		"n/a",
	"GPP_B6",	"SRCCLKREQ1#",		"n/a",		"n/a",
	"GPP_B7",	"SRCCLKREQ2#",		"n/a",		"n/a",
	"GPP_B8",	"SRCCLKREQ3#",		"n/a",		"n/a",
	"GPP_B9",	"SRCCLKREQ4#",		"n/a",		"n/a",
	"GPP_B10",	"SRCCLKREQ5#",		"n/a",		"n/a",
	"GPP_B11",	"EXT_PWR_GATE#",	"n/a",		"n/a",
	"GPP_B12",	"SLP_S0#",		"n/a",		"n/a",
	"GPP_B13",	"PLTRST#",		"n/a",		"n/a",
	"GPP_B14",	"SPKR",			"n/a",		"n/a",
	"GPP_B15",	"GSPI0_CS#",		"n/a",		"n/a",
	"GPP_B16",	"GSPI0_CLK",		"n/a",		"n/a",
	"GPP_B17",	"GSPI0_MISO",		"n/a",		"n/a",
	"GPP_B18",	"GSPI0_MOSI",		"n/a",		"n/a",
	"GPP_B19",	"GSPI1_CS#",		"n/a",		"n/a",
	"GPP_B20",	"GSPI1_CLK",		"n/a",		"n/a",
	"GPP_B21",	"GSPI1_MISO",		"n/a",		"n/a",
	"GPP_B22",	"GSPI1_MOSI",		"n/a",		"n/a",
	"GPP_B23",	"SML1ALERT#",		"PCHHOT#",	"n/a",
};

static const char *const sunrise_group_c_names[] = {
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
	"GPP_C12",	"UART1_RXD",	"ISH_UART1_RXD",	"n/a",
	"GPP_C13",	"UART1_TXD",	"ISH_UART1_TXD",	"n/a",
	"GPP_C14",	"UART1_RTS#",	"ISH_UART1_RTS#",	"n/a",
	"GPP_C15",	"UART1_CTS#",	"ISH_UART1_CTS#",	"n/a",
	"GPP_C16",	"I2C0_SDA",	"n/a",			"n/a",
	"GPP_C17",	"I2C0_SCL",	"n/a",			"n/a",
	"GPP_C18",	"I2C1_SDA",	"n/a",			"n/a",
	"GPP_C19",	"I2C1_SCL",	"n/a",			"n/a",
	"GPP_C20",	"UART2_RXD",	"n/a",			"n/a",
	"GPP_C21",	"UART2_TXD",	"n/a",			"n/a",
	"GPP_C22",	"UART2_RTS#",	"n/a",			"n/a",
	"GPP_C23",	"UART2_CTS#",	"n/a",			"n/a",
};

static const char *const sunrise_group_d_names[] = {
	"GPP_D0",	"n/a",			"n/a",		"n/a",
	"GPP_D1",	"n/a",			"n/a",		"n/a",
	"GPP_D2",	"n/a",			"n/a",		"n/a",
	"GPP_D3",	"n/a",			"n/a",		"n/a",
	"GPP_D4",	"ISH_I2C2_SDA",		"I2C3_SDA",	"n/a",
	"GPP_D5",	"I2S_SFRM",		"n/a",		"n/a",
	"GPP_D6",	"I2S_TXD",		"n/a",		"n/a",
	"GPP_D7",	"I2S_RXD",		"n/a",		"n/a",
	"GPP_D8",	"I2S_SCLK",		"n/a",		"n/a",
	"GPP_D9",	"n/a",			"n/a",		"n/a",
	"GPP_D10",	"n/a",			"n/a",		"n/a",
	"GPP_D11",	"n/a",			"n/a",		"n/a",
	"GPP_D12",	"n/a",			"n/a",		"n/a",
	"GPP_D13",	"ISH_UART0_RXD",	"n/a",		"I2C2_SDA",
	"GPP_D14",	"ISH_UART0_TXD",	"n/a",		"I2C2_SCL",
	"GPP_D15",	"ISH_UART0_RTS#",	"n/a",		"n/a",
	"GPP_D16",	"ISH_UART0_CTS#",	"n/a",		"n/a",
	"GPP_D17",	"DMIC_CLK1",		"n/a",		"n/a",
	"GPP_D18",	"DMIC_DATA1",		"n/a",		"n/a",
	"GPP_D19",	"DMIC_CLK0",		"n/a",		"n/a",
	"GPP_D20",	"DMIC_DATA0",		"n/a",		"n/a",
	"GPP_D21",	"n/a",			"n/a",		"n/a",
	"GPP_D22",	"n/a",			"n/a",		"n/a",
	"GPP_D23",	"ISH_I2C2_SCL",		"I2C3_SCL",	"n/a",
};

static const char *const sunrise_lp_group_d_names[] = {
	"GPP_D0",	"SPI1_CS#",		"n/a",		"n/a",
	"GPP_D1",	"SPI1_CLK",		"n/a",		"n/a",
	"GPP_D2",	"SPI1_MISO",		"n/a",		"n/a",
	"GPP_D3",	"SPI1_MOSI",		"n/a",		"n/a",
	"GPP_D4",	"FLASHTRIG",		"n/a",		"n/a",
	"GPP_D5",	"ISH_I2C0_SDA",		"n/a",		"n/a",
	"GPP_D6",	"ISH_I2C0_SCL",		"n/a",		"n/a",
	"GPP_D7",	"ISH_I2C1_SDA",		"n/a",		"n/a",
	"GPP_D8",	"ISH_I2C1_SCL",		"n/a",		"n/a",
	"GPP_D9",	"n/a",			"n/a",		"n/a",
	"GPP_D10",	"n/a",			"n/a",		"n/a",
	"GPP_D11",	"n/a",			"n/a",		"n/a",
	"GPP_D12",	"n/a",			"n/a",		"n/a",
	"GPP_D13",	"ISH_UART0_RXD",	"n/a",		"n/a",
	"GPP_D14",	"ISH_UART0_TXD",	"n/a",		"n/a",
	"GPP_D15",	"ISH_UART0_RTS#",	"n/a",		"n/a",
	"GPP_D16",	"ISH_UART0_CTS#",	"n/a",		"n/a",
	"GPP_D17",	"DMIC_CLK1",		"n/a",		"n/a",
	"GPP_D18",	"DMIC_DATA1",		"n/a",		"n/a",
	"GPP_D19",	"DMIC_CLK0",		"n/a",		"n/a",
	"GPP_D20",	"DMIC_DATA0",		"n/a",		"n/a",
	"GPP_D21",	"SPI1_IO2",		"n/a",		"n/a",
	"GPP_D22",	"SPI1_IO3",		"n/a",		"n/a",
	"GPP_D23",	"I2S_MCLK",		"n/a",		"n/a",
};

static const char *const sunrise_group_e_names[] = {
	"GPP_E0",	"SATAXPCIE0",	"SATAGP0",	"n/a",
	"GPP_E1",	"SATAXPCIE1",	"SATAGP1",	"n/a",
	"GPP_E2",	"SATAXPCIE2",	"SATAGP2",	"n/a",
	"GPP_E3",	"CPU_GP0",	"n/a",		"n/a",
	"GPP_E4",	"SATA_DEVSLP0",	"n/a",		"n/a",
	"GPP_E5",	"SATA_DEVSLP1",	"n/a",		"n/a",
	"GPP_E6",	"SATA_DEVSLP2",	"n/a",		"n/a",
	"GPP_E7",	"CPU_GP1",	"n/a",		"n/a",
	"GPP_E8",	"SATA_LED#",	"n/a",		"n/a",
	"GPP_E9",	"USB_OC0#",	"n/a",		"n/a",
	"GPP_E10",	"USB_OC1#",	"n/a",		"n/a",
	"GPP_E11",	"USB_OC2#",	"n/a",		"n/a",
	"GPP_E12",	"USB_OC3#",	"n/a",		"n/a",
};

static const char *const sunrise_lp_group_e_names[] = {
	"GPP_E0",	"SATAXPCIE0",		"SATAGP0",	"n/a",
	"GPP_E1",	"SATAXPCIE1",		"SATAGP1",	"n/a",
	"GPP_E2",	"SATAXPCIE2",		"SATAGP2",	"n/a",
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
	"GPP_E13",	"DDPB_HPD0",		"n/a",		"n/a",
	"GPP_E14",	"DDPC_HPD1",		"n/a",		"n/a",
	"GPP_E15",	"DDPD_HPD2",		"n/a",		"n/a",
	"GPP_E16",	"DDPE_HPD3",		"n/a",		"n/a",
	"GPP_E17",	"EDP_HPD",		"n/a",		"n/a",
	"GPP_E18",	"DDPB_CTRLCLK",		"n/a",		"n/a",
	"GPP_E19",	"DDPB_CTRLDATA",	"n/a",		"n/a",
	"GPP_E20",	"DDPC_CTRLCLK",		"n/a",		"n/a",
	"GPP_E21",	"DDPC_CTRLDATA",	"n/a",		"n/a",
	"GPP_E22",	"n/a",			"n/a",		"n/a",
	"GPP_E23",	"n/a",			"n/a",		"n/a",
};

static const char *const sunrise_group_f_names[] = {
	"GPP_F0",	"SATAXPCIE3",		"SATAGP3",	"n/a",
	"GPP_F1",	"SATAXPCIE4",		"SATAGP4",	"n/a",
	"GPP_F2",	"SATAXPCIE5",		"SATAGP5",	"n/a",
	"GPP_F3",	"SATAXPCIE6",		"SATAGP6",	"n/a",
	"GPP_F4",	"SATAXPCIE7",		"SATAGP7",	"n/a",
	"GPP_F5",	"SATA_DEVSLP3",		"n/a",		"n/a",
	"GPP_F6",	"SATA_DEVSLP4",		"n/a",		"n/a",
	"GPP_F7",	"SATA_DEVSLP5",		"n/a",		"n/a",
	"GPP_F8",	"SATA_DEVSLP6",		"n/a",		"n/a",
	"GPP_F9",	"SATA_DEVSLP7",		"n/a",		"n/a",
	"GPP_F10",	"SATA_SCLOCK",		"n/a",		"n/a",
	"GPP_F11",	"SATA_SLOAD",		"n/a",		"n/a",
	"GPP_F12",	"SATA_SDATAOUT1",	"n/a",		"n/a",
	"GPP_F13",	"SATA_SDATAOUT2",	"n/a",		"n/a",
	"GPP_F14",	"n/a",			"n/a",		"n/a",
	"GPP_F15",	"USB_OC4#",		"n/a",		"n/a",
	"GPP_F16",	"USB_OC5#",		"n/a",		"n/a",
	"GPP_F17",	"USB_OC6#",		"n/a",		"n/a",
	"GPP_F18",	"USB_OC7#",		"n/a",		"n/a",
	"GPP_F19",	"eDP_VDDEN",		"n/a",		"n/a",
	"GPP_F20",	"eDP_BKLTEN",		"n/a",		"n/a",
	"GPP_F21",	"eDP_BKLTCTL",		"n/a",		"n/a",
	"GPP_F22",	"n/a",			"n/a",		"n/a",
	"GPP_F23",	"n/a",			"n/a",		"n/a",
};

static const char *const sunrise_lp_group_f_names[] = {
	"GPP_F0",	"I2S2_SCLK",		"n/a",		"n/a",
	"GPP_F1",	"I2S2_SFRM",		"n/a",		"n/a",
	"GPP_F2",	"I2S2_TXD",		"n/a",		"n/a",
	"GPP_F3",	"I2S2_RXD",		"n/a",		"n/a",
	"GPP_F4",	"I2C2_SDA",		"n/a",		"n/a",
	"GPP_F5",	"I2C2_SCL",		"n/a",		"n/a",
	"GPP_F6",	"I2C3_SDA",		"n/a",		"n/a",
	"GPP_F7",	"I2C3_SCL",		"n/a",		"n/a",
	"GPP_F8",	"I2C4_SDA",		"n/a",		"n/a",
	"GPP_F9",	"I2C4_SCL",		"n/a",		"n/a",
	"GPP_F10",	"I2C5_SDA",		"ISH_I2C2_SDA",	"n/a",
	"GPP_F11",	"I2C5_SCL",		"ISH_I2C2_SCL",	"n/a",
	"GPP_F12",	"EMMC_CMD",		"n/a",		"n/a",
	"GPP_F13",	"EMMC_DATA0",		"n/a",		"n/a",
	"GPP_F14",	"EMMC_DATA1",		"n/a",		"n/a",
	"GPP_F15",	"EMMC_DATA2",		"n/a",		"n/a",
	"GPP_F16",	"EMMC_DATA3",		"n/a",		"n/a",
	"GPP_F17",	"EMMC_DATA4",		"n/a",		"n/a",
	"GPP_F18",	"EMMC_DATA5",		"n/a",		"n/a",
	"GPP_F19",	"EMMC_DATA6",		"n/a",		"n/a",
	"GPP_F20",	"EMMC_DATA7",		"n/a",		"n/a",
	"GPP_F21",	"EMMC_RCLK",		"n/a",		"n/a",
	"GPP_F22",	"EMMC_CLK",		"n/a",		"n/a",
	"GPP_F23",	"n/a",			"n/a",		"n/a",
};

static const char *const sunrise_group_g_names[] = {
	"GPP_G0",	"FAN_TACH_0",	"n/a",	"n/a",
	"GPP_G1",	"FAN_TACH_1",	"n/a",	"n/a",
	"GPP_G2",	"FAN_TACH_2",	"n/a",	"n/a",
	"GPP_G3",	"FAN_TACH_3",	"n/a",	"n/a",
	"GPP_G4",	"FAN_TACH_4",	"n/a",	"n/a",
	"GPP_G5",	"FAN_TACH_5",	"n/a",	"n/a",
	"GPP_G6",	"FAN_TACH_6",	"n/a",	"n/a",
	"GPP_G7",	"FAN_TACH_7",	"n/a",	"n/a",
	"GPP_G8",	"FAN_PWM_0",	"n/a",	"n/a",
	"GPP_G9",	"FAN_PWM_1",	"n/a",	"n/a",
	"GPP_G10",	"FAN_PWM_2",	"n/a",	"n/a",
	"GPP_G11",	"FAN_PWM_3",	"n/a",	"n/a",
	"GPP_G12",	"GSXDOUT",	"n/a",	"n/a",
	"GPP_G13",	"GSXSLOAD",	"n/a",	"n/a",
	"GPP_G14",	"GSXDIN",	"n/a",	"n/a",
	"GPP_G15",	"GSXRESET#",	"n/a",	"n/a",
	"GPP_G16",	"GSXCLK",	"n/a",	"n/a",
	"GPP_G17",	"ADR_COMPLETE",	"n/a",	"n/a",
	"GPP_G18",	"NMI#",		"n/a",	"n/a",
	"GPP_G19",	"SMI#",		"n/a",	"n/a",
	"GPP_G20",	"n/a",		"n/a",	"n/a",
	"GPP_G21",	"n/a",		"n/a",	"n/a",
	"GPP_G22",	"n/a",		"n/a",	"n/a",
	"GPP_G23",	"n/a",		"n/a",	"n/a",
};

static const char *const sunrise_lp_group_g_names[] = {
	"GPP_G0",	"SD_CMD",	"n/a",	"n/a",
	"GPP_G1",	"SD_DATA0",	"n/a",	"n/a",
	"GPP_G2",	"SD_DATA1",	"n/a",	"n/a",
	"GPP_G3",	"SD_DATA2",	"n/a",	"n/a",
	"GPP_G4",	"SD_DATA3",	"n/a",	"n/a",
	"GPP_G5",	"SD_CD#",	"n/a",	"n/a",
	"GPP_G6",	"SD_CLK",	"n/a",	"n/a",
	"GPP_G7",	"SD_WP",	"n/a",	"n/a",
};

static const char *const sunrise_group_h_names[] = {
	"GPP_H0",	"SRCCLKREQ6#",	"n/a",	"n/a",
	"GPP_H1",	"SRCCLKREQ7#",	"n/a",	"n/a",
	"GPP_H2",	"SRCCLKREQ8#",	"n/a",	"n/a",
	"GPP_H3",	"SRCCLKREQ9#",	"n/a",	"n/a",
	"GPP_H4",	"SRCCLKREQ10#",	"n/a",	"n/a",
	"GPP_H5",	"SRCCLKREQ11#",	"n/a",	"n/a",
	"GPP_H6",	"SRCCLKREQ12#",	"n/a",	"n/a",
	"GPP_H7",	"SRCCLKREQ13#",	"n/a",	"n/a",
	"GPP_H8",	"SRCCLKREQ14#",	"n/a",	"n/a",
	"GPP_H9",	"SRCCLKREQ15#",	"n/a",	"n/a",
	"GPP_H10",	"SML2CLK",	"n/a",	"n/a",
	"GPP_H11",	"SML2DATA",	"n/a",	"n/a",
	"GPP_H12",	"SML2ALERT#",	"n/a",	"n/a",
	"GPP_H13",	"SML3CLK",	"n/a",	"n/a",
	"GPP_H14",	"SML3DATA",	"n/a",	"n/a",
	"GPP_H15",	"SML3ALERT#",	"n/a",	"n/a",
	"GPP_H16",	"SML4CLK",	"n/a",	"n/a",
	"GPP_H17",	"SML4DATA",	"n/a",	"n/a",
	"GPP_H18",	"SML4ALERT#",	"n/a",	"n/a",
	"GPP_H19",	"ISH_I2C0_SDA",	"n/a",	"n/a",
	"GPP_H20",	"ISH_I2C0_SCL",	"n/a",	"n/a",
	"GPP_H21",	"ISH_I2C1_SDA",	"n/a",	"n/a",
	"GPP_H22",	"ISH_I2C1_SCL",	"n/a",	"n/a",
	"GPP_H23",	"n/a",		"n/a",	"n/a",
};

static const char *const sunrise_group_gpd_names[] = {
	"GPD0",		"BATLOW#",	"n/a",	"n/a",
	"GPD1",		"ACPRESENT",	"n/a",	"n/a",
	"GPD2",		"LAN_WAKE#",	"n/a",	"n/a",
	"GPD3",		"PWRBTN#",	"n/a",	"n/a",
	"GPD4",		"SLP_S3#",	"n/a",	"n/a",
	"GPD5",		"SLP_S4#",	"n/a",	"n/a",
	"GPD6",		"SLP_A#",	"n/a",	"n/a",
	"GPD7",		"RESERVED",	"n/a",	"n/a",
	"GPD8",		"SUSCLK",	"n/a",	"n/a",
	"GPD9",		"SLP_WLAN#",	"n/a",	"n/a",
	"GPD10",	"SLP_S5#",	"n/a",	"n/a",
	"GPD11",	"LANPHYPC",	"n/a",	"n/a",
};

static const char *const sunrise_group_i_names[] = {
	"GPP_I0",	"DDPB_HPD0",		"n/a",	"n/a",
	"GPP_I1",	"DDPC_HPD1",		"n/a",	"n/a",
	"GPP_I2",	"DDPD_HPD2",		"n/a",	"n/a",
	"GPP_I3",	"DDPE_HPD3",		"n/a",	"n/a",
	"GPP_I4",	"EDP_HPD",		"n/a",	"n/a",
	"GPP_I5",	"DDPB_CTRLCLK",		"n/a",	"n/a",
	"GPP_I6",	"DDPB_CTRLDATA",	"n/a",	"n/a",
	"GPP_I7",	"DDPC_CTRLCLK",		"n/a",	"n/a",
	"GPP_I8",	"DDPC_CTRLDATA",	"n/a",	"n/a",
	"GPP_I9",	"DDPD_CTRLCLK",		"n/a",	"n/a",
	"GPP_I10",	"DDPD_CTRLDATA",	"n/a",	"n/a",
};

static const struct gpio_group sunrise_group_a = {
	.display	= "------- GPIO Group GPP_A -------",
	.pad_count	= ARRAY_SIZE(sunrise_group_a_names) / 4,
	.func_count	= 4,
	.pad_names	= sunrise_group_a_names,
};

static const struct gpio_group sunrise_lp_group_a = {
	.display	= "------- GPIO group GPP_A -------",
	.pad_count	= ARRAY_SIZE(sunrise_lp_group_a_names) / 4,
	.func_count	= 4,
	.pad_names	= sunrise_lp_group_a_names,
};

static const struct gpio_group sunrise_group_b = {
	.display	= "------- GPIO Group GPP_B -------",
	.pad_count	= ARRAY_SIZE(sunrise_group_b_names) / 4,
	.func_count	= 4,
	.pad_names	= sunrise_group_b_names,
};

static const struct gpio_group sunrise_lp_group_b = {
	.display	= "------- GPIO Group GPP_B -------",
	.pad_count	= ARRAY_SIZE(sunrise_lp_group_b_names) / 4,
	.func_count	= 4,
	.pad_names	= sunrise_lp_group_b_names,
};

static const struct gpio_group *const sunrise_community_ab_groups[] = {
	&sunrise_group_a, &sunrise_group_b,
};

static const struct gpio_group *const sunrise_lp_community_ab_groups[] = {
	&sunrise_lp_group_a, &sunrise_lp_group_b,
};

static const struct gpio_community sunrise_community_ab = {
	.name		= "------- GPIO Community 0 -------",
	.pcr_port_id	= 0xaf,
	.group_count	= ARRAY_SIZE(sunrise_community_ab_groups),
	.groups		= sunrise_community_ab_groups,
};

static const struct gpio_community sunrise_lp_community_ab = {
	.name		= "------- GPIO Community 0 -------",
	.pcr_port_id	= 0xaf,
	.group_count	= ARRAY_SIZE(sunrise_lp_community_ab_groups),
	.groups		= sunrise_lp_community_ab_groups,
};

static const struct gpio_group sunrise_group_c = {
	.display	= "------- GPIO Group GPP_C -------",
	.pad_count	= ARRAY_SIZE(sunrise_group_c_names) / 4,
	.func_count	= 4,
	.pad_names	= sunrise_group_c_names,
};

static const struct gpio_group sunrise_group_d = {
	.display	= "------- GPIO Group GPP_D -------",
	.pad_count	= ARRAY_SIZE(sunrise_group_d_names) / 4,
	.func_count	= 4,
	.pad_names	= sunrise_group_d_names,
};

static const struct gpio_group sunrise_lp_group_d = {
	.display	= "------- GPIO Group GPP_D -------",
	.pad_count	= ARRAY_SIZE(sunrise_lp_group_d_names) / 4,
	.func_count	= 4,
	.pad_names	= sunrise_lp_group_d_names,
};

static const struct gpio_group sunrise_group_e = {
	.display	= "------- GPIO Group GPP_E -------",
	.pad_count	= ARRAY_SIZE(sunrise_group_e_names) / 4,
	.func_count	= 4,
	.pad_names	= sunrise_group_e_names,
};

static const struct gpio_group sunrise_lp_group_e = {
	.display	= "------- GPIO Group GPP_E -------",
	.pad_count	= ARRAY_SIZE(sunrise_lp_group_e_names) / 4,
	.func_count	= 4,
	.pad_names	= sunrise_lp_group_e_names,
};

static const struct gpio_group sunrise_group_f = {
	.display	= "------- GPIO Group GPP_F -------",
	.pad_count	= ARRAY_SIZE(sunrise_group_f_names) / 4,
	.func_count	= 4,
	.pad_names	= sunrise_group_f_names,
};

static const struct gpio_group sunrise_lp_group_f = {
	.display	= "------- GPIO Group GPP_F -------",
	.pad_count	= ARRAY_SIZE(sunrise_lp_group_f_names) / 4,
	.func_count	= 4,
	.pad_names	= sunrise_lp_group_f_names,
};

static const struct gpio_group sunrise_group_g = {
	.display	= "------- GPIO Group GPP_G -------",
	.pad_count	= ARRAY_SIZE(sunrise_group_g_names) / 4,
	.func_count	= 4,
	.pad_names	= sunrise_group_g_names,
};

static const struct gpio_group sunrise_lp_group_g = {
	.display	= "------- GPIO Group GPP_G -------",
	.pad_count	= ARRAY_SIZE(sunrise_lp_group_g_names) / 4,
	.func_count	= 4,
	.pad_names	= sunrise_lp_group_g_names,
};

static const struct gpio_group sunrise_group_h = {
	.display	= "------- GPIO Group GPP_H -------",
	.pad_count	= ARRAY_SIZE(sunrise_group_h_names) / 4,
	.func_count	= 4,
	.pad_names	= sunrise_group_h_names,
};

static const struct gpio_group *const sunrise_community_cdefgh_groups[] = {
	&sunrise_group_c, &sunrise_group_d, &sunrise_group_e,
	&sunrise_group_f, &sunrise_group_g, &sunrise_group_h,
};

static const struct gpio_group *const sunrise_lp_community_cde_groups[] = {
	&sunrise_group_c, &sunrise_lp_group_d, &sunrise_lp_group_e,
};

static const struct gpio_community sunrise_community_cdefgh = {
	.name		= "------- GPIO Community 1 -------",
	.pcr_port_id	= 0xae,
	.group_count	= ARRAY_SIZE(sunrise_community_cdefgh_groups),
	.groups		= sunrise_community_cdefgh_groups,
};

static const struct gpio_community sunrise_lp_community_cde = {
	.name		= "------- GPIO Community 1 -------",
	.pcr_port_id	= 0xae,
	.group_count	= ARRAY_SIZE(sunrise_lp_community_cde_groups),
	.groups		= sunrise_lp_community_cde_groups,
};

static const struct gpio_group sunrise_group_gpd = {
	.display	= "-------- GPIO Group GPD --------",
	.pad_count	= ARRAY_SIZE(sunrise_group_gpd_names) / 4,
	.func_count	= 4,
	.pad_names	= sunrise_group_gpd_names,
};

static const struct gpio_group *const sunrise_community_gpd_groups[] = {
	&sunrise_group_gpd,
};

static const struct gpio_community sunrise_community_gpd = {
	.name		= "------- GPIO Community 2 -------",
	.pcr_port_id	= 0xad,
	.group_count	= ARRAY_SIZE(sunrise_community_gpd_groups),
	.groups		= sunrise_community_gpd_groups,
};

static const struct gpio_group sunrise_group_i = {
	.display	= "------- GPIO Group GPP_I -------",
	.pad_count	= ARRAY_SIZE(sunrise_group_i_names) / 4,
	.func_count	= 4,
	.pad_names	= sunrise_group_i_names,
};

static const struct gpio_group *const sunrise_community_i_groups[] = {
	&sunrise_group_i,
};

static const struct gpio_group *const sunrise_lp_community_fg_groups[] = {
	&sunrise_lp_group_f, &sunrise_lp_group_g,
};

static const struct gpio_community sunrise_community_i = {
	.name		= "------- GPIO Community 3 -------",
	.pcr_port_id	= 0xac,
	.group_count	= ARRAY_SIZE(sunrise_community_i_groups),
	.groups		= sunrise_community_i_groups,
};

static const struct gpio_community sunrise_lp_community_fg = {
	.name		= "------- GPIO Community 3 -------",
	.pcr_port_id	= 0xac,
	.group_count	= ARRAY_SIZE(sunrise_lp_community_fg_groups),
	.groups		= sunrise_lp_community_fg_groups,
};

static const struct gpio_community *const sunrise_communities[] = {
	&sunrise_community_ab, &sunrise_community_cdefgh,
	&sunrise_community_gpd, &sunrise_community_i,
};

static const struct gpio_community *const sunrise_lp_communities[] = {
	&sunrise_lp_community_ab, &sunrise_lp_community_cde,
	&sunrise_community_gpd, &sunrise_lp_community_fg,
};

#endif
