/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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

#ifndef _MAINBOARD_GPIO_H_
#define _MAINBOARD_GPIO_H_

#include <soc/gpio.h>

const GPIO_INIT_CONFIG mainboard_gpio_table[] = {
//Primary Well Group A
/* EC_PCH_RCIN */
{ GPIO_LP_GPP_A0, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* LPC_LAD_0 */
{ GPIO_LP_GPP_A1, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* LPC_LAD_1 */
{ GPIO_LP_GPP_A2, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioTermNone} },
/* LPC_LAD_2 */
{ GPIO_LP_GPP_A3, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* LPC_LAD_3 */
{ GPIO_LP_GPP_A4, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* LPC_FRAME */
{ GPIO_LP_GPP_A5, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* LPC_SERIRQ */
{ GPIO_LP_GPP_A6, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* XDP_PREQ */
{ GPIO_LP_GPP_A7, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* LPC_CLKRUN */
{ GPIO_LP_GPP_A8, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* EC_LPC_CLK */
{ GPIO_LP_GPP_A9, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* PCH_LPC_CLK */
{ GPIO_LP_GPP_A10, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* EC_HID_INT */
{ GPIO_LP_GPP_A11, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* ISH_KB_PROX_INT */
{ GPIO_LP_GPP_A12, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* PCH_SUSPWRACB */
{ GPIO_LP_GPP_A13, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* PM_SUS_STAT */
{ GPIO_LP_GPP_A14, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* PCH_SUSACK */
{ GPIO_LP_GPP_A15, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* SD_1P8_SEL */
{ GPIO_LP_GPP_A16, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* SD_PWR_EN */
{ GPIO_LP_GPP_A17, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* GYRO INTERRUPT */
{ GPIO_LP_GPP_A18, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* ALS INTERRUPT / SNSR_HUB_INT */
{ GPIO_LP_GPP_A19, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* ISH_GYRO_DRDY/ACCEL INTERRUPT */
{ GPIO_LP_GPP_A20, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* SNSR_HUB_STANDBY_WAKE */
{ GPIO_LP_GPP_A21, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_A22, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_A23, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
//Primary Well Group B

{ GPIO_LP_GPP_B0, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_B1, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* HSJ_MIC_DET */
{ GPIO_LP_GPP_B2, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntEdge, GpioResetDeep, GpioTermNone} },
/* TRACKPAD_INT */
{ GPIO_LP_GPP_B3, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, (GpioIntApic | GpioIntLevel), GpioResetDeep,
				GpioTermNone} },
/* BT_RF_KILL */
{ GPIO_LP_GPP_B4, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,
	GpioOutHigh, GpioIntDis, GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_B5, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* WIFI_CLK_REQ */
{ GPIO_LP_GPP_B6, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone} },
/* KEPLR_CLK_REQ/VIDEO_CLK_REQ */
{ GPIO_LP_GPP_B7, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_B8, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone} },
/* SSD_CLK_REQ/CLK_REQ_SSD */
{ GPIO_LP_GPP_B9, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_B10, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone} },
/* MPHY_EXT_PWR_GATE */
{ GPIO_LP_GPP_B11, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone} },
/* PM_SLP_S0 */
{ GPIO_LP_GPP_B12, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutHigh, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* PCH_PLT_RST */
{ GPIO_LP_GPP_B13, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutHigh, GpioIntDis, GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_B14, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone} },
/* WWAN_DISABLE */
{ GPIO_LP_GPP_B15, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,
	GpioOutLow, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* WLAN_PCIE_WAKE */
{ GPIO_LP_GPP_B16, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntEdge, GpioResetDeep, GpioTermNone} },
/* SSD_PCIE_WAKE */
{ GPIO_LP_GPP_B17, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_B18, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone} },
/* CCODEC_SPI_CS */
{ GPIO_LP_GPP_B19, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* CODEC_SPI_CLK */
{ GPIO_LP_GPP_B20, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* CODEC_SPI_MISO */
{ GPIO_LP_GPP_B21, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* CODEC_SPI_MOSI */
{ GPIO_LP_GPP_B22, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_B23, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone} },
//Primary Well Group C
/* SMB_CLK */
{ GPIO_LP_GPP_C0, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* SMB_DATA */
{ GPIO_LP_GPP_C1, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_C2, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* M2_WWAN_PWREN */
{ GPIO_LP_GPP_C3, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,
	GpioOutLow, GpioIntDis, GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_C4, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_C5, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,
	GpioOutLow, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* EC_IN_RW */
{ GPIO_LP_GPP_C6, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone} },
/* USB_CTL */
{ GPIO_LP_GPP_C7, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,
	GpioOutHigh, GpioIntDis, GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_C8, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_C9, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* NFC_RST* */
{ GPIO_LP_GPP_C10, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* EN_PP3300_KEPLER */
{ GPIO_LP_GPP_C11, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,
	GpioOutHigh, GpioIntDis, GpioResetDeep, GpioTermWpd20K} },
/* PCH_MEM_CFG0 */
{ GPIO_LP_GPP_C12, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* PCH_MEM_CFG1 */
{ GPIO_LP_GPP_C13, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* PCH_MEM_CFG2 */
{ GPIO_LP_GPP_C14, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* PCH_MEM_CFG3 */
{ GPIO_LP_GPP_C15, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* I2C0_SDA */
{ GPIO_LP_GPP_C16, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermWpu5K} },
/* I2C0_SCL */
{ GPIO_LP_GPP_C17, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermWpu5K} },
/* I2C1_SDA */
{ GPIO_LP_GPP_C18, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* I2C1_SCL */
{ GPIO_LP_GPP_C19, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* GD_UART2_RXD */
{ GPIO_LP_GPP_C20, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* GD_UART2_TXD */
{ GPIO_LP_GPP_C21, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* TCH_PNL_PWREN */
{ GPIO_LP_GPP_C22, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,
	GpioOutHigh, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* SCREW_SPI_WP_STATUS */
{ GPIO_LP_GPP_C23, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermWpu20K} },

// Primary Well Group D
/* ITCH_SPI_CS */
{ GPIO_LP_GPP_D0, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* ITCH_SPI_CLK */
{ GPIO_LP_GPP_D1, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* ITCH_SPI_MISO */
{ GPIO_LP_GPP_D2, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* CAM_FLASH_STROBE */
{ GPIO_LP_GPP_D4, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* EN_PP3300_DX_EMMC */
{ GPIO_LP_GPP_D5, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* EN_PP1800_DX_EMMC */
{ GPIO_LP_GPP_D6, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* SH_I2C1_SDA */
{ GPIO_LP_GPP_D7, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* SH_I2C1_SCL */
{ GPIO_LP_GPP_D8, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_D9, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* USB_A0_ILIM_SEL */
{ GPIO_LP_GPP_D10, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,
	GpioOutLow, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* USB_A1_ILIM_SEL */
{ GPIO_LP_GPP_D11, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,
	GpioOutLow, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* EN_PP3300_DX_CAM */
{ GPIO_LP_GPP_D12, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* EN_PP1800_DX_AUDIO */
{ GPIO_LP_GPP_D13, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* WWAN_WAKE* */
{ GPIO_LP_GPP_D14, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntEdge, GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_D15, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_D16, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* DMIC_CLK_1 */
{ GPIO_LP_GPP_D17, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* DMIC_DATA_1 */
{ GPIO_LP_GPP_D18, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* DMIC_CLK_0 */
{ GPIO_LP_GPP_D19, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* DMIC_DATA_0 */
{ GPIO_LP_GPP_D20, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* ITCH_SPI_D2 */
{ GPIO_LP_GPP_D21, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* ITCH_SPI_D3 */
{ GPIO_LP_GPP_D22, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* I2S_MCLK */
{ GPIO_LP_GPP_D23, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
//Primary Well Group E
/* SPI_TPM_IRQ */
{ GPIO_LP_GPP_E0, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntEdge, GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_E1, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* SSD_PEDET */
{ GPIO_LP_GPP_E2, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_E3, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_E4, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_E5, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* SSD_SATA_DEVSLP */
{ GPIO_LP_GPP_E6, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutHigh, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* TCH_PNL_INTR* */
{ GPIO_LP_GPP_E7, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, (GpioIntApic | GpioIntLevel),
	GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_E8, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* USB2_OC_0 */
{ GPIO_LP_GPP_E9, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	 GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone} },
/* USB2_OC_1 */
{ GPIO_LP_GPP_E10, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone} },
/* USB2_OC_2 */
{ GPIO_LP_GPP_E11, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone} },
/* USB2_OC_3 */
{ GPIO_LP_GPP_E12, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone} },
/* DDI1_HPD */
{ GPIO_LP_GPP_E13, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone} },
/* DDI2_HPD */
{ GPIO_LP_GPP_E14, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone} },
/* EC_SMI */
{ GPIO_LP_GPP_E15, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntEdge, GpioResetDeep, GpioTermNone} },
/* EC_SCI */
{ GPIO_LP_GPP_E16, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntEdge, GpioResetDeep, GpioTermNone} },
/* EDP_HPD */
{ GPIO_LP_GPP_E17, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_E18, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_E19, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_E20, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_E21, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_E22, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* TCH_PNL_RST */
{ GPIO_LP_GPP_E23, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut, GpioOutHigh,
	GpioIntDis, GpioResetDeep, GpioTermNone} },
//Primary Well Group F
/* I2S2_SCLK */
{ GPIO_LP_GPP_F0, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* I2S2_SFRM */
{ GPIO_LP_GPP_F1, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* I2S2_TXD */
{ GPIO_LP_GPP_F2, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* I2S2_RXD */
{ GPIO_LP_GPP_F3, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_F4, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_F5, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_F6, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
{ GPIO_LP_GPP_F7, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* I2C4_SDA */
{ GPIO_LP_GPP_F8, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* I2C4_SDA */
{ GPIO_LP_GPP_F9, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* AUDIO_IRQ */
{ GPIO_LP_GPP_F10, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntApic | GpioIntEdge, GpioResetDeep, GpioTermNone} },
/* AUDIO_SW_INT* */
{ GPIO_LP_GPP_F11, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirInInv,
	GpioOutDefault, GpioIntApic | GpioIntEdge, GpioResetDeep, GpioTermNone} },
/* EMMC_CMD */
{ GPIO_LP_GPP_F12, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* EMMC_D_0 */
{ GPIO_LP_GPP_F13, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* EMMC_D_1 */
{ GPIO_LP_GPP_F14, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* EMMC_D_2 */
{ GPIO_LP_GPP_F15, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* EMMC_D_3 */
{ GPIO_LP_GPP_F16, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* EMMC_D_4 */
{ GPIO_LP_GPP_F17, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* EMMC_D_5 */
{ GPIO_LP_GPP_F18, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* EMMC_D_6 */
{ GPIO_LP_GPP_F19, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* EMMC_D_7 */
{ GPIO_LP_GPP_F20, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* EMMC_RCLK */
{ GPIO_LP_GPP_F21, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* EMMC_CLK */
{ GPIO_LP_GPP_F22, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* USIM_DET */
{ GPIO_LP_GPP_F23, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
//Primary Well Group G
/* SD_CMD */
{ GPIO_LP_GPP_G0, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* SD_D_0 */
{ GPIO_LP_GPP_G1, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* SD_D_1 */
{ GPIO_LP_GPP_G2, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* SD_D_2 */
{ GPIO_LP_GPP_G3, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* SD_D_3 */
{ GPIO_LP_GPP_G4, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* SD_CD */
{ GPIO_LP_GPP_G5, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* SD_CLK */
{ GPIO_LP_GPP_G6, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntEdge, GpioResetDeep, GpioTermNone} },
/* SD_WP */
{ GPIO_LP_GPP_G7, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },

//Deep Sleep Well Group
/* PCH_BATLOW */
{ GPIO_LP_GPD0, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* EC_PCH_ACPRESENT */
{ GPIO_LP_GPD1, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone} },
/* EC_PCH_WAKE */
{ GPIO_LP_GPD2, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntEdge, GpioResetDeep, GpioTermNone} },
/* EC_PCH_PWRBTN */
{ GPIO_LP_GPD3, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,
	GpioOutDefault, GpioIntLevel, GpioResetDeep, GpioTermNone} },
/* PM_SLP_S3 */
{ GPIO_LP_GPD4, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutHigh, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* PM_SLP_S4 */
{ GPIO_LP_GPD5, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutHigh, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* PCH_SLP_SA */
{ GPIO_LP_GPD6, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* USB_WAKEOUT_INTRUDET */
{ GPIO_LP_GPD7, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* PM_SUSCLK */
{ GPIO_LP_GPD8, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut, GpioOutLow,
	GpioIntDis, GpioResetDeep, GpioTermNone} },
/* PCH_SLP_WLAN */
{ GPIO_LP_GPD9, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* PCH_SLP_S5 */
{ GPIO_LP_GPD10, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
/* PM_LANPHY_ENABLE */
{ GPIO_LP_GPD11, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,
	GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTermNone} },
{ END_OF_GPIO_TABLE, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,
	GpioOutDefault, GpioIntDis, GpioResetPwrGood,  GpioTermNone} },
};
#endif
