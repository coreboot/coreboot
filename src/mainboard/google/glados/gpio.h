/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Inc.
 * Copyright (C) 2015 Intel Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#ifndef MAINBOARD_GPIO_H
#define MAINBOARD_GPIO_H

#include <soc/gpio.h>

const GPIO_INIT_CONFIG mainboard_gpio_table[] = {
/* RCIN# */
{GPIO_LP_GPP_A0, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* LAD0 */
{GPIO_LP_GPP_A1, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* LAD1 */
{GPIO_LP_GPP_A2, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioTermNone}},
/* LAD2 */
{GPIO_LP_GPP_A3, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* LAD3 */
{GPIO_LP_GPP_A4, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* LFRAME# */
{GPIO_LP_GPP_A5, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* SERIRQ */
{GPIO_LP_GPP_A6, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* CLKRUN# */
{GPIO_LP_GPP_A8, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* CLKOUT_LPC0 */
{GPIO_LP_GPP_A9, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* SUSWARN# tied to SUSACK# */
{GPIO_LP_GPP_A13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* SUS_STAT# TP27 */
{GPIO_LP_GPP_A13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* SUSACK# tied to SUSWARN# */
{GPIO_LP_GPP_A15, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* SD_1P8_SEL */
{GPIO_LP_GPP_A16, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* SD_PWR_EN# */
{GPIO_LP_GPP_A17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* TRACKPAD_INT_L */
{GPIO_LP_GPP_B3, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, (GpioIntApic | GpioIntLevel), GpioResetDeep,
	GpioTermNone}},
/* SRCCLKREQ1# / WLAN_PCIE_CLKREQ_L */
{GPIO_LP_GPP_B6, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone}},
/* SRCCLKREQ2# / KEPLER_PCIE_CLKREQ_L */
{GPIO_LP_GPP_B7, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone}},
/* EXT_PWR_GATE# */
{GPIO_LP_GPP_B11, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone}},
/* SLP_S0# */
{GPIO_LP_GPP_B12, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutHigh, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* PLTRST# */
{GPIO_LP_GPP_B13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutHigh, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* WLAN_PCIE_WAKE_L */
{GPIO_LP_GPP_B16, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntEdge, GpioResetDeep, GpioTermNone}},
/* SMBCLK (XDP) */
{GPIO_LP_GPP_C0, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* SMBDATA (XDP) */
{GPIO_LP_GPP_C1, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* EC_IN_RW */
{GPIO_LP_GPP_C6, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone}},
/* EN_PP3300_KEPLER */
{GPIO_LP_GPP_C11, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,
	GpioOutHigh, GpioIntDis, GpioResetDeep, GpioTermWpd20K}},
/* PCH_MEM_CONFIG[0] */
{GPIO_LP_GPP_C12, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* PCH_MEM_CONFIG[1] */
{GPIO_LP_GPP_C13, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* PCH_MEM_CONFIG[2] */
{GPIO_LP_GPP_C14, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* PCH_MEM_CONFIG[3] */
{GPIO_LP_GPP_C15, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* I2C0_SDA */
{GPIO_LP_GPP_C16, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* I2C0_SCL */
{GPIO_LP_GPP_C17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* I2C1 SDA */
{GPIO_LP_GPP_C18, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* I2C1 SDA */
{GPIO_LP_GPP_C19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* UART2_RXD */
{GPIO_LP_GPP_C20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* UART2_TXD */
{GPIO_LP_GPP_C21, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* EN_PP3300_DX_TOUCHSCREEN */
{GPIO_LP_GPP_C22, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,
	GpioOutHigh, GpioIntDis, GpioResetDeep, GpioTermWpd20K}},
/* PCH_WP */
{GPIO_LP_GPP_C23, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermWpu20K}},
/* EN_PP3300_DX_EMMC */
{GPIO_LP_GPP_D5, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,
	GpioOutHigh, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* EN_PP1800_DX_EMMC */
{GPIO_LP_GPP_D6, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,
	GpioOutHigh, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* USBA_1_ILIM_SEL_L */
{GPIO_LP_GPP_D10, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,
	GpioOutHigh, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* USBA_2_ILIM_SEL_L */
{GPIO_LP_GPP_D11, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,
	GpioOutHigh, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* EN_PP3300_DX_CAM */
{GPIO_LP_GPP_D12, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,
	GpioOutHigh, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* DMIC_CLK1 */
{GPIO_LP_GPP_D17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* DMIC_DATA1 */
{GPIO_LP_GPP_D18, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* DMIC_CLK0 */
{GPIO_LP_GPP_D19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* DMIC_DATA0 */
{GPIO_LP_GPP_D20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* I2S_MCLK */
{GPIO_LP_GPP_D23, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* TPM_PIRQ_L */
{GPIO_LP_GPP_E0, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntEdge, GpioResetDeep, GpioTermNone}},
/* TOUCHSCREEN_INT_L */
{GPIO_LP_GPP_E7, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, (GpioIntApic | GpioIntLevel), GpioResetDeep,
	GpioTermNone}},
/* USB2_OC0# */
{GPIO_LP_GPP_E9, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone}},
/* USB2_OC1# */
{GPIO_LP_GPP_E10, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone}},
/* USB2_OC2# */
{GPIO_LP_GPP_E11, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone}},
/* USB2_OC3# */
{GPIO_LP_GPP_E12, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone}},
/* DDPB_HPD0 */
{GPIO_LP_GPP_E13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone}},
/* DDPC_HPD1 */
{GPIO_LP_GPP_E14, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone}},
/* EC_SMI_L */
{GPIO_LP_GPP_E15, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntEdge, GpioResetDeep, GpioTermNone}},
/* EC_SCI_L */
{GPIO_LP_GPP_E16, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntEdge, GpioResetDeep, GpioTermNone}},
/* EDP_HPD */
{GPIO_LP_GPP_E17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone}},
/*
 * The next 4 pads are for bit banging the amplifiers. They are connected
 * together with i2s0 signals. For default behavior of i2s make these
 * gpio inupts.
 */
/* I2S2_SCLK */
{GPIO_LP_GPP_F0, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* I2S2_SFRM */
{GPIO_LP_GPP_F1, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* I2S2_TXD */
{GPIO_LP_GPP_F2, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* I2S2_RXD */
{GPIO_LP_GPP_F3, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* I2C4_SDA */
{GPIO_LP_GPP_F8, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* I2C4_SCL */
{GPIO_LP_GPP_F9, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* MIC_INT_L */
{GPIO_LP_GPP_F10, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirInInv,
	GpioOutDefault, (GpioIntApic | GpioIntEdge), GpioResetDeep,
	GpioTermNone}},
/* EMMC_CMD */
{GPIO_LP_GPP_F12, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* EMMC_DATA0 */
{GPIO_LP_GPP_F13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* EMMC_DATA1 */
{GPIO_LP_GPP_F14, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* EMMC_DATA2 */
{GPIO_LP_GPP_F15, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* EMMC_DATA3 */
{GPIO_LP_GPP_F16, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* EMMC_DATA4 */
{GPIO_LP_GPP_F17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* EMMC_DATA5 */
{GPIO_LP_GPP_F18, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* EMMC_DATA6 */
{GPIO_LP_GPP_F19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* EMMC_DATA7 */
{GPIO_LP_GPP_F20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* EMMC_RCLK */
{GPIO_LP_GPP_F21, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* EMMC_CLK */
{GPIO_LP_GPP_F22, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* EMMC_CMD */
{GPIO_LP_GPP_F23, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* SD_CMD */
{GPIO_LP_GPP_G0, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* SD_DATA0 */
{GPIO_LP_GPP_G1, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* SD_DATA1 */
{GPIO_LP_GPP_G2, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* SD_DATA2 */
{GPIO_LP_GPP_G3, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* SD_DATA3 */
{GPIO_LP_GPP_G4, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* SD_CD# */
{GPIO_LP_GPP_G5, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* SD_CLK# */
{GPIO_LP_GPP_G6, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntEdge, GpioResetDeep, GpioTermNone}},
/* SD_WP# */
{GPIO_LP_GPP_G7, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntEdge, GpioResetDeep, GpioTermNone}},
/* ACPRESENT# */
{GPIO_LP_GPD1, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone}},
/* EC_PCH_WAKE_L */
{GPIO_LP_GPD2, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntEdge, GpioResetDeep, GpioTermNone}},
/* PWRBTN# */
{GPIO_LP_GPD3, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone}},
/* SLP_S3# */
{GPIO_LP_GPD4, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutHigh, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* SLP_S4# */
{GPIO_LP_GPD5, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutHigh, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* SUSCLK */
{GPIO_LP_GPD8, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutLow, GpioIntDis, GpioResetDeep, GpioTermNone}},
/* SLP_S5# */
{GPIO_LP_GPD10, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutLow, GpioIntDis, GpioResetDeep, GpioTermNone}},
{END_OF_GPIO_TABLE,  {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,
	GpioOutDefault, GpioIntDis, GpioResetPwrGood, GpioTermNone}},
};

#endif
