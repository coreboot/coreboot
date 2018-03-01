/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 - 2017 Intel Corporation.
 * Copyright (C) 2017 - 2018 Online SAS.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _MAINBOARD_GPIO_H
#define _MAINBOARD_GPIO_H

#include <soc/gpio_dnv.h>

#ifndef __ACPI__
const struct dnv_pad_config tagada_gpio_config[] = {
	// GBE0_SDP0 (GPIO_14) NC
/*ME	{ NORTH_ALL_GBE0_SDP0, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } }, */
	// GBE1_SDP0 (GPIO_15) NC
	{ NORTH_ALL_GBE1_SDP0, { GpioPadModeNative1, GpioHostOwnGpio,
	  GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// GBE2_I2C_CLK (GPIO_16) NC
	{ NORTH_ALL_GBE0_SDP1, { GpioPadModeGpio, GpioHostOwnGpio,
	  GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// GBE2_I2C_DATA (GPIO_17) NC
	{ NORTH_ALL_GBE1_SDP1, { GpioPadModeGpio, GpioHostOwnGpio,
	  GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// GBE2_SDP0 (GPIO_18) NC
	{ NORTH_ALL_GBE0_SDP2, { GpioPadModeNative2, GpioHostOwnGpio,
	  GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// GBE3_SDP0 (GPIO_19) NC
	{ NORTH_ALL_GBE1_SDP2, { GpioPadModeNative2, GpioHostOwnGpio,
	  GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// GBE3_I2C_CLK (GPIO_20) NC
	{ NORTH_ALL_GBE0_SDP3, { GpioPadModeGpio, GpioHostOwnGpio,
	  GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// GBE3_I2C_DATA (GPIO_21) NC
	{ NORTH_ALL_GBE1_SDP3, { GpioPadModeGpio, GpioHostOwnGpio,
	  GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// GBE2_LED0 (GPIO_22) Z1:NC / A0:ETH0_LED0
	{ NORTH_ALL_GBE2_LED0, { GpioPadModeNative1, GpioHostOwnGpio,
	  GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// GBE2_LED1 (GPIO_23) Z1:NC / A0:ETH0_LED1
	{ NORTH_ALL_GBE2_LED1, { GpioPadModeNative1, GpioHostOwnGpio,
	  GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// GBE0_I2C_CLK (GPIO_24) NC
	{ NORTH_ALL_GBE0_I2C_CLK, { GpioPadModeGpio, GpioHostOwnGpio,
	  GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// GBE0_I2C_DATA (GPIO_25) NC
	{ NORTH_ALL_GBE0_I2C_DATA, { GpioPadModeGpio, GpioHostOwnGpio,
	  GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// GBE1_I2C_CLK (GPIO_26) NC
	{ NORTH_ALL_GBE1_I2C_CLK, { GpioPadModeGpio, GpioHostOwnGpio,
	  GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// GBE1_I2C_DATA (GPIO_27) NC
	{ NORTH_ALL_GBE1_I2C_DATA, { GpioPadModeGpio, GpioHostOwnGpio,
	  GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// NCSI_RXD0 (GPIO_28) NC
	{ NORTH_ALL_NCSI_RXD0, { GpioPadModeNative2, GpioHostOwnGpio,
	  GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// NCSI_CLK_IN (GPIO_29) Pull Down
	{ NORTH_ALL_NCSI_CLK_IN, { GpioPadModeNative2, GpioHostOwnGpio,
	  GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// NCSI_RXD1 (GPIO_30) NC
	{ NORTH_ALL_NCSI_RXD1, { GpioPadModeNative2, GpioHostOwnGpio,
	  GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// NCSI_CRS_DV (GPIO_31) NC
	{ NORTH_ALL_NCSI_CRS_DV, { GpioPadModeNative2, GpioHostOwnGpio,
	  GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// NCSI_ARB_IN (GPIO_32) NC
	{ NORTH_ALL_NCSI_ARB_IN, { GpioPadModeNative2, GpioHostOwnGpio,
	  GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// NCSI_TX_EN (GPIO_33) Pull Down
	{ NORTH_ALL_NCSI_TX_EN, { GpioPadModeNative2, GpioHostOwnGpio,
	  GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// NCSI_TXD0 (GPIO_34) Pull Down
	{ NORTH_ALL_NCSI_TXD0, { GpioPadModeNative2, GpioHostOwnGpio,
	  GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// NCSI_TXD1 (GPIO_35) Pull Down
	{ NORTH_ALL_NCSI_TXD1, { GpioPadModeNative2, GpioHostOwnGpio,
	  GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// NCSI_ARB_OUT (GPIO_36) NC
	{ NORTH_ALL_NCSI_ARB_OUT, { GpioPadModeNative2, GpioHostOwnGpio,
	  GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// GBE0_LED0 (GPIO_37) Z1:ETH0_LED0 / A1:ETH1_LED0
	{ NORTH_ALL_GBE0_LED0, { GpioPadModeNative1, GpioHostOwnGpio,
	  GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// GBE0_LED1 (GPIO_38) Z1:ETH0_LED1 / A1:ETH1_LED1
	{ NORTH_ALL_GBE0_LED1, { GpioPadModeNative1, GpioHostOwnGpio,
	  GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// GBE1_LED0 (GPIO_39) Z1:ETH1_LED0 / A1:NC
	{ NORTH_ALL_GBE1_LED0, { GpioPadModeNative1, GpioHostOwnGpio,
	  GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// GBE1_LED1 (GPIO_40) Z1:ETH1_LED1 / A1:NC
	{ NORTH_ALL_GBE1_LED1, { GpioPadModeNative1, GpioHostOwnGpio,
	  GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// ADR-COMPLETE (GPIO_0) LFFF: DVT_GPIO<0> : BOOTED, output
	{ NORTH_ALL_GPIO_0, { GpioPadModeGpio, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock } },
	// PCIE_CLKREQ0_N (GPIO_41) Pull Up
	{ NORTH_ALL_PCIE_CLKREQ0_N, { GpioPadModeNative1, GpioHostOwnGpio,
	  GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// PCIE_CLKREQ1_N (GPIO_42) Pull Up
	{ NORTH_ALL_PCIE_CLKREQ1_N, { GpioPadModeNative1, GpioHostOwnGpio,
	  GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// PCIE_CLKREQ2_N (GPIO_43) Pull Up
	{ NORTH_ALL_PCIE_CLKREQ2_N, { GpioPadModeNative1, GpioHostOwnGpio,
	  GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// PCIE_CLKREQ3_N (GPIO_44) Pull Up
	{ NORTH_ALL_PCIE_CLKREQ3_N, { GpioPadModeNative1, GpioHostOwnGpio,
	  GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// PCIE_CLKREQ4_N (GPIO_45) Pull Up
	{ NORTH_ALL_PCIE_CLKREQ4_N, { GpioPadModeNative1, GpioHostOwnGpio,
	  GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// GBE_MDC (GPIO_1) NC
	{ NORTH_ALL_GPIO_1, { GpioPadModeNative1, GpioHostOwnGpio,
	  GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// GBE_MDIO  (GPIO_2) NC
	{ NORTH_ALL_GPIO_2, { GpioPadModeNative1, GpioHostOwnGpio,
	  GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
	  GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SVID_ALERT_N (GPIO_47) SVID_ALERTn
	{ NORTH_ALL_SVID_ALERT_N, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SVID_DATA (GPIO_48) SVID_DATA
	{ NORTH_ALL_SVID_DATA, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SVID_CLK (GPIO_49) SVID_CLK
	{ NORTH_ALL_SVID_CLK, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// THERMTRIP_N (GPIO_50) SOC_THERMTRIPn Pull Up
	{ NORTH_ALL_THERMTRIP_N, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// PROCHOT_N (GPIO_51) PROCHOTn Pull Up
	{ NORTH_ALL_PROCHOT_N, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// MEMHOT_N (GPIO_52) SOC_MEMHOTn
	{ NORTH_ALL_MEMHOT_N, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// DFX_PORT_CLK0 (GPIO_53) NC
	{ SOUTH_DFX_DFX_PORT_CLK0, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// DFX_PORT_CLK1 (GPIO_54) NC
	{ SOUTH_DFX_DFX_PORT_CLK1, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// DFX_PORT0 (GPIO_55) NC
	{ SOUTH_DFX_DFX_PORT0, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// DFX_PORT1 (GPIO_56) NC
	{ SOUTH_DFX_DFX_PORT1, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// DFX_PORT2 (GPIO_57) NC
	{ SOUTH_DFX_DFX_PORT2, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// DFX_PORT3 (GPIO_58) NC
	{ SOUTH_DFX_DFX_PORT3, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// DFX_PORT4 (GPIO_59) NC
	{ SOUTH_DFX_DFX_PORT4, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// DFX_PORT5 (GPIO_60) NC
	{ SOUTH_DFX_DFX_PORT5, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// DFX_PORT6 (GPIO_61) NC
	{ SOUTH_DFX_DFX_PORT6, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// DFX_PORT7 (GPIO_62) NC
	{ SOUTH_DFX_DFX_PORT7, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// DFX_PORT8 (GPIO_63) NC
	{ SOUTH_DFX_DFX_PORT8, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// DFX_PORT9 (GPIO_134) NC
	{ SOUTH_DFX_DFX_PORT9, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// DFX_PORT10 (GPIO_135) NC
	{ SOUTH_DFX_DFX_PORT10, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// DFX_PORT11 (GPIO_136) NC
	{ SOUTH_DFX_DFX_PORT11, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// DFX_PORT12 (GPIO_137) NC
	{ SOUTH_DFX_DFX_PORT12, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// DFX_PORT13 (GPIO_138) NC
	{ SOUTH_DFX_DFX_PORT13, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// DFX_PORT14 (GPIO_139) NC
	{ SOUTH_DFX_DFX_PORT14, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// DFX_PORT15 (GPIO_140) NC
	{ SOUTH_DFX_DFX_PORT15, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SPI_TPM_CS_N (GPIO_12) HS_TCO_WDT NC (Possible Pull Up)
	{ SOUTH_GROUP0_GPIO_12, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SMB5_GBE_ALRT_N (GPIO_13) LAN_ALRTn Pull Up
	{ SOUTH_GROUP0_SMB5_GBE_ALRT_N, { GpioPadModeNative3, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// PCIE_CLKREQ5_N (GPIO_98) Pull Up
	{ SOUTH_GROUP0_PCIE_CLKREQ5_N, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// PCIE_CLKREQ6_N (GPIO_99) Pull Up
	{ SOUTH_GROUP0_PCIE_CLKREQ6_N, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// PCIE_CLKREQ7_N (GPIO_100) Pull Up
	{ SOUTH_GROUP0_PCIE_CLKREQ7_N, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// UART0_RXD (GPIO_101) CONSOLE_RX
	{ SOUTH_GROUP0_UART0_RXD, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// UART0_TXD (GPIO_102) CONSOLE_TX
	{ SOUTH_GROUP0_UART0_TXD, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SMB5_GBE_CLK (GPIO_103) LAN_SLC Pull Up
	{ SOUTH_GROUP0_SMB5_GBE_CLK, { GpioPadModeNative3, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SMB_GBE_DATA (GPIO_104) LAN_SDA Pull UP
	{ SOUTH_GROUP0_SMB5_GBE_DATA, { GpioPadModeNative3, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// ERROR2_N (GPIO_105) ERRORn2
	{ SOUTH_GROUP0_ERROR2_N, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// ERROR1_N (GPIO_106) ERRORn1
	{ SOUTH_GROUP0_ERROR1_N, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// ERROR0_N (GPIO_107) ERRORn0 Pull Up
	{ SOUTH_GROUP0_ERROR0_N, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// IERR_N (CATERR_N) (GPIO_108) IERRn (HardStrap Pull Up)
	{ SOUTH_GROUP0_IERR_N, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// MCERR_N  (GPIO_109) MCERR
	{ SOUTH_GROUP0_MCERR_N, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SMB0_LEG_CLK (GPIO_110) LEG_SCL Pull Up
	{ SOUTH_GROUP0_SMB0_LEG_CLK, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SMB0_LEG_DATA (GPIO_111) LEG_SDA Pull Up
	{ SOUTH_GROUP0_SMB0_LEG_DATA, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SMB0_LEG_ALRT_N (GPIO_112) Pull Up
	{ SOUTH_GROUP0_SMB0_LEG_ALRT_N, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SMB1_HOST_DATA (GPIO_113) HOST_SDA Pull Up
/*ME	{ SOUTH_GROUP0_SMB1_HOST_DATA, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },*/
	// SMB1_HOST_CLK (GPIO_114) HOST_SCL Pull Up
/*ME	{ SOUTH_GROUP0_SMB1_HOST_CLK, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },*/
	// SMB2_PECI_DATA (GPIO_115) Pull Up
	{ SOUTH_GROUP0_SMB2_PECI_DATA, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SMB2_PECI_CLK (GPIO_116) Pull Up
	{ SOUTH_GROUP0_SMB2_PECI_CLK, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SMB4_CSME0_DATA (GPIO_117) ME_SDA Pull Up
/*ME	{ SOUTH_GROUP0_SMB4_CSME0_DATA, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },*/
	// SMB4_CSME0_CLK (GPIO_118) ME_SCL Pull Up
/*ME	{ SOUTH_GROUP0_SMB4_CSME0_CLK, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },*/
	// SMB4_CSME0_ALRT_N (GPIO_119) ME_ALRTn Pull Up
	{ SOUTH_GROUP0_SMB4_CSME0_ALRT_N, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// USB_OC0_N (GPIO_120) Pull Up
	{ SOUTH_GROUP0_USB_OC0_N, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// FLEX_CLK_SE0 (GPIO_121) NC
	{ SOUTH_GROUP0_FLEX_CLK_SE0, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// FLEX_CLK_SE1 (GPIO_122) NC
	{ SOUTH_GROUP0_FLEX_CLK_SE1, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// GBE3_LED1 (GPIO_4) LFFF: M2A_CFGn : M2A_SATAn, input
	{ SOUTH_GROUP0_GPIO_4, { GpioPadModeGpio, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SMB3_IE0_CLK (GPIO_5) LFFF: M2B_CFGn : M2B_SATAn, input
	{ SOUTH_GROUP0_GPIO_5, { GpioPadModeGpio, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SMB3_IE0_DATA (GPIO_6) NC
	{ SOUTH_GROUP0_GPIO_6, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SMB3_IE0_ALERT_N (GPIO_7) NC
	{ SOUTH_GROUP0_GPIO_7, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SATA0_LED (GPIO_90) SATA_LED0 Pull Up
	{ SOUTH_GROUP0_SATA0_LED_N, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SATA1_LED (GPIO_91) SATA_LED1 Pull Up
	{ SOUTH_GROUP0_SATA1_LED_N, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SATA_PDETECT0 (GPIO_92) Pull Up
	{ SOUTH_GROUP0_SATA_PDETECT0, { GpioPadModeNative2, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SATA_PDETECT1 (GPIO_93) Pull Up
	{ SOUTH_GROUP0_SATA_PDETECT1, { GpioPadModeNative2, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// UART1_RTS (GPIO_94) NC (Possible Pull Up)
	{ SOUTH_GROUP0_SATA0_SDOUT, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// UART1_CTS (GPIO_95) NC (Possible Pull Up)
	{ SOUTH_GROUP0_SATA1_SDOUT, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// UART1_RXD (GPIO_96) NC
	{ SOUTH_GROUP0_UART1_RXD, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// UART1_TXD (GPIO_97) NC
	{ SOUTH_GROUP0_UART1_TXD, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SMB6_CSME1_DATA (GPIO_8) LFFF: DVT_GPIO<1> : Baud select, input
	{ SOUTH_GROUP0_GPIO_8, { GpioPadModeGpio, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDis, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SMB6_CSME1_CLK (GPIO_9) LFFF: DVT_GPIO<2> : Verbose Traces, input
	{ SOUTH_GROUP0_GPIO_9, { GpioPadModeGpio, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDis, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// TCK (GPIO_141) n/a NC
	{ SOUTH_GROUP0_TCK, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// TRST_N (GPIO_142) n/a NC
	{ SOUTH_GROUP0_TRST_N, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// TMS (GPIO_143) n/a NC
	{ SOUTH_GROUP0_TMS, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// TDI (GPIO_144) n/a NC
	{ SOUTH_GROUP0_TDI, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// TDO (GPIO_145) n/a NC
	{ SOUTH_GROUP0_TDO, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// CX_PRDY_N (GPIO_146) NC
	{ SOUTH_GROUP0_CX_PRDY_N, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// CX-PREQ_N (GPIO_147) Pull Up
	{ SOUTH_GROUP0_CX_PREQ_N, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// ME_RECVR_HDR (GPIO_148) ME_RECVR Pull Up
/*ME	{ SOUTH_GROUP0_CTBTRIGINOUT, { GpioPadModeGpio, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },*/
	// ADV_DBG_DFX_HDR (GPIO_149) NC
	{ SOUTH_GROUP0_CTBTRIGOUT, { GpioPadModeGpio, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// LAD2_SPI_IRQ_N (GPIO_150) NC
	{ SOUTH_GROUP0_DFX_SPARE2, { GpioPadModeGpio, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SMB_PECI_ALRT_N (GPIO_151) Pull Up
	{ SOUTH_GROUP0_DFX_SPARE3, { GpioPadModeGpio, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SMB_CSME1_ALRT_N (GPIO_152) NC
	{ SOUTH_GROUP0_DFX_SPARE4, { GpioPadModeGpio, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SUSPWRDNACK (GPIO_79) SUSPWRDNACK Pull Up
	{ SOUTH_GROUP1_SUSPWRDNACK, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// PMU_SUSCLK (GPIO_80) PMU_SUSCLK
	{ SOUTH_GROUP1_PMU_SUSCLK, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// ADR_TRIGGER_N (GPIO_81) Pull Down
	{ SOUTH_GROUP1_ADR_TRIGGER, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// PMU_SLP_S45_N (GPIO_82) SLP_S45n
	{ SOUTH_GROUP1_PMU_SLP_S45_N, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// PMU_SLP_S3_N (GPIO_83) SLP_S3n
	{ SOUTH_GROUP1_PMU_SLP_S3_N, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// PMU_WAKE_N (GPIO_84) PMU_WAKEn Pull Up
	{ SOUTH_GROUP1_PMU_WAKE_N, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// PMU_PWRBTN_N (GPIO_85) PWNBTNn
	{ SOUTH_GROUP1_PMU_PWRBTN_N, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// PMU_RESETBUTTON_N (GPIO_86) RSTBTNn
	{ SOUTH_GROUP1_PMU_RESETBUTTON_N, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// PMU_PLTRST_N (GPIO_87) PLTRSTn
	{ SOUTH_GROUP1_PMU_PLTRST_N, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// PMU_SUS_STAT_N (GPIO_88) SUS_STATn
	{ SOUTH_GROUP1_SUS_STAT_N, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// TDB_CIO_PLUG_EVENT (GPIO_89) NC
	{ SOUTH_GROUP1_SLP_S0IX_N, { GpioPadModeGpio, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SPI_CS0_N (GPIO_72) SPI_CS0
	{ SOUTH_GROUP1_SPI_CS0_N, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SPI_CS1_N (GPIO_73) NC
	{ SOUTH_GROUP1_SPI_CS1_N, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SPI_MOSI_IO0 (GPIO_74) SPI_MOSI
	{ SOUTH_GROUP1_SPI_MOSI_IO0, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SPI_MISO_IO1 (GPIO_75) SPI_MISO
	{ SOUTH_GROUP1_SPI_MISO_IO1, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SPI_IO2 (GPIO_76) NC
	{ SOUTH_GROUP1_SPI_IO2, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SPI_IO3 (GPIO_77) NC
	{ SOUTH_GROUP1_SPI_IO3, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// SPI_CLK (GPIO_78) SPI_CLK
	{ SOUTH_GROUP1_SPI_CLK, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// LPC_AD0 (GPIO_64) NC
	{ SOUTH_GROUP1_ESPI_IO0, { GpioPadModeNative2, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// LPC_AD1 (GPIO_65) NC
	{ SOUTH_GROUP1_ESPI_IO1, { GpioPadModeNative2, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// LPC_AD2 (GPIO_66) NC
	{ SOUTH_GROUP1_ESPI_IO2, { GpioPadModeNative2, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// LPC_AD3 (GPIO_67) NC
	{ SOUTH_GROUP1_ESPI_IO3, { GpioPadModeNative2, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// LPC_FRAME_N (GPIO_68) NC
	{ SOUTH_GROUP1_ESPI_CS0_N, { GpioPadModeNative2, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// LPC_CLKOUT0 (GPIO_69) NC
	{ SOUTH_GROUP1_ESPI_CLK, { GpioPadModeNative2, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// LPC_CLKOUT1 (GPIO_70) NC
	{ SOUTH_GROUP1_ESPI_RST_N, { GpioPadModeNative2, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// LPC_CLKRUN_N (GPIO_71) Pull Up
	{ SOUTH_GROUP1_ESPI_ALRT0_N, { GpioPadModeNative2, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// MFG_MODE_HDR (GPIO_10) MFG_MODE Pull Up
	{ SOUTH_GROUP1_GPIO_10, { GpioPadModeGpio, GpioHostOwnGpio,
          GpioDirIn, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// LPC_SERIRQ (GPIO_11) NC
	{ SOUTH_GROUP1_GPIO_11, { GpioPadModeNative2, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// EMMC-CMD (GPIO_123) NC
	{ SOUTH_GROUP1_EMMC_CMD, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// EMMC-CSTROBE (GPIO_124) NC
	{ SOUTH_GROUP1_EMMC_STROBE, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// EMMC-CLK (GPIO_125) NC
	{ SOUTH_GROUP1_EMMC_CLK, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
	// EMMC-D0 (GPIO_126) NC
	{ SOUTH_GROUP1_EMMC_D0, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermWpu20K, GpioPadConfigLock | GpioOutputStateLock } },
	// EMMC-D1 (GPIO_127) NC
	{ SOUTH_GROUP1_EMMC_D1, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermWpu20K, GpioPadConfigLock | GpioOutputStateLock } },
	// EMMC-D2 (GPIO_128) NC
	{ SOUTH_GROUP1_EMMC_D2, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermWpu20K, GpioPadConfigLock | GpioOutputStateLock } },
	// EMMC-D3 (GPIO_129) NC
	{ SOUTH_GROUP1_EMMC_D3, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermWpu20K, GpioPadConfigLock | GpioOutputStateLock } },
	// EMMC-D4 (GPIO_130) NC
	{ SOUTH_GROUP1_EMMC_D4, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermWpu20K, GpioPadConfigLock | GpioOutputStateLock } },
	// EMMC-D5 (GPIO_131) NC
	{ SOUTH_GROUP1_EMMC_D5, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermWpu20K, GpioPadConfigLock | GpioOutputStateLock } },
	// EMMC-D6 (GPIO_132) NC
	{ SOUTH_GROUP1_EMMC_D6, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermWpu20K, GpioPadConfigLock | GpioOutputStateLock } },
	// EMMC-D7 (GPIO_133) NC
	{ SOUTH_GROUP1_EMMC_D7, { GpioPadModeNative1, GpioHostOwnGpio,
          GpioDirInOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermWpu20K, GpioPadConfigLock | GpioOutputStateLock } },
	// IE_ROM GPIO (GPIO_3) HS_TSO NC (Possible Pull Up)
	{ SOUTH_GROUP1_GPIO_3, { GpioPadModeGpio, GpioHostOwnGpio,
          GpioDirOut, GpioOutDefault, GpioIntDefault, GpioResetPwrGood,
          GpioTermDefault, GpioPadConfigLock | GpioOutputStateLock } },
};
#endif

#endif /* _MAINBOARD_GPIO_H */
