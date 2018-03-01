/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 - 2017 Intel Corporation.
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
const struct dnv_pad_config harcuvar_gpio_table[] = {
	// GBE0_SDP0 (GPIO_14)
	{NORTH_ALL_GBE0_SDP0,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// GBE1_SDP0 (GPIO_15)
	{NORTH_ALL_GBE1_SDP0,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// GBE2_I2C_CLK (GPIO_16)
	{NORTH_ALL_GBE0_SDP1,
	 {GpioPadModeNative2, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// GBE2_I2C_DATA (GPIO_17)
	{NORTH_ALL_GBE1_SDP1,
	 {GpioPadModeNative2, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// GBE2_SDP0 (GPIO_18)
	{NORTH_ALL_GBE0_SDP2,
	 {GpioPadModeNative2, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// GBE3_SDP0 (GPIO_19)
	{NORTH_ALL_GBE1_SDP2,
	 {GpioPadModeNative2, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// GBE3_I2C_CLK (GPIO_20)
	{NORTH_ALL_GBE0_SDP3,
	 {GpioPadModeNative2, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// GBE3_I2C_DATA (GPIO_21)
	{NORTH_ALL_GBE1_SDP3,
	 {GpioPadModeNative2, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// GBE2_LED0 (GPIO_22)
	{NORTH_ALL_GBE2_LED0,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// GBE2_LED1 (GPIO_23)
	{NORTH_ALL_GBE2_LED1,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// GBE0_I2C_CLK (GPIO_24)
	{NORTH_ALL_GBE0_I2C_CLK,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// GBE0_I2C_DATA (GPIO_25)
	{NORTH_ALL_GBE0_I2C_DATA,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// GBE1_I2C_CLK (GPIO_26)
	{NORTH_ALL_GBE1_I2C_CLK,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// GBE1_I2C_DATA (GPIO_27)
	{NORTH_ALL_GBE1_I2C_DATA,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// NCSI_RXD0 (GPIO_28)
	{NORTH_ALL_NCSI_RXD0,
	 {GpioPadModeNative2, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetPwrGood, GpioTermDefault, GpioLockDefault} },
	// NCSI_CLK_IN (GPIO_29)
	{NORTH_ALL_NCSI_CLK_IN,
	 {GpioPadModeNative2, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetPwrGood, GpioTermDefault, GpioLockDefault} },
	// NCSI_RXD1 (GPIO_30)
	{NORTH_ALL_NCSI_RXD1,
	 {GpioPadModeNative2, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetPwrGood, GpioTermDefault, GpioLockDefault} },
	// NCSI_CRS_DV (GPIO_31)
	{NORTH_ALL_NCSI_CRS_DV,
	 {GpioPadModeNative2, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetPwrGood, GpioTermDefault, GpioLockDefault} },
	// NCSI_ARB_IN (GPIO_32)
	{NORTH_ALL_NCSI_ARB_IN,
	 {GpioPadModeNative2, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetPwrGood, GpioTermDefault, GpioLockDefault} },
	// NCSI_TX_EN (GPIO_33)
	{NORTH_ALL_NCSI_TX_EN,
	 {GpioPadModeNative2, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetPwrGood, GpioTermDefault, GpioLockDefault} },
	// NCSI_TXD0 (GPIO_34)
	{NORTH_ALL_NCSI_TXD0,
	 {GpioPadModeNative2, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetPwrGood, GpioTermDefault, GpioLockDefault} },
	// NCSI_TXD1 (GPIO_35)
	{NORTH_ALL_NCSI_TXD1,
	 {GpioPadModeNative2, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetPwrGood, GpioTermDefault, GpioLockDefault} },
	// NCSI_ARB_OUT (GPIO_36)
	{NORTH_ALL_NCSI_ARB_OUT,
	 {GpioPadModeNative2, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetPwrGood, GpioTermDefault, GpioLockDefault} },
	// GBE0_LED0 (GPIO_37)
	{NORTH_ALL_GBE0_LED0,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetPwrGood, GpioTermDefault, GpioLockDefault} },
	// GBE0_LED1 (GPIO_38)
	{NORTH_ALL_GBE0_LED1,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetPwrGood, GpioTermDefault, GpioLockDefault} },
	// GBE1_LED0 (GPIO_39)
	{NORTH_ALL_GBE1_LED0,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetPwrGood, GpioTermDefault, GpioLockDefault} },
	// GBE1_LED1 (GPIO_40)
	{NORTH_ALL_GBE1_LED1,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetPwrGood, GpioTermDefault, GpioLockDefault} },
	// ADR-COMPLETE (GPIO_0)
	{NORTH_ALL_GPIO_0,
	 {GpioPadModeNative3, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// PCIE_CLKREQ0_N (GPIO_41)
	{NORTH_ALL_PCIE_CLKREQ0_N,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// PCIE_CLKREQ1_N (GPIO_42)
	{NORTH_ALL_PCIE_CLKREQ1_N,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// PCIE_CLKREQ2_N (GPIO_43)
	{NORTH_ALL_PCIE_CLKREQ2_N,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// PCIE_CLKREQ3_N (GPIO_44)
	{NORTH_ALL_PCIE_CLKREQ3_N,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// FORCE_POWER (GPIO_45)
	{NORTH_ALL_PCIE_CLKREQ4_N,
	 {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// GBE_MDC (GPIO_1)
	{NORTH_ALL_GPIO_1,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// GBE_MDIO  (GPIO_2)
	{NORTH_ALL_GPIO_2,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SVID_ALERT_N (GPIO_47)
	{NORTH_ALL_SVID_ALERT_N,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SVID_DATA (GPIO_48)
	{NORTH_ALL_SVID_DATA,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SVID_CLK (GPIO_49)
	{NORTH_ALL_SVID_CLK,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// THERMTRIP_N (GPIO_50)
	{NORTH_ALL_THERMTRIP_N,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// PROCHOT_N (GPIO_51)
	{NORTH_ALL_PROCHOT_N,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// MEMHOT_N (GPIO_52)
	{NORTH_ALL_MEMHOT_N,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// DFX_PORT_CLK0 (GPIO_53)
	{SOUTH_DFX_DFX_PORT_CLK0,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// DFX_PORT_CLK1 (GPIO_54)
	{SOUTH_DFX_DFX_PORT_CLK1,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// DFX_PORT0 (GPIO_55)
	{SOUTH_DFX_DFX_PORT0,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// DFX_PORT1 (GPIO_56)
	{SOUTH_DFX_DFX_PORT1,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// DFX_PORT2 (GPIO_57)
	{SOUTH_DFX_DFX_PORT2,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// DFX_PORT3 (GPIO_58)
	{SOUTH_DFX_DFX_PORT3,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// DFX_PORT4 (GPIO_59)
	{SOUTH_DFX_DFX_PORT4,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// DFX_PORT5 (GPIO_60)
	{SOUTH_DFX_DFX_PORT5,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// DFX_PORT6 (GPIO_61)
	{SOUTH_DFX_DFX_PORT6,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// DFX_PORT7 (GPIO_62)
	{SOUTH_DFX_DFX_PORT7,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// DFX_PORT8 (GPIO_63)
	{SOUTH_DFX_DFX_PORT8,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// DFX_PORT9 (GPIO_134)
	{SOUTH_DFX_DFX_PORT9,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// DFX_PORT10 (GPIO_135)
	{SOUTH_DFX_DFX_PORT10,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// DFX_PORT11 (GPIO_136)
	{SOUTH_DFX_DFX_PORT11,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// DFX_PORT12 (GPIO_137)
	{SOUTH_DFX_DFX_PORT12,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// DFX_PORT13 (GPIO_138)
	{SOUTH_DFX_DFX_PORT13,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// DFX_PORT14 (GPIO_139)
	{SOUTH_DFX_DFX_PORT14,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// DFX_PORT15 (GPIO_140)
	{SOUTH_DFX_DFX_PORT15,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SPI_TPM_CS_N (GPIO_12)
	{SOUTH_GROUP0_GPIO_12,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SMB5_GBE_ALRT_N (GPIO_13)
	{SOUTH_GROUP0_SMB5_GBE_ALRT_N,
	 {GpioPadModeNative3, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetPwrGood, GpioTermDefault, GpioLockDefault} },
	// SMI (GPIO_98)
	{SOUTH_GROUP0_PCIE_CLKREQ5_N,
	 {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntSmi, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// NMI (GPIO_99)
	{SOUTH_GROUP0_PCIE_CLKREQ6_N,
	 {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntNmi, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// GBE3_LED0 (GPIO_100)
	{SOUTH_GROUP0_PCIE_CLKREQ7_N,
	 {GpioPadModeNative3, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// UART0_RXD (GPIO_101)
	{SOUTH_GROUP0_UART0_RXD,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// UART0_TXD (GPIO_102)
	{SOUTH_GROUP0_UART0_TXD,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SMB5_GBE_CLK (GPIO_103)
	{SOUTH_GROUP0_SMB5_GBE_CLK,
	 {GpioPadModeNative3, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetPwrGood, GpioTermDefault, GpioLockDefault} },
	// SMB_GBE_DATA (GPIO_104)
	{SOUTH_GROUP0_SMB5_GBE_DATA,
	 {GpioPadModeNative3, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetPwrGood, GpioTermDefault, GpioLockDefault} },
	// ERROR2_N (GPIO_105)
	{SOUTH_GROUP0_ERROR2_N,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// ERROR1_N (GPIO_106)
	{SOUTH_GROUP0_ERROR1_N,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// ERROR0_N (GPIO_107)
	{SOUTH_GROUP0_ERROR0_N,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// IERR_N (CATERR_N) (GPIO_108)
	{SOUTH_GROUP0_IERR_N,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// MCERR_N  (GPIO_109)
	{SOUTH_GROUP0_MCERR_N,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SMB0_LEG_CLK (GPIO_110)
	{SOUTH_GROUP0_SMB0_LEG_CLK,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SMB0_LEG_DATA (GPIO_111)
	{SOUTH_GROUP0_SMB0_LEG_DATA,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SMB0_LEG_ALRT_N (GPIO_112)
	{SOUTH_GROUP0_SMB0_LEG_ALRT_N,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SMB1_HOST_DATA (GPIO_113)
	{SOUTH_GROUP0_SMB1_HOST_DATA,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SMB1_HOST_CLK (GPIO_114)
	{SOUTH_GROUP0_SMB1_HOST_CLK,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SMB2_PECI_DATA (GPIO_115)
	{SOUTH_GROUP0_SMB2_PECI_DATA,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SMB2_PECI_CLK (GPIO_116)
	{SOUTH_GROUP0_SMB2_PECI_CLK,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SMB4_CSME0_DATA (GPIO_117)
	{SOUTH_GROUP0_SMB4_CSME0_DATA,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SMB4_CSME0_CLK (GPIO_118)
	{SOUTH_GROUP0_SMB4_CSME0_CLK,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SMB4_CSME0_ALRT_N (GPIO_119)
	{SOUTH_GROUP0_SMB4_CSME0_ALRT_N,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// USB_OC0_N (GPIO_120)
	{SOUTH_GROUP0_USB_OC0_N,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// FLEX_CLK_SE0 (GPIO_121)
	{SOUTH_GROUP0_FLEX_CLK_SE0,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// FLEX_CLK_SE1 (GPIO_122)
	{SOUTH_GROUP0_FLEX_CLK_SE1,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// GBE3_LED1 (GPIO_4)
	{SOUTH_GROUP0_GPIO_4,
	 {GpioPadModeNative3, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SMB3_IE0_CLK (GPIO_5)
	{SOUTH_GROUP0_GPIO_5,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SMB3_IE0_DATA (GPIO_6)
	{SOUTH_GROUP0_GPIO_6,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SMB3_IE0_ALERT_N (GPIO_7)
	{SOUTH_GROUP0_GPIO_7,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SATA0_LED (GPIO_90)
	{SOUTH_GROUP0_SATA0_LED_N,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SATA1_LED (GPIO_91)
	{SOUTH_GROUP0_SATA1_LED_N,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SATA_PDETECT0 (GPIO_92)
	{SOUTH_GROUP0_SATA_PDETECT0,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SATA_PDETECT1 (GPIO_93)
	{SOUTH_GROUP0_SATA_PDETECT1,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// UART1_RTS (GPIO_94)
	{SOUTH_GROUP0_SATA0_SDOUT,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// UART1_CTS (GPIO_95)
	{SOUTH_GROUP0_SATA1_SDOUT,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// UART1_RXD (GPIO_96)
	{SOUTH_GROUP0_UART1_RXD,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// UART1_TXD (GPIO_97)
	{SOUTH_GROUP0_UART1_TXD,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SMB6_CSME1_DATA (GPIO_8)
	{SOUTH_GROUP0_GPIO_8,
	 {GpioPadModeNative3, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SMB6_CSME1_CLK (GPIO_9)
	{SOUTH_GROUP0_GPIO_9,
	 {GpioPadModeNative3, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// TCK (GPIO_141)
	{SOUTH_GROUP0_TCK,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// TRST_N (GPIO_142)
	{SOUTH_GROUP0_TRST_N,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// TMS (GPIO_143)
	{SOUTH_GROUP0_TMS,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// TDI (GPIO_144)
	{SOUTH_GROUP0_TDI,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// TDO (GPIO_145)
	{SOUTH_GROUP0_TDO,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// CX_PRDY_N (GPIO_146)
	{SOUTH_GROUP0_CX_PRDY_N,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// CX-PREQ_N (GPIO_147)
	{SOUTH_GROUP0_CX_PREQ_N,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// ME_RECVR_HDR (GPIO_148)
	{SOUTH_GROUP0_CTBTRIGINOUT,
	 {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// ADV_DBG_DFX_HDR (GPIO_149)
	{SOUTH_GROUP0_CTBTRIGOUT,
	 {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// LAD2_SPI_IRQ_N (GPIO_150)
	{SOUTH_GROUP0_DFX_SPARE2,
	 {GpioPadModeGpio, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SMB_PECI_ALRT_N (GPIO_151)
	{SOUTH_GROUP0_DFX_SPARE3,
	 {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SMB_CSME1_ALRT_N (GPIO_152)
	{SOUTH_GROUP0_DFX_SPARE4,
	 {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SUSPWRDNACK (GPIO_79)
	{SOUTH_GROUP1_SUSPWRDNACK,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// PMU_SUSCLK (GPIO_80)
	{SOUTH_GROUP1_PMU_SUSCLK,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// ADR_TRIGGER_N (GPIO_81)
	{SOUTH_GROUP1_ADR_TRIGGER,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// PMU_SLP_S45_N (GPIO_82)
	{SOUTH_GROUP1_PMU_SLP_S45_N,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// PMU_SLP_S3_N (GPIO_83)
	{SOUTH_GROUP1_PMU_SLP_S3_N,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// PMU_WAKE_N (GPIO_84)
	{SOUTH_GROUP1_PMU_WAKE_N,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// PMU_PWRBTN_N (GPIO_85)
	{SOUTH_GROUP1_PMU_PWRBTN_N,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// PMU_RESETBUTTON_N (GPIO_86)
	{SOUTH_GROUP1_PMU_RESETBUTTON_N,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// PMU_PLTRST_N (GPIO_87)
	{SOUTH_GROUP1_PMU_PLTRST_N,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// PMU_SUS_STAT_N (GPIO_88)
	{SOUTH_GROUP1_SUS_STAT_N,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// TDB_CIO_PLUG_EVENT (GPIO_89)
	{SOUTH_GROUP1_SLP_S0IX_N,
	 {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SPI_CS0_N (GPIO_72)
	{SOUTH_GROUP1_SPI_CS0_N,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SPI_CS1_N (GPIO_73)
	{SOUTH_GROUP1_SPI_CS1_N,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SPI_MOSI_IO0 (GPIO_74)
	{SOUTH_GROUP1_SPI_MOSI_IO0,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SPI_MISO_IO1 (GPIO_75)
	{SOUTH_GROUP1_SPI_MISO_IO1,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SPI_IO2 (GPIO_76)
	{SOUTH_GROUP1_SPI_IO2,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SPI_IO3 (GPIO_77)
	{SOUTH_GROUP1_SPI_IO3,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// SPI_CLK (GPIO_78)
	{SOUTH_GROUP1_SPI_CLK,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// LPC_AD0 (GPIO_64)
	{SOUTH_GROUP1_ESPI_IO0,
	 {GpioPadModeNative2, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// LPC_AD1 (GPIO_65)
	{SOUTH_GROUP1_ESPI_IO1,
	 {GpioPadModeNative2, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// LPC_AD2 (GPIO_66)
	{SOUTH_GROUP1_ESPI_IO2,
	 {GpioPadModeNative2, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// LPC_AD3 (GPIO_67)
	{SOUTH_GROUP1_ESPI_IO3,
	 {GpioPadModeNative2, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// LPC_FRAME_N (GPIO_68)
	{SOUTH_GROUP1_ESPI_CS0_N,
	 {GpioPadModeNative2, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// LPC_CLKOUT0 (GPIO_69)
	{SOUTH_GROUP1_ESPI_CLK,
	 {GpioPadModeNative2, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// LPC_CLKOUT1 (GPIO_70)
	{SOUTH_GROUP1_ESPI_RST_N,
	 {GpioPadModeNative2, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// LPC_CLKRUN_N (GPIO_71)
	{SOUTH_GROUP1_ESPI_ALRT0_N,
	 {GpioPadModeNative2, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// MFG_MODE_HDR (GPIO_10)
	{SOUTH_GROUP1_GPIO_10,
	 {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// LPC_SERIRQ (GPIO_11)
	{SOUTH_GROUP1_GPIO_11,
	 {GpioPadModeNative2, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// EMMC-CMD (GPIO_123)
	{SOUTH_GROUP1_EMMC_CMD,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermWpu20K, GpioLockDefault} },
	// EMMC-CSTROBE (GPIO_124)
	{SOUTH_GROUP1_EMMC_STROBE,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
	// EMMC-CLK (GPIO_125)
	{SOUTH_GROUP1_EMMC_CLK,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermWpd20K, GpioLockDefault} },
	// EMMC-D0 (GPIO_126)
	{SOUTH_GROUP1_EMMC_D0,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermWpu20K, GpioLockDefault} },
	// EMMC-D1 (GPIO_127)
	{SOUTH_GROUP1_EMMC_D1,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermWpu20K, GpioLockDefault} },
	// EMMC-D2 (GPIO_128)
	{SOUTH_GROUP1_EMMC_D2,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermWpu20K, GpioLockDefault} },
	// EMMC-D3 (GPIO_129)
	{SOUTH_GROUP1_EMMC_D3,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermWpu20K, GpioLockDefault} },
	// EMMC-D4 (GPIO_130)
	{SOUTH_GROUP1_EMMC_D4,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermWpu20K, GpioLockDefault} },
	// EMMC-D5 (GPIO_131)
	{SOUTH_GROUP1_EMMC_D5,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermWpu20K, GpioLockDefault} },
	// EMMC-D6 (GPIO_132)
	{SOUTH_GROUP1_EMMC_D6,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermWpu20K, GpioLockDefault} },
	// EMMC-D7 (GPIO_133)
	{SOUTH_GROUP1_EMMC_D7,
	 {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermWpu20K, GpioLockDefault} },
	// IE_ROM GPIO (GPIO_3)
	{SOUTH_GROUP1_GPIO_3,
	 {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut, GpioOutDefault,
	  GpioIntDefault, GpioResetDefault, GpioTermDefault, GpioLockDefault} },
};
#endif

#endif /* _MAINBOARD_GPIO_H */
