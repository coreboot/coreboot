/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_GPIO_H
#define MAINBOARD_GPIO_H

#include <soc/gpe.h>
#include <soc/gpio.h>

#ifndef __ACPI__

/* Early pad configuration in romstage. */
static const struct pad_config early_gpio_table[] = {
	// UART2
		// UART2_RXD
		PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),
		// UART2_TXD
		PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),
		// NC
		PAD_NC(GPP_C22, UP_20K),
		// NC
		PAD_NC(GPP_C23, UP_20K),
};

/* Pad configuration in ramstage. */
static const struct pad_config gpio_table[] = {
// GPD
	// Power Management
		// NC
		PAD_NC(GPD0, NONE),
		// ACPRESENT / AC_PRESENT
		PAD_CFG_NF(GPD1, NATIVE, DEEP, NF1),
		// NC
		PAD_NC(GPD2, UP_20K),
		// PWRBTN# / PWR_BTN#
		PAD_CFG_NF(GPD3, UP_20K, DEEP, NF1),
		// SLP_S3# / SUSB#_PCH
		PAD_CFG_NF(GPD4, NONE, DEEP, NF1),
		// SLP_S4# / SUSC#_PCH
		PAD_CFG_NF(GPD5, NONE, DEEP, NF1),
		// NC
		PAD_NC(GPD6, UP_20K),

	// GPIO
		// NC
		PAD_NC(GPD7, NONE),

	// Clock Signals
		// SUSCLK / SUS_CLK
		PAD_CFG_NF(GPD8, NONE, DEEP, NF1),

	// Power Management
		// NC
		PAD_NC(GPD9, UP_20K),
		// NC
		PAD_NC(GPD10, UP_20K),
		// NC
		PAD_NC(GPD11, UP_20K),

// GPP_A
	// LPC
		// RCIN# / SB_KBCRST#
		PAD_CFG_NF(GPP_A0, NONE, DEEP, NF1),
		// LAD0 / LPC_AD0
		PAD_CFG_NF(GPP_A1, NATIVE, DEEP, NF1),
		// LAD1 / LPC_AD1
		PAD_CFG_NF(GPP_A2, NATIVE, DEEP, NF1),
		// LAD2 / LPC_AD2
		PAD_CFG_NF(GPP_A3, NATIVE, DEEP, NF1),
		// LAD3 / LPC_AD3
		PAD_CFG_NF(GPP_A4, NATIVE, DEEP, NF1),
		// LFRAME# / LPC_FRAME#
		PAD_CFG_NF(GPP_A5, NONE, DEEP, NF1),
		// SERIRQ
		PAD_CFG_NF(GPP_A6, NONE, DEEP, NF1),

	// GSPI0
		// PIRQA# / TPM_PIRQ#
		PAD_CFG_NF(GPP_A7, NONE, DEEP, NF1),

	// LPC
		// CLKRUN# / PM_CLKRUN#
		// Note: R209 is populated despite being marked no-stuff in schematic
		PAD_CFG_NF(GPP_A8, NONE, DEEP, NF1),
		// CLKOUT_LPC0 / PCLK_KBC
		PAD_CFG_NF(GPP_A9, DN_20K, DEEP, NF1),
		// NC
		PAD_NC(GPP_A10, UP_20K),

	// GSPI1
		// INTP_OUT (Type-C VBUS_SENSE; unused in cb)
		PAD_NC(GPP_A11, UP_20K),

	// ISH_GP
		// NC
		PAD_NC(GPP_A12, UP_20K),

	// Power Management
		// SUSWARN# (unused due to missing DeepSx support)
		PAD_NC(GPP_A13, UP_20K),

	// LPC
		// NC
		PAD_NC(GPP_A14, UP_20K),

	// Power Management
		// NC
		PAD_NC(GPP_A15, UP_20K),

	// SD
		// NC
		PAD_NC(GPP_A16, UP_20K),
		// LEDKB_DET# (unused in cb; all devices of that model have KB LED)
		PAD_NC(GPP_A17, NONE),

	// ISH_GP
		// NC
		PAD_NC(GPP_A18, UP_20K),
		// NC
		PAD_NC(GPP_A19, UP_20K),
		// GPP_A20 / TEST_R
		PAD_CFG_GPO(GPP_A20, 0, DEEP),
		// NC
		PAD_NC(GPP_A21, UP_20K),
		// NC
		PAD_NC(GPP_A22, UP_20K),
		// NC
		PAD_NC(GPP_A23, UP_20K),

// GPP_B
	// Power
		// NC
		PAD_NC(GPP_B0, UP_20K),
		// NC
		PAD_NC(GPP_B1, UP_20K),

	// Power Management
		// CNVI_WAKE# (UART_WAKE# in M.2 spec; unused)
		PAD_NC(GPP_B2, UP_20K),

	// CPU Misc
		// GPP_B3
		_PAD_CFG_STRUCT(GPP_B3, 0x80100100, 0x0000),
		// NC
		PAD_NC(GPP_B4, UP_20K),

	// Clock Signals
		// NC
		PAD_NC(GPP_B5, UP_20K),
		// NC
		PAD_NC(GPP_B6, UP_20K),
		// SRCCLKREQ2# / WLAN_CLKREQ#
		PAD_CFG_NF(GPP_B7, NONE, DEEP, NF1),
		// SRCCLKREQ3# / CARD_CLKREQ#
		PAD_CFG_NF(GPP_B8, NONE, DEEP, NF1),
		// SRCCLKREQ4# / SSD2_CLKREQ#
		PAD_CFG_NF(GPP_B9, NONE, DEEP, NF1),
		// SRCCLKREQ5# / SSD1_CLKREQ#
		PAD_CFG_NF(GPP_B10, NONE, DEEP, NF1),

	// Power Management
		// NC
		PAD_NC(GPP_B11, NONE),
		// SLP_S0#
		PAD_CFG_NF(GPP_B12, NONE, DEEP, NF1),
		// PLT_RST#
		PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1),

	// SPKR
		// SPKR / PCH_SPKR
		PAD_CFG_NF(GPP_B14, NONE, DEEP, NF1),

	// GSPI0
		// NC
		PAD_NC(GPP_B15, UP_20K),
		// NC
		PAD_NC(GPP_B16, UP_20K),
		// NC
		PAD_NC(GPP_B17, NONE),
		// NC
		PAD_NC(GPP_B18, UP_20K),

	// GSPI1
		// NC
		PAD_NC(GPP_B19, UP_20K),
		// NC
		PAD_NC(GPP_B20, UP_20K),
		// NC
		PAD_NC(GPP_B21, UP_20K),
		// NC
		PAD_NC(GPP_B22, UP_20K),

	// SMBUS
		// NC
		PAD_NC(GPP_B23, UP_20K),

// GPP_C
	// SMBUS
		// SMBCLK / SMB_CLK_DDR
		PAD_CFG_NF(GPP_C0, NONE, DEEP, NF1),
		// SMBDATA / SMB_DAT_DDR
		PAD_CFG_NF(GPP_C1, NONE, DEEP, NF1),
		// NC
		PAD_NC(GPP_C2, UP_20K),
		// NC
		PAD_NC(GPP_C3, UP_20K),
		// NC
		PAD_NC(GPP_C4, UP_20K),
		// NC
		PAD_NC(GPP_C5, UP_20K),
		// NC
		PAD_NC(GPP_C6, UP_20K),
		// NC
		PAD_NC(GPP_C7, UP_20K),

	// UART0
		// NC
		PAD_NC(GPP_C8, UP_20K),
		// NC
		PAD_NC(GPP_C9, UP_20K),
		// NC
		PAD_NC(GPP_C10, UP_20K),
		// NC
		PAD_NC(GPP_C11, UP_20K),

	// UART1
		// NC
		PAD_NC(GPP_C12, UP_20K),
		// GPP_C13 / SSD1_PWR_DN#
		PAD_CFG_GPO(GPP_C13, 1, PLTRST),
		// NC
		PAD_NC(GPP_C14, UP_20K),
		// NC
		PAD_NC(GPP_C15, UP_20K),

	// I2C
		// I2C0_SDA / T_SDA
		PAD_CFG_NF(GPP_C16, NONE, DEEP, NF1),
		// I2C0_SCL / T_SCL
		PAD_CFG_NF(GPP_C17, NONE, DEEP, NF1),
		// NC
		PAD_NC(GPP_C18, UP_20K),
		// NC
		PAD_NC(GPP_C19, UP_20K),

	// UART2
		// UART2_RXD
		PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),
		// UART2_TXD
		PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),
		// NC
		PAD_NC(GPP_C22, UP_20K),
		// NC
		PAD_NC(GPP_C23, UP_20K),

// GPP_D
	// SPI1
		// NC
		PAD_NC(GPP_D0, UP_20K),
		// NC
		PAD_NC(GPP_D1, UP_20K),
		// NC
		PAD_NC(GPP_D2, UP_20K),
		// NC
		PAD_NC(GPP_D3, UP_20K),

	// IMGCLKOUT
		// NC
		PAD_NC(GPP_D4, UP_20K),

	// I2C
		// NC
		PAD_NC(GPP_D5, UP_20K),
		// NC
		PAD_NC(GPP_D6, UP_20K),
		// NC
		PAD_NC(GPP_D7, UP_20K),
		// SB_BLON
		PAD_CFG_GPO(GPP_D8, 1, DEEP),

	// GSPI2
		// SWI#
		_PAD_CFG_STRUCT(GPP_D9, 0x40880100, 0x0000),
		// DDR_TYPE_D10 (unused; there is only one on-board ram type/model)
		PAD_NC(GPP_D10, NONE),
		// BOARD_ID (unused in cb; we already know the device model)
		PAD_NC(GPP_D11, NONE),
		// NC
		PAD_NC(GPP_D12, UP_20K),

	// UART0
		// NC
		PAD_NC(GPP_D13, UP_20K),
		// SSD2_PWR_DN#
		PAD_CFG_GPO(GPP_D14, 1, PLTRST),
		// NC
		PAD_NC(GPP_D15, UP_20K),
		// NC
		PAD_NC(GPP_D16, UP_20K),

	// DMIC
		// NC
		PAD_NC(GPP_D17, UP_20K),
		// NC
		PAD_NC(GPP_D18, UP_20K),
		// NC
		PAD_NC(GPP_D19, UP_20K),
		// NC
		PAD_NC(GPP_D20, UP_20K),

	// SPI1
		// TPM_DET# (currently unused in cb; there seem to be no devices without TPM)
		PAD_NC(GPP_D21, NONE),
		// DDR_TYPE_D22 (unused in cb; there is only one on-board ram type)
		PAD_NC(GPP_D22, NONE),

	// I2S
		// NC
		PAD_NC(GPP_D23, UP_20K),

// GPP_E
	// SATA
		// NC
		PAD_NC(GPP_E0, UP_20K),
		// SATAXPCIE1 / SATAGP1
		PAD_CFG_NF(GPP_E1, NONE, DEEP, NF1),
		// SATAXPCIE2 / SATAGP2
		PAD_CFG_NF(GPP_E2, NONE, DEEP, NF1),

	// CPU Misc
		// NC
		PAD_NC(GPP_E3, UP_20K),

	// DEVSLP
		// NC
		PAD_NC(GPP_E4, UP_20K),
		// DEVSLP1
		PAD_CFG_NF(GPP_E5, NONE, DEEP, NF1),
		// DEVSLP2
		PAD_CFG_NF(GPP_E6, NONE, DEEP, NF1),

	// CPU Misc
		// NC
		PAD_NC(GPP_E7, UP_20K),

	// SATA
		// NC
		PAD_NC(GPP_E8, NONE),

	// USB2
		// NC
		PAD_NC(GPP_E9, NONE),
		// NC
		PAD_NC(GPP_E10, NONE),
		// NC
		PAD_NC(GPP_E11, NONE),
		// NC
		PAD_NC(GPP_E12, NONE),

	// Display Signals
		// DDPB_HPD0 / MUX_HPD
		PAD_CFG_NF(GPP_E13, NONE, DEEP, NF1),
		// DDPC_HPD1 / HDMI_HPD
		PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1),
		// SMI#
		_PAD_CFG_STRUCT(GPP_E15, 0x42840100, 0x0),
		// SCI#
		_PAD_CFG_STRUCT(GPP_E16, 0x80880100, 0x0000),
		// EDP_HPD
		PAD_CFG_NF(GPP_E17, NONE, DEEP, NF1),
		// NC
		PAD_NC(GPP_E18, UP_20K),
		// NC
		PAD_NC(GPP_E19, NONE),
		// DPPC_CTRLCLK / HDMI_CTRLCLK
		PAD_CFG_NF(GPP_E20, NONE, DEEP, NF1),
		// DPPC_CTRLDATA / HDMI_CTRLDATA
		PAD_CFG_NF(GPP_E21, NONE, DEEP, NF1),
		// NC
		PAD_NC(GPP_E22, UP_20K),
		// NC
		PAD_NC(GPP_E23, UP_20K),

// GPP_F
	// CNVI
		// NC
		PAD_NC(GPP_F0, UP_20K),

	// GPIO
		// NC
		PAD_NC(GPP_F1, UP_20K),
		// NC
		PAD_NC(GPP_F2, UP_20K),
		// NC
		PAD_NC(GPP_F3, UP_20K),

	// CNVI
		// CNV_BRI_DT / CNVI_BRI_DT
		PAD_CFG_NF(GPP_F4, NONE, DEEP, NF1),
		// CNV_BRI_RSP / CNVI_BRI_RSP
		PAD_CFG_NF(GPP_F5, UP_20K, DEEP, NF1),
		// CNV_RGI_DT / CNVI_RGI_DT
		PAD_CFG_NF(GPP_F6, NONE, DEEP, NF1),
		// CNV_RGI_RSP / CNVI_RGI_RSP
		PAD_CFG_NF(GPP_F7, UP_20K, DEEP, NF1),
		// NC
		PAD_NC(GPP_F8, UP_20K),
		// NC
		PAD_NC(GPP_F9, UP_20K),

	// GPIO
		// NC
		PAD_NC(GPP_F10, UP_20K),

	// EMMC
		// NC
		PAD_NC(GPP_F11, UP_20K),
		// NC
		PAD_NC(GPP_F12, UP_20K),
		// NC
		PAD_NC(GPP_F13, UP_20K),
		// NC
		PAD_NC(GPP_F14, UP_20K),
		// NC
		PAD_NC(GPP_F15, UP_20K),
		// NC
		PAD_NC(GPP_F16, UP_20K),
		// NC
		PAD_NC(GPP_F17, UP_20K),
		// NC
		PAD_NC(GPP_F18, UP_20K),
		// NC
		PAD_NC(GPP_F19, UP_20K),
		// NC
		PAD_NC(GPP_F20, UP_20K),
		// NC
		PAD_NC(GPP_F21, UP_20K),
		// NC
		PAD_NC(GPP_F22, UP_20K),

	// A4WP
		// NC
		PAD_NC(GPP_F23, NONE),

// GPP_G
	// SD
		// NC
		PAD_NC(GPP_G0, UP_20K),
		// NC
		PAD_NC(GPP_G1, UP_20K),
		// NC
		PAD_NC(GPP_G2, UP_20K),
		// NC
		PAD_NC(GPP_G3, UP_20K),
		// NC
		PAD_NC(GPP_G4, UP_20K),
		// NC
		PAD_NC(GPP_G5, UP_20K),
		// NC
		PAD_NC(GPP_G6, UP_20K),
		// NC
		PAD_NC(GPP_G7, UP_20K),

// GPP_H
	// CNVI
		// NC
		PAD_NC(GPP_H0, UP_20K),
		// CNV_RF_RESET# / CNVI_RST#
		PAD_CFG_NF(GPP_H1, NONE, DEEP, NF3),
		// MODEM_CLKREQ / CNVI_CLKREQ
		PAD_CFG_NF(GPP_H2, NONE, DEEP, NF3),
		// NC
		PAD_NC(GPP_H3, UP_20K),

	// I2C
		// I2C2_SDA / SMD_7411
		PAD_CFG_NF(GPP_H4, NONE, DEEP, NF1),
		// I2C2_SCL / SMC_7411
		PAD_CFG_NF(GPP_H5, NONE, DEEP, NF1),
		// NC
		PAD_NC(GPP_H6, UP_20K),
		// NC
		PAD_NC(GPP_H7, UP_20K),
		// NC
		PAD_NC(GPP_H8, UP_20K),
		// NC
		PAD_NC(GPP_H9, UP_20K),

	// I2C
		// NC
		PAD_NC(GPP_H10, UP_20K),
		// NC
		PAD_NC(GPP_H11, UP_20K),

	// PCIE
		// NC
		PAD_NC(GPP_H12, UP_20K),
		// NC
		PAD_NC(GPP_H13, UP_20K),
		// NC
		PAD_NC(GPP_H14, UP_20K),
		// NC
		PAD_NC(GPP_H15, UP_20K),

	// Display Signals
		// NC
		PAD_NC(GPP_H16, UP_20K),
		// NC
		PAD_NC(GPP_H17, UP_20K),

	// CPU Power
		// CPU_C10_GATE#
		PAD_CFG_NF(GPP_H18, NONE, DEEP, NF1),

	// TIMESYNC
		// NC
		PAD_NC(GPP_H19, UP_20K),

	// IMGCLKOUT
		// NC
		PAD_NC(GPP_H20, UP_20K),

	// GPIO
		// NC
		PAD_NC(GPP_H21, NONE),
		// NC
		PAD_NC(GPP_H22, UP_20K),
		// NC
		PAD_NC(GPP_H23, UP_20K),
};

#endif

#endif
