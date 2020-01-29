#ifndef GPIO_NAMES_LEWISBURG_H
#define GPIO_NAMES_LEWISBURG_H

#include "gpio_groups.h"
#include "sunrise.h"

static const char *const lewisburg_group_a_names[] = {
	"GPP_A0",	"RCIN#",		"n/a",		"ESPI_ALERT1#",
	"GPP_A1",	"LAD0",			"n/a",		"ESPI_IO0",
	"GPP_A2",	"LAD1",			"n/a",		"ESPI_IO1",
	"GPP_A3",	"LAD2",			"n/a",		"ESPI_IO2",
	"GPP_A4",	"LAD3",			"n/a",		"ESPI_IO3",
	"GPP_A5",	"LFRAME#",		"n/a",		"ESPI_CS0#",
	"GPP_A6",	"SERIRQ",		"n/a",		"ESPI_CS1#",
	"GPP_A7",	"PIRQA#",		"n/a",		"ESPI_ALERT0#",
	"GPP_A8",	"CLKRUN#",		"n/a",		"n/a",
	"GPP_A9",	"CLKOUT_LPC0",		"n/a",		"ESPI_CLK",
	"GPP_A10",	"CLKOUT_LPC1",		"n/a",		"n/a",
	"GPP_A11",	"PME#",			"n/a",		"n/a",
	"GPP_A12",	"BM_BUSY#",		"n/a",		"SX_EXIT_HOLDOFF#",
	"GPP_A13",	"SUSWARN#/SUSPWRDNACK",	"n/a",		"n/a",
	"GPP_A14",	"n/a",			"n/a",		"ESPI_RESET#",
	"GPP_A15",	"SUS_ACK#",		"n/a",		"n/a",
	"GPP_A16",	"CLKOUT_LPC2",		"n/a",		"n/a",
	"GPP_A17",	"n/a",			"n/a",		"n/a",
	"GPP_A18",	"n/a",			"n/a",		"n/a",
	"GPP_A19",	"n/a",			"n/a",		"n/a",
	"GPP_A20",	"n/a",			"n/a",		"n/a",
	"GPP_A21",	"n/a",			"n/a",		"n/a",
	"GPP_A22",	"n/a",			"n/a",		"n/a",
	"GPP_A23",	"n/a",			"n/a",		"n/a",
};

static const char *const lewisburg_group_b_names[] = {
	"GPP_B0",	"CORE_VID0",		"n/a",		"n/a",
	"GPP_B1",	"CORE_VID1",		"n/a",		"n/a",
	"GPP_B2",	"n/a",			"n/a",		"n/a",
	"GPP_B3",	"CPU_GP2",		"n/a",		"n/a",
	"GPP_B4",	"CPU_GP3",		"n/a",		"n/a",
	"GPP_B5",	"SRCCLKREQ0#",		"n/a",		"n/a",
	"GPP_B6",	"SRCCLKREQ1#",		"n/a",		"n/a",
	"GPP_B7",	"SRCCLKREQ2#",		"n/a",		"n/a",
	"GPP_B8",	"SRCCLKREQ3#",		"n/a",		"n/a",
	"GPP_B9",	"SRCCLKREQ4#",		"n/a",		"n/a",
	"GPP_B10",	"SRCCLKREQ5#",		"n/a",		"n/a",
	"GPP_B11",	"n/a",			"n/a",		"n/a",
	"GPP_B12",	"GLB_RST_WARN_N#",	"n/a",		"n/a",
	"GPP_B13",	"PLTRST#",		"n/a",		"n/a",
	"GPP_B14",	"SPKR",			"n/a",		"n/a",
	"GPP_B15",	"n/a",			"n/a",		"n/a",
	"GPP_B16",	"n/a",			"n/a",		"n/a",
	"GPP_B17",	"n/a",			"n/a",		"n/a",
	"GPP_B18",	"n/a",			"n/a",		"n/a",
	"GPP_B19",	"n/a",			"n/a",		"n/a",
	"GPP_B20",	"n/a",			"n/a",		"n/a",
	"GPP_B21",	"n/a",			"n/a",		"n/a",
	"GPP_B22",	"n/a",			"n/a",		"n/a",
	"GPP_B23",	"SML1ALERT#",		"PCHHOT#",	"MEIE_SML1ALRT#",
};

static const char *const lewisburg_group_c_names[] = {
	"GPP_C0",	"SMBCLK",	"n/a",			"n/a",
	"GPP_C1",	"SMBDATA",	"n/a",			"n/a",
	"GPP_C2",	"SMBALERT#",	"n/a",			"n/a",
	"GPP_C3",	"SML0CLK",	"SML0CLK_IE#",		"n/a",
	"GPP_C4",	"SML0DATA",	"SML0DATA_IE",		"n/a",
	"GPP_C5",	"SML0ALERT#",	"SML0ALERT_IE#",	"n/a",
	"GPP_C6",	"SML1CLK",	"SML1CLK_IE",		"n/a",
	"GPP_C7",	"SML1DATA",	"SML1DATA_IE",		"n/a",
	"GPP_C8",	"n/a",		"n/a",			"n/a",
	"GPP_C9",	"n/a",		"n/a",			"n/a",
	"GPP_C10",	"n/a",		"n/a",			"n/a",
	"GPP_C11",	"n/a",		"n/a",			"n/a",
	"GPP_C12",	"n/a",		"n/a",			"n/a",
	"GPP_C13",	"n/a",		"n/a",			"n/a",
	"GPP_C14",	"n/a",		"n/a",			"n/a",
	"GPP_C15",	"n/a",		"n/a",			"n/a",
	"GPP_C16",	"n/a",		"n/a",			"n/a",
	"GPP_C17",	"n/a",		"n/a",			"n/a",
	"GPP_C18",	"n/a",		"n/a",			"n/a",
	"GPP_C19",	"n/a",		"n/a",			"n/a",
	"GPP_C20",	"n/a",		"n/a",			"n/a",
	"GPP_C21",	"n/a",		"n/a",			"n/a",
	"GPP_C22",	"n/a",		"n/a",			"n/a",
	"GPP_C23",	"n/a",		"n/a",			"n/a",
};

static const char *const lewisburg_group_d_names[] = {
	"GPP_D0",	"n/a",		"n/a",		"SERIAL_BLINK",
	"GPP_D1",	"n/a",		"n/a",		"SERIAL_BLINK",
	"GPP_D2",	"n/a",		"n/a",		"SERIAL_BLINK",
	"GPP_D3",	"n/a",		"n/a",		"SERIAL_BLINK",
	"GPP_D4",	"n/a",		"n/a",		"SERIAL_BLINK",
	"GPP_D5",	"n/a",		"n/a",		"n/a",
	"GPP_D6",	"n/a",		"n/a",		"n/a",
	"GPP_D7",	"n/a",		"n/a",		"n/a",
	"GPP_D8",	"n/a",		"n/a",		"n/a",
	"GPP_D9",	"n/a",		"n/a",		"SSATA_DEVSLP3",
	"GPP_D10",	"n/a",		"n/a",		"SSATA_DEVSLP4",
	"GPP_D11",	"n/a",		"n/a",		"SSATA_DEVSLP5",
	"GPP_D12",	"n/a",		"n/a",		"SSATA_SDATAOUT1",
	"GPP_D13",	"n/a",		"SML0BCLK",	"SML0BCLK_IE",
	"GPP_D14",	"n/a",		"SML0BDATA",	"SML0BDATA_IE",
	"GPP_D15",	"n/a",		"n/a",		"SSATA_SDATAOUT0",
	"GPP_D16",	"n/a",		"SML0BALERT#",	"SML0BALERT_IE#",
	"GPP_D17",	"n/a",		"n/a",		"n/a",
	"GPP_D18",	"n/a",		"n/a",		"n/a",
	"GPP_D19",	"n/a",		"n/a",		"n/a",
	"GPP_D20",	"n/a",		"n/a",		"n/a",
	"GPP_D21",	"n/a",		"n/a",		"IE_UART_RX",
	"GPP_D22",	"n/a",		"n/a",		"IE_UART_TX",
	"GPP_D23",	"n/a",		"n/a",		"n/a",
};

static const char *const lewisburg_group_f_names[] = {
	"GPP_F0",	"SATAXPCIE3",		"n/a",		"SATAGP3",
	"GPP_F1",	"SATAXPCIE4",		"n/a",		"SATAGP4",
	"GPP_F2",	"SATAXPCIE5",		"n/a",		"SATAGP5",
	"GPP_F3",	"SATAXPCIE6",		"n/a",		"SATAGP6",
	"GPP_F4",	"SATAXPCIE7",		"n/a",		"SATAGP7",
	"GPP_F5",	"SATA_DEVSLP3",		"n/a",		"n/a",
	"GPP_F6",	"SATA_DEVSLP4",		"n/a",		"n/a",
	"GPP_F7",	"SATA_DEVSLP5",		"n/a",		"n/a",
	"GPP_F8",	"SATA_DEVSLP6",		"n/a",		"n/a",
	"GPP_F9",	"SATA_DEVSLP7",		"n/a",		"n/a",
	"GPP_F10",	"SATA_SCLOCK",		"n/a",		"n/a",
	"GPP_F11",	"SATA_SLOAD",		"n/a",		"n/a",
	"GPP_F12",	"SATA_SDATAOUT1",	"n/a",		"n/a",
	"GPP_F13",	"SATA_SDATAOUT2",	"n/a",		"n/a",
	"GPP_F14",	"n/a",			"n/a",		"SSATA_LED#",
	"GPP_F15",	"USB_OC4#",		"n/a",		"n/a",
	"GPP_F16",	"USB_OC5#",		"n/a",		"n/a",
	"GPP_F17",	"USB_OC6#",		"n/a",		"n/a",
	"GPP_F18",	"USB_OC7#",		"n/a",		"n/a",
	"GPP_F19",	"LAN_SMBCLK",		"n/a",		"n/a",
	"GPP_F20",	"LAN_SMBDATA",		"n/a",		"n/a",
	"GPP_F21",	"LAN_SMBALRT#",		"n/a",		"n/a",
	"GPP_F22",	"n/a",			"n/a",		"SSATA_SCLOCK",
	"GPP_F23",	"n/a",			"n/a",		"SSATA_SLOAD",
};

static const char *const lewisburg_group_g_names[] = {
	"GPP_G0",	"FAN_TACH_0",	"FAN_TACH_0_IE",	"n/a",
	"GPP_G1",	"FAN_TACH_1",	"FAN_TACH_1_IE",	"n/a",
	"GPP_G2",	"FAN_TACH_2",	"FAN_TACH_2_IE",	"n/a",
	"GPP_G3",	"FAN_TACH_3",	"FAN_TACH_3_IE",	"n/a",
	"GPP_G4",	"FAN_TACH_4",	"FAN_TACH_4_IE",	"n/a",
	"GPP_G5",	"FAN_TACH_5",	"FAN_TACH_5_IE",	"n/a",
	"GPP_G6",	"FAN_TACH_6",	"FAN_TACH_6_IE",	"n/a",
	"GPP_G7",	"FAN_TACH_7",	"FAN_TACH_7_IE",	"n/a",
	"GPP_G8",	"FAN_PWM_0",	"FAN_PWM_0_IE",		"n/a",
	"GPP_G9",	"FAN_PWM_1",	"FAN_PWM_1_IE",		"n/a",
	"GPP_G10",	"FAN_PWM_2",	"FAN_PWM_2_IE",		"n/a",
	"GPP_G11",	"FAN_PWM_3",	"FAN_PWM_3_IE",		"n/a",
	"GPP_G12",	"n/a",		"n/a",			"n/a",
	"GPP_G13",	"n/a",		"n/a",			"n/a",
	"GPP_G14",	"n/a",		"n/a",			"n/a",
	"GPP_G15",	"n/a",		"n/a",			"n/a",
	"GPP_G16",	"n/a",		"n/a",			"n/a",
	"GPP_G17",	"ADR_COMPLETE",	"n/a",			"n/a",
	"GPP_G18",	"NMI#",		"n/a",			"n/a",
	"GPP_G19",	"SMI#",		"n/a",			"n/a",
	"GPP_G20",	"n/a",		"SSATA_DEVSLP0",	"n/a",
	"GPP_G21",	"n/a",		"SSATA_DEVSLP1",	"n/a",
	"GPP_G22",	"n/a",		"SSATA_DEVSLP2",	"n/a",
	"GPP_G23",	"n/a",		"SSATAXPCIE0",		"SSATAGP0",
};

static const char *const lewisburg_group_h_names[] = {
	"GPP_H0",	"SRCCLKREQ6#",		"n/a",			"n/a",
	"GPP_H1",	"SRCCLKREQ7#",		"n/a",			"n/a",
	"GPP_H2",	"SRCCLKREQ8#",		"n/a",			"n/a",
	"GPP_H3",	"SRCCLKREQ9#",		"n/a",			"n/a",
	"GPP_H4",	"SRCCLKREQ10#",		"n/a",			"n/a",
	"GPP_H5",	"SRCCLKREQ11#",		"n/a",			"n/a",
	"GPP_H6",	"SRCCLKREQ12#",		"n/a",			"n/a",
	"GPP_H7",	"SRCCLKREQ13#",		"n/a",			"n/a",
	"GPP_H8",	"SRCCLKREQ14#",		"n/a",			"n/a",
	"GPP_H9",	"SRCCLKREQ15#",		"n/a",			"n/a",
	"GPP_H10",	"SML2CLK",		"SML2CLK_IE",		"n/a",
	"GPP_H11",	"SML2DATA",		"SML2DATA_IE",		"n/a",
	"GPP_H12",	"SML2ALERT#",		"SML2ALERT#_IE#",	"n/a",
	"GPP_H13",	"SML3CLK",		"SML3CLK_IE",		"n/a",
	"GPP_H14",	"SML3DATA",		"SML3DATA_IE",		"n/a",
	"GPP_H15",	"SML3ALERT#",		"SML3ALERT#_IE#",	"n/a",
	"GPP_H16",	"SML4CLK",		"SML4CLK_IE",		"n/a",
	"GPP_H17",	"SML4DATA",		"SML4DATA_IE",		"n/a",
	"GPP_H18",	"SML4ALERT#",		"SML4ALERT#_IE#",	"n/a",
	"GPP_H19",	"n/a",			"SSATAXPCIE1",		"SSATAGP1",
	"GPP_H20",	"n/a",			"SSATAXPCIE2",		"SSATAGP2",
	"GPP_H21",	"n/a",			"SSATAXPCIE3",		"SSATAGP3",
	"GPP_H22",	"n/a",			"SSATAXPCIE4",		"SSATAGP4",
	"GPP_H23",	"n/a",			"SSATAXPCIE5",		"SSATAGP5",
};

static const char *const lewisburg_group_i_names[] = {
	"GPP_I0",	"n/a",		"LAN_TDO",		"n/a",
	"GPP_I1",	"n/a",		"LAN_TCK",		"n/a",
	"GPP_I2",	"n/a",		"LAN_TMS",		"n/a",
	"GPP_I3",	"n/a",		"LAN_TDI",		"n/a",
	"GPP_I4",	"n/a",		"RESET_IN#",		"n/a",
	"GPP_I5",	"n/a",		"RESET_OUT#",		"n/a",
	"GPP_I6",	"n/a",		"RESET_DONE",		"n/a",
	"GPP_I7",	"n/a",		"LAN_TRST_IN",		"n/a",
	"GPP_I8",	"n/a",		"PCI_DIS",		"n/a",
	"GPP_I9",	"n/a",		"LAN_DIS",		"n/a",
	"GPP_I10",	"n/a",		"n/a",			"n/a",
};

static const char *const lewisburg_group_j_names[] = {
	"GPP_J0",	"LAN_LED_P0_0",		"n/a",		"n/a",
	"GPP_J1",	"LAN_LED_P0_1",		"n/a",		"n/a",
	"GPP_J2",	"LAN_LED_P1_0",		"n/a",		"n/a",
	"GPP_J3",	"LAN_LED_P1_1",		"n/a",		"n/a",
	"GPP_J4",	"LAN_LED_P2_0",		"n/a",		"n/a",
	"GPP_J5",	"LAN_LED_P2_1",		"n/a",		"n/a",
	"GPP_J6",	"LAN_LED_P3_0",		"n/a",		"n/a",
	"GPP_J7",	"LAN_LED_P3_1",		"n/a",		"n/a",
	"GPP_J8",	"LAN_I2C_SCL_MDC_P0",	"n/a",		"n/a",
	"GPP_J9",	"LAN_I2C_SDA_MDIO_P0",	"n/a",		"n/a",
	"GPP_J10",	"LAN_I2C_SCL_MDC_P1",	"n/a",		"n/a",
	"GPP_J11",	"LAN_I2C_SDA_MDIO_P1",	"n/a",		"n/a",
	"GPP_J12",	"LAN_I2C_SCL_MDC_P2",	"n/a",		"n/a",
	"GPP_J13",	"LAN_I2C_SDA_MDIO_P2",	"n/a",		"n/a",
	"GPP_J14",	"LAN_I2C_SCL_MDC_P3",	"n/a",		"n/a",
	"GPP_J15",	"LAN_I2C_SDA_MDIO_P3",	"n/a",		"n/a",
	"GPP_J16",	"LAN_SDP_P0_0",		"n/a",		"n/a",
	"GPP_J17",	"LAN_SDP_P0_1",		"n/a",		"n/a",
	"GPP_J18",	"LAN_SDP_P1_0",		"n/a",		"n/a",
	"GPP_J19",	"LAN_SDP_P1_1",		"n/a",		"n/a",
	"GPP_J20",	"LAN_SDP_P2_0",		"n/a",		"n/a",
	"GPP_J21",	"LAN_SDP_P2_1",		"n/a",		"n/a",
	"GPP_J22",	"LAN_SDP_P3_0",		"n/a",		"n/a",
	"GPP_J23",	"LAN_SDP_P3_1",		"n/a",		"n/a",
};

static const char *const lewisburg_group_k_names[] = {
	"GPP_K0",	"LAN_NCSI_CLK_IN",	"n/a",		"n/a",
	"GPP_K1",	"LAN_NCSI_TXD0",	"n/a",		"n/a",
	"GPP_K2",	"LAN_NCSI_TXD1",	"n/a",		"n/a",
	"GPP_K3",	"LAN_NCSI_TX_EN",	"n/a",		"n/a",
	"GPP_K4",	"LAN_NCSI_CRS_DV",	"n/a",		"n/a",
	"GPP_K5",	"LAN_NCSI_RXD0",	"n/a",		"n/a",
	"GPP_K6",	"LAN_NCSI_RXD1",	"n/a",		"n/a",
	"GPP_K7",	"RESERVED",		"n/a",		"n/a",
	"GPP_K8",	"LAN_NCSI_ARB_IN",	"n/a",		"n/a",
	"GPP_K9",	"LAN_NCSI_ARB_OUT",	"n/a",		"n/a",
	"GPP_K10",	"PE_RST#",		"n/a",		"n/a",
};

static const char *const lewisburg_group_l_names[] = {
	"GPP_L2",	"TESTCH0_D0",		"n/a",		"n/a",
	"GPP_L3",	"TESTCH0_D1",		"n/a",		"n/a",
	"GPP_L4",	"TESTCH0_D2",		"n/a",		"n/a",
	"GPP_L5",	"TESTCH0_D3",		"n/a",		"n/a",
	"GPP_L6",	"TESTCH0_D4",		"n/a",		"n/a",
	"GPP_L7",	"TESTCH0_D5",		"n/a",		"n/a",
	"GPP_L8",	"TESTCH0_D6",		"n/a",		"n/a",
	"GPP_L9",	"TESTCH0_D7",		"n/a",		"n/a",
	"GPP_L10",	"TESTCH0_CLK",		"n/a",		"n/a",
	"GPP_L11",	"TESTCH1_D0",		"n/a",		"n/a",
	"GPP_L12",	"TESTCH1_D1",		"n/a",		"n/a",
	"GPP_L13",	"TESTCH1_D2",		"n/a",		"n/a",
	"GPP_L14",	"TESTCH1_D3",		"n/a",		"n/a",
	"GPP_L15",	"TESTCH1_D4",		"n/a",		"n/a",
	"GPP_L16",	"TESTCH1_D5",		"n/a",		"n/a",
	"GPP_L17",	"TESTCH1_D6",		"n/a",		"n/a",
	"GPP_L18",	"TESTCH1_D7",		"n/a",		"n/a",
	"GPP_L19",	"TESTCH1_CLK",		"n/a",		"n/a",
};

static const char *const lewisburg_group_gpd_names[] = {
	"GPD0",		"POWER_DEBUG_N",	"n/a",		"n/a",
	"GPD1",		"ACPRESENT",		"n/a",		"n/a",
	"GPD2",		"GBE_WAKE#",		"n/a",		"n/a",
	"GPD3",		"PWRBTN#",		"n/a",		"n/a",
	"GPD4",		"SLP_S3#",		"n/a",		"n/a",
	"GPD5",		"SLP_S4#",		"n/a",		"n/a",
	"GPD6",		"SLP_A#",		"n/a",		"n/a",
	"GPD7",		"RESERVED",		"n/a",		"n/a",
	"GPD8",		"SUSCLK",		"n/a",		"n/a",
	"GPD9",		"RESERVED",		"n/a",		"n/a",
	"GPD10",	"SLP_S5#",		"n/a",		"n/a",
	"GPD11",	"GBEPHY",		"n/a",		"n/a",
};

static const struct gpio_group lewisburg_group_a = {
	.display	= "------- GPIO Group GPP_A -------",
	.pad_count	= ARRAY_SIZE(lewisburg_group_a_names) / 4,
	.func_count	= 4,
	.pad_names	= lewisburg_group_a_names,
};

static const struct gpio_group lewisburg_group_b = {
	.display	= "------- GPIO Group GPP_B -------",
	.pad_count	= ARRAY_SIZE(lewisburg_group_b_names) / 4,
	.func_count	= 4,
	.pad_names	= lewisburg_group_b_names,
};

static const struct gpio_group lewisburg_group_c = {
	.display	= "------- GPIO Group GPP_C -------",
	.pad_count	= ARRAY_SIZE(lewisburg_group_c_names) / 4,
	.func_count	= 4,
	.pad_names	= lewisburg_group_c_names,
};

static const struct gpio_group lewisburg_group_d = {
	.display	= "------- GPIO Group GPP_D -------",
	.pad_count	= ARRAY_SIZE(lewisburg_group_d_names) / 4,
	.func_count	= 4,
	.pad_names	= lewisburg_group_d_names,
};

/* The functions in this group are the same as in the pad group E for
   the Sunrise-H PCH */
static const struct gpio_group lewisburg_group_e = {
	.display	= "------- GPIO Group GPP_E -------",
	.pad_count	= ARRAY_SIZE(sunrise_group_e_names) / 4,
	.func_count	= 4,
	.pad_names	= sunrise_group_e_names,
};

static const struct gpio_group lewisburg_group_f = {
	.display	= "------- GPIO Group GPP_F -------",
	.pad_count	= ARRAY_SIZE(lewisburg_group_f_names) / 4,
	.func_count	= 4,
	.pad_names	= lewisburg_group_f_names,
};

static const struct gpio_group lewisburg_group_g = {
	.display	= "------- GPIO Group GPP_G -------",
	.pad_count	= ARRAY_SIZE(lewisburg_group_g_names) / 4,
	.func_count	= 4,
	.pad_names	= lewisburg_group_g_names,
};

static const struct gpio_group lewisburg_group_h = {
	.display	= "------- GPIO Group GPP_H -------",
	.pad_count	= ARRAY_SIZE(lewisburg_group_h_names) / 4,
	.func_count	= 4,
	.pad_names	= lewisburg_group_h_names,
};

static const struct gpio_group lewisburg_group_i = {
	.display	= "------- GPIO Group GPP_I -------",
	.pad_count	= ARRAY_SIZE(lewisburg_group_i_names) / 4,
	.func_count	= 4,
	.pad_names	= lewisburg_group_i_names,
};

static const struct gpio_group lewisburg_group_j = {
	.display	= "------- GPIO Group GPP_J -------",
	.pad_count	= ARRAY_SIZE(lewisburg_group_j_names) / 4,
	.func_count	= 4,
	.pad_names	= lewisburg_group_j_names,
};

static const struct gpio_group lewisburg_group_k = {
	.display	= "------- GPIO Group GPP_K -------",
	.pad_count	= ARRAY_SIZE(lewisburg_group_k_names) / 4,
	.func_count	= 4,
	.pad_names	= lewisburg_group_k_names,
};

static const struct gpio_group lewisburg_group_l = {
	.display	= "------- GPIO Group GPP_L -------",
	.pad_count	= ARRAY_SIZE(lewisburg_group_l_names) / 4,
	.func_count	= 4,
	.pad_names	= lewisburg_group_l_names,
};

static const struct gpio_group lewisburg_group_gpd = {
	.display	= "-------- GPIO Group GPD --------",
	.pad_count	= ARRAY_SIZE(lewisburg_group_gpd_names) / 4,
	.func_count	= 4,
	.pad_names	= lewisburg_group_gpd_names,
};

static const struct gpio_group *const lewisburg_community0_abf_groups[] = {
	&lewisburg_group_a,
	&lewisburg_group_b,
	&lewisburg_group_f,
};

static const struct gpio_community lewisburg_community0_abf = {
	.name		= "------- GPIO Community 0 -------",
	.pcr_port_id	= 0xaf,
	.group_count	= ARRAY_SIZE(lewisburg_community0_abf_groups),
	.groups		= lewisburg_community0_abf_groups,
};

static const struct gpio_group *const lewisburg_community1_cde_groups[] = {
	&lewisburg_group_c,
	&lewisburg_group_d,
	&lewisburg_group_e,
};

static const struct gpio_community lewisburg_community1_cde = {
	.name		= "------- GPIO Community 1 -------",
	.pcr_port_id	= 0xae,
	.group_count	= ARRAY_SIZE(lewisburg_community1_cde_groups),
	.groups		= lewisburg_community1_cde_groups,
};

static const struct gpio_group *const lewisburg_community2_gpd_groups[] = {
	&lewisburg_group_gpd,
};

static const struct gpio_community lewisburg_community2_gpd = {
	.name		= "------- GPIO Community 2 -------",
	.pcr_port_id	= 0xad,
	.group_count	= ARRAY_SIZE(lewisburg_community2_gpd_groups),
	.groups		= lewisburg_community2_gpd_groups,
};

static const struct gpio_group *const lewisburg_community3_i_groups[] = {
	&lewisburg_group_i,
};

static const struct gpio_community lewisburg_community3_i = {
	.name		= "------- GPIO Community 3 -------",
	.pcr_port_id	= 0xac,
	.group_count	= ARRAY_SIZE(lewisburg_community3_i_groups),
	.groups		= lewisburg_community3_i_groups,
};

static const struct gpio_group *const lewisburg_community4_jk_groups[] = {
	&lewisburg_group_j,
	&lewisburg_group_k,
};

static const struct gpio_community lewisburg_community4_jk = {
	.name		= "------- GPIO Community 4 -------",
	.pcr_port_id	= 0xab,
	.group_count	= ARRAY_SIZE(lewisburg_community4_jk_groups),
	.groups		= lewisburg_community4_jk_groups,
};

static const struct gpio_group *const lewisburg_community5_ghl_groups[] = {
	&lewisburg_group_g,
	&lewisburg_group_h,
	&lewisburg_group_l,
};

static const struct gpio_community lewisburg_community5_ghl = {
	.name		= "------- GPIO Community 5 -------",
	.pcr_port_id	= 0x11,
	.group_count	= ARRAY_SIZE(lewisburg_community5_ghl_groups),
	.groups		= lewisburg_community5_ghl_groups,
};

static const struct gpio_community *const lewisburg_communities[] = {
	&lewisburg_community0_abf,
	&lewisburg_community1_cde,
	&lewisburg_community2_gpd,
	&lewisburg_community3_i,
	&lewisburg_community4_jk,
	&lewisburg_community5_ghl,
};

#endif
