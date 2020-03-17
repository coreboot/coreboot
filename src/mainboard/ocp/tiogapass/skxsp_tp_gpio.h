/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SKXSP_TP_GPIO_H_
#define _SKXSP_TP_GPIO_H_

#include <gpio_fsp.h>
#include <soc/gpio_soc_defs.h>

/*
 *  OCP TiogaPass Gpio Pad Configuration
 */
static const UPD_GPIO_INIT_CONFIG tp_gpio_table[] = {
	{GPIO_SKL_H_GPP_A0, {
		GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_A_0_LPC_RCIN_N_ESPI_ALERT1_N
	{GPIO_SKL_H_GPP_A1, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_A_1_LAD_0_ESPI_IO_0
	{GPIO_SKL_H_GPP_A2, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_A_2_LAD_1_ESPI_IO_1
	{GPIO_SKL_H_GPP_A3, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_A_3_LAD_2_ESPI_IO_2
	{GPIO_SKL_H_GPP_A4, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_A_4_LAD_3_ESPI_IO_3
	{GPIO_SKL_H_GPP_A5, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_A_5_LPC_LFRAME_N_ESPI_CS0_N
	{GPIO_SKL_H_GPP_A6, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_A_6_IRQ_LPC_SERIRQ_ESPI_CS1_N
	{GPIO_SKL_H_GPP_A7, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_A_7_IRQ_LPC_PIRQA_N_ESPI_ALERT0_N
	{GPIO_SKL_H_GPP_A8, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_A_8_FM_LPC_CLKRUN_N
	{GPIO_SKL_H_GPP_A9, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_A_9_CLKOUT_LPC0_ESPI_CLK
	{GPIO_SKL_H_GPP_A10, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_A_10_CLKOUT_LPC1
	{GPIO_SKL_H_GPP_A11, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_A_11_FM_LPC_PME_N
	{GPIO_SKL_H_GPP_A12, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetNormal, GpioTermNone, GpioPadConfigLock
		} }, //GPP_A_12_BMBUSY_N_SXEXITHLDOFF_N
	{GPIO_SKL_H_GPP_A13, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_A_13_SUSWARN_N_SUSPWRDNACK
	{GPIO_SKL_H_GPP_A14, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_A_14_ESPI_RESET_N
	{GPIO_SKL_H_GPP_A15, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_A_15_SUSACK_N
	{GPIO_SKL_H_GPP_A16, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_A_16_CLKOUT_LPC2
	{GPIO_SKL_H_GPP_A17, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_A_17
	{GPIO_SKL_H_GPP_A18, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_A_18
	//		{GPIO_SKL_H_GPP_A19, {} }, //GPP_A_19, controlled by ME
	{GPIO_SKL_H_GPP_A20, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} },//GPP_A_20
	{GPIO_SKL_H_GPP_A21, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutLow,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_A_21
	{GPIO_SKL_H_GPP_A22, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_A_22
	{GPIO_SKL_H_GPP_A23, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_A_23

	{GPIO_SKL_H_GPP_B0,  {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_B_0_CORE_VID_0
	{GPIO_SKL_H_GPP_B1,  {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_B_1_CORE_VID_1
	{GPIO_SKL_H_GPP_B2,  {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_B_2_VRALERT_N
	{GPIO_SKL_H_GPP_B3,  {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_B_3_CPU_GP2
	{GPIO_SKL_H_GPP_B4,  {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_B_4_CPU_GP3
	{GPIO_SKL_H_GPP_B5,  {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_B_5_SRCCLKREQ0_N
	{GPIO_SKL_H_GPP_B6,  {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetNormal, GpioTermNone, GpioPadConfigLock
		} }, //GPP_B_6_SRCCLKREQ1_N
	{GPIO_SKL_H_GPP_B7,  {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetNormal, GpioTermNone, GpioPadConfigLock
		} }, //GPP_B_7_SRCCLKREQ2_N
	{GPIO_SKL_H_GPP_B8,  {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_B_8_SRCCLKREQ3_N
	{GPIO_SKL_H_GPP_B9,  {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_B_9_SRCCLKREQ4_N
	{GPIO_SKL_H_GPP_B10, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_B_10_SRCCLKREQ5_N
	{GPIO_SKL_H_GPP_B11, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutLow,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_B_11
	{GPIO_SKL_H_GPP_B12, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_B_12_GLB_RST_WARN_N
	{GPIO_SKL_H_GPP_B13, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_B_13_RST_PLTRST_N
	{GPIO_SKL_H_GPP_B14, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_B_14_FM_PCH_BIOS_RCVR_SPKR
	{GPIO_SKL_H_GPP_B15, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_B_15
	{GPIO_SKL_H_GPP_B16, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_B_16
	{GPIO_SKL_H_GPP_B17, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_B_17
	{GPIO_SKL_H_GPP_B18, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_B_18
	{GPIO_SKL_H_GPP_B19, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutLow,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_B_19
	{GPIO_SKL_H_GPP_B20, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_B_20
	{GPIO_SKL_H_GPP_B21, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_B_21
	{GPIO_SKL_H_GPP_B22, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_B_22
	{GPIO_SKL_H_GPP_B23, {
		GpioPadModeNative2, GpioHostOwnGpio, GpioDirOut,   GpioOutLow,
		GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock
		} }, //GPP_B_23_MEIE_SML1ALRT_N_PHOT_N

//		{GPIO_SKL_H_GPP_C0,  {} }, //GPP_C_0_SMBCLK, controlled by ME
//		{GPIO_SKL_H_GPP_C1,  {} }, //GPP_C_1_SMBDATA, controlled by ME
	{GPIO_SKL_H_GPP_C2,  {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_C_2_SMBALERT_N
//		{GPIO_SKL_H_GPP_C3,  {} }, //GPP_C_3_SML0CLK_IE, controlled by ME
//		{GPIO_SKL_H_GPP_C4,  {} }, //GPP_C_4_SML0DATA_IE, controlled by ME
	{GPIO_SKL_H_GPP_C5,  {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
		GpioIntDis, GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_C_5_SML0ALERT_IE_N
//		{GPIO_SKL_H_GPP_C6,  {} }, //GPP_C_6_SML1CLK_IE, controlled by ME
//		{GPIO_SKL_H_GPP_C7,  {} }, //GPP_C_7_SML1DATA_IE, controlled by ME
	{GPIO_SKL_H_GPP_C8,  {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_C_8
	{GPIO_SKL_H_GPP_C9,  {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut, GpioOutLow,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_C_9
	{GPIO_SKL_H_GPP_C10, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutLow,
		GpioIntDis, GpioResetNormal, GpioTermNone, GpioPadConfigLock
		} }, //GPP_C_10
	{GPIO_SKL_H_GPP_C11, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,	GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_C_11
	{GPIO_SKL_H_GPP_C12, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutLow,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_C_12
	{GPIO_SKL_H_GPP_C13, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutLow,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_C_13
	{GPIO_SKL_H_GPP_C14, {
		GpioPadModeGpio, GpioHostOwnAcpi, GpioDirIn, GpioOutDefault,
		GpioIntLevel | GpioIntSci, GpioResetNormal, GpioTermNone, GpioPadConfigLock
		} }, //GPP_C_14
	{GPIO_SKL_H_GPP_C15, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_C_15
	{GPIO_SKL_H_GPP_C16, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_C_16
	{GPIO_SKL_H_GPP_C17, {
		GpioPadModeGpio,		GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_C_17
	{GPIO_SKL_H_GPP_C18, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_C_18
	{GPIO_SKL_H_GPP_C19, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_C_19
//		{GPIO_SKL_H_GPP_C20, {} }, //GPP_C_20, controlled by ME
	{GPIO_SKL_H_GPP_C21, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_C_21
	{GPIO_SKL_H_GPP_C22, {
		GpioPadModeGpio, GpioHostOwnAcpi, GpioDirIn, GpioOutDefault,
		GpioIntLevel | GpioIntSmi, GpioResetNormal, GpioTermNone, GpioPadConfigLock
		} }, //GPP_C_22
	{GPIO_SKL_H_GPP_C23, {
		GpioPadModeGpio, GpioHostOwnAcpi, GpioDirIn, GpioOutDefault,
		GpioIntLevel | GpioIntSmi, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_C_23

	{GPIO_SKL_H_GPP_D0, {
		GpioPadModeGpio, GpioHostOwnAcpi, GpioDirIn,		GpioOutDefault,
		GpioIntLevel | GpioIntSmi, GpioResetNormal, GpioTermNone, GpioPadConfigLock
		} }, //GPP_D_0
	{GPIO_SKL_H_GPP_D1, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_D_1
	{GPIO_SKL_H_GPP_D2, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetNormal, GpioTermNone, GpioPadConfigLock
		} }, //GPP_D_2
	{GPIO_SKL_H_GPP_D3, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetNormal, GpioTermNone, GpioPadConfigLock
		} }, //GPP_D_3
	{GPIO_SKL_H_GPP_D4, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutLow,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_D_4
	{GPIO_SKL_H_GPP_D5, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_D_5
	{GPIO_SKL_H_GPP_D6, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_D_6
	{GPIO_SKL_H_GPP_D7, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_D_7
	{GPIO_SKL_H_GPP_D8, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_D_8
	{GPIO_SKL_H_GPP_D9, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_D_9_SSATA_DEVSLP3
	{GPIO_SKL_H_GPP_D10, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_D_10_SSATA_DEVSLP4
	{GPIO_SKL_H_GPP_D11, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_D_11_SSATA_DEVSLP5
	{GPIO_SKL_H_GPP_D12, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_D_12_SSATA_SDATAOUT1
	{GPIO_SKL_H_GPP_D13, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_D_13_SML0BLCK_IE
	{GPIO_SKL_H_GPP_D14, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_D_14_SML0BDATA_IE
	{GPIO_SKL_H_GPP_D15, {
		GpioPadModeNative3, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_D_15_SSATA_SDATAOUT0
	{GPIO_SKL_H_GPP_D16, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_D_16_SML0BALERT_IE_N
	{GPIO_SKL_H_GPP_D17, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_D_17
	{GPIO_SKL_H_GPP_D18, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_D_18
	{GPIO_SKL_H_GPP_D19, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutLow,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock | GpioOutputStateLock
		} }, //GPP_D_19
	{GPIO_SKL_H_GPP_D20, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_D_20_TP_PCH_GPP_D_20
	{GPIO_SKL_H_GPP_D21, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_D_21_IE_URAT_RX
	{GPIO_SKL_H_GPP_D22, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_D_22_IE_URAT_TX
	{GPIO_SKL_H_GPP_D23, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_D_23

	{GPIO_SKL_H_GPP_E0, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntLevel | GpioIntSmi, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_E_0_SATAXPCIE0_SATAGP0
	{GPIO_SKL_H_GPP_E1, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntLevel | GpioIntSmi, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_E_1_SATAXPCIE1_SATAGP1
	{GPIO_SKL_H_GPP_E2, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_E_2_SATAXPCIE2_SATAGP2
	{GPIO_SKL_H_GPP_E3, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_E_3_CPU_GP0
	{GPIO_SKL_H_GPP_E4, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_E_4_SATA_DEVSLP0
	{GPIO_SKL_H_GPP_E5, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_E_5_SATA_DEVSLP1
	{GPIO_SKL_H_GPP_E6, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_E_6_SATA_DEVSLP2
	{GPIO_SKL_H_GPP_E7, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_E_7_CPU_GP1
	{GPIO_SKL_H_GPP_E8, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_E_8_SATA_LED_N
	{GPIO_SKL_H_GPP_E9, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_E_9_USB2_OC0_N
	{GPIO_SKL_H_GPP_E10, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_E_10_USB2_OC1_N
	{GPIO_SKL_H_GPP_E11, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_E_11_USB2_OC2_N
	{GPIO_SKL_H_GPP_E12, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_E_12_USB2_OC3_N

	{GPIO_SKL_H_GPP_F0, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_F_0_SATAXPCIE3_SATAGP3
	{GPIO_SKL_H_GPP_F1, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_F_1_SATAXPCIE4_SATAGP4
	{GPIO_SKL_H_GPP_F2, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_F_2_SATAXPCIE5_SATAGP5
	{GPIO_SKL_H_GPP_F3, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_F_3_SATAXPCIE6_SATAGP6
	{GPIO_SKL_H_GPP_F4, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_F_4_SATAXPCIE7_SATAGP7
	{GPIO_SKL_H_GPP_F5, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutLow,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_F_5_SATA_DEVSLP3
	{GPIO_SKL_H_GPP_F6, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetNormal, GpioTermNone, GpioPadConfigLock
		} }, //GPP_F_6_SATA_DEVSLP4
	{GPIO_SKL_H_GPP_F7, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetNormal, GpioTermNone, GpioPadConfigLock
		} }, //GPP_F_7_SATA_DEVSLP5
	{GPIO_SKL_H_GPP_F8, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetNormal, GpioTermNone, GpioPadConfigLock
		} }, //GPP_F_8_SATA_DEVSLP6
	{GPIO_SKL_H_GPP_F9, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetNormal, GpioTermNone, GpioPadConfigLock
		} }, //GPP_F_9_SATA_DEVSLP7
	{GPIO_SKL_H_GPP_F10, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_F_10_SATA_SCLOCK
	{GPIO_SKL_H_GPP_F11, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_F_11_SATA_SLOAD
	{GPIO_SKL_H_GPP_F12, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_F_12_SATA_SDATAOUT1
	{GPIO_SKL_H_GPP_F13, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_F_13_SATA_SDATAOUT0
	{GPIO_SKL_H_GPP_F14, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_F_14_SSATA_LED_N
	{GPIO_SKL_H_GPP_F15, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_F_15_USB2_OC4_N
	{GPIO_SKL_H_GPP_F16, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_F_16_USB2_OC5_N
	{GPIO_SKL_H_GPP_F17, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_F_17_USB2_OC6_N
	{GPIO_SKL_H_GPP_F18, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_F_18_USB2_OC7_N
	{GPIO_SKL_H_GPP_F19, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_F_19_LAN_SMBCLK
	{GPIO_SKL_H_GPP_F20, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_F_20_LAN_SMBDATA
	{GPIO_SKL_H_GPP_F21, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_F_21_LAN_SMBALERT_N
	{GPIO_SKL_H_GPP_F22, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_F_22_SSATA_SCLOCK
	{GPIO_SKL_H_GPP_F23, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_F_23_SSATA_SLOAD

	{GPIO_SKL_H_GPP_G0, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_G_0_FANTACH0_FANTACH0IE
	{GPIO_SKL_H_GPP_G1, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_G_1_FANTACH1_FANTACH1IE
	{GPIO_SKL_H_GPP_G2, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_G_2_FANTACH2_FANTACH2IE
	{GPIO_SKL_H_GPP_G3, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_G_3_FANTACH3_FANTACH3IE
	{GPIO_SKL_H_GPP_G4, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_G_4_FANTACH4_FANTACH4IE
	{GPIO_SKL_H_GPP_G5, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_G_5_FANTACH5_FANTACH5IE
	{GPIO_SKL_H_GPP_G6, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_G_6_FANTACH6_FANTACH6IE
	{GPIO_SKL_H_GPP_G7, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_G_7_FANTACH7_FANTACH7IE
	{GPIO_SKL_H_GPP_G8, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_G_8_FANPWM0_FANPWM0IE
	{GPIO_SKL_H_GPP_G9, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_G_9_FANPWM1_FANPWM1IE
	{GPIO_SKL_H_GPP_G10, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_G_10_FANPWM2_FANPWM2IE
	{GPIO_SKL_H_GPP_G11, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_G_11_FANPWM3_FANPWM3IE
	{GPIO_SKL_H_GPP_G12, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutLow,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_G_12
	{GPIO_SKL_H_GPP_G13, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutLow,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_G_13
	{GPIO_SKL_H_GPP_G14, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutLow,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_G_14
	{GPIO_SKL_H_GPP_G15, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutLow,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_G_15
	{GPIO_SKL_H_GPP_G16, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutLow,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_G_16
	{GPIO_SKL_H_GPP_G17, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_G_17_ADR_COMPLETE
	{GPIO_SKL_H_GPP_G18, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_G_18_FM_NMI_EVENT_N
	{GPIO_SKL_H_GPP_G19, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_G_19_FM_SMI_ACTIVE_N
//		{GPIO_SKL_H_GPP_G20, {} }, //GPP_G_20_SSATA_DEVSLP0, controlled by ME
	{GPIO_SKL_H_GPP_G21, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_G_21_SSATA_DEVSLP1
	{GPIO_SKL_H_GPP_G22, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_G_22_SSATA_DEVSLP2
	{GPIO_SKL_H_GPP_G23, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_G_23_SSATAXPCIE0_SSATAGP0

	{GPIO_SKL_H_GPP_H0, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_H_0_SRCCLKREQ6_N
	{GPIO_SKL_H_GPP_H1, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_H_1_SRCCLKREQ7_N
	{GPIO_SKL_H_GPP_H2, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_H_2_SRCCLKREQ8_N
	{GPIO_SKL_H_GPP_H3, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_H_3_SRCCLKREQ9_N
	{GPIO_SKL_H_GPP_H4, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_H_4_SRCCLKREQ10_N
//		{GPIO_SKL_H_GPP_H5,  {} }, //GPP_H_5_SRCCLKREQ11_N
	{GPIO_SKL_H_GPP_H6, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_H_6_SRCCLKREQ12_N
	{GPIO_SKL_H_GPP_H7, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_H_7_SRCCLKREQ13_N
	{GPIO_SKL_H_GPP_H8, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_H_8_SRCCLKREQ14_N
	{GPIO_SKL_H_GPP_H9, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_H_9_SRCCLKREQ15_N
//		{GPIO_SKL_H_GPP_H10, {} }, //GPP_H_10_SML2CLK_IE
//		{GPIO_SKL_H_GPP_H11, {} }, //GPP_H_11_SML2DATA_IE
	{GPIO_SKL_H_GPP_H12, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_H_12_SML2ALERT_N_IE
//		{GPIO_SKL_H_GPP_H13, {} }, //GPP_H_13_SML3CLK_IE
//		{GPIO_SKL_H_GPP_H14, {} }, //GPP_H_14_SML3DATA_IE
	{GPIO_SKL_H_GPP_H15, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetNormal, GpioTermNone, GpioPadConfigLock
		} }, //GPP_H_15_SML3ALERT_N_IE
//		{GPIO_SKL_H_GPP_H16, {} }, //GPP_H_16_SML4CLK_IE
//		{GPIO_SKL_H_GPP_H17, {} }, //GPP_H_17_SML4DATA_IE
	{GPIO_SKL_H_GPP_H18, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_H_18_SML4ALERT_N_IE
	{GPIO_SKL_H_GPP_H19, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetNormal, GpioTermNone, GpioPadConfigLock
		} }, //GPP_H_19_SSATAXPCIE1_SSATAGP1
	{GPIO_SKL_H_GPP_H20, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_H_20_SSATAXPCIE2_SSATAGP2
	{GPIO_SKL_H_GPP_H21, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_H_21_SSATAXPCIE3_SSATAGP3
	{GPIO_SKL_H_GPP_H22, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_H_22_SSATAXPCIE4_SSATAGP4
	{GPIO_SKL_H_GPP_H23, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep, GpioTermNone, GpioPadConfigLock
		} }, //GPP_H_23_SSATAXPCIE5_SSATAGP5

	{GPIO_SKL_H_GPP_I0, {
		GpioPadModeNative2, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_I_0_GBE_TDO
	{GPIO_SKL_H_GPP_I1, {
		GpioPadModeNative2, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_I_1_GBE_TCK
	{GPIO_SKL_H_GPP_I2, {
		GpioPadModeNative2, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_I_2_GBE_TMS
	{GPIO_SKL_H_GPP_I3, {
		GpioPadModeNative2, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_I_3_GBE_TDI
	{GPIO_SKL_H_GPP_I4, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_I_4_DO_RESET_IN_N
	{GPIO_SKL_H_GPP_I5, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_I_5_DO_RESET_OUT_N
	{GPIO_SKL_H_GPP_I6, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_I_6_RESET_DONE
	{GPIO_SKL_H_GPP_I7, {
		GpioPadModeNative2, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_I_7_JTAG_GBE_TRST_N
	{GPIO_SKL_H_GPP_I8, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_I_8_GBE_PCI_DIS
	{GPIO_SKL_H_GPP_I9, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_I_9_GBE_LAN_DIS
	{GPIO_SKL_H_GPP_I10, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_I_10

	{GPIO_SKL_H_GPP_J0, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_J_0_LAN_LED_P0_0
	{GPIO_SKL_H_GPP_J1, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_J_1_LAN_LED_P0_1
	{GPIO_SKL_H_GPP_J2, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_J_2_LAN_LED_P1_0
	{GPIO_SKL_H_GPP_J3, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_J_3_LAN_LED_P1_1
	{GPIO_SKL_H_GPP_J4, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_J_4_LAN_LED_P2_0
	{GPIO_SKL_H_GPP_J5, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_J_5_LAN_LED_P2_1
	{GPIO_SKL_H_GPP_J6, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_J_6_LAN_LED_P3_0
	{GPIO_SKL_H_GPP_J7, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_J_7_LAN_LED_P3_1
	{GPIO_SKL_H_GPP_J8, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_J_8_LAN_I2C_SCL_MDC_P0
	{GPIO_SKL_H_GPP_J9, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_J_9_LAN_I2C_SDA_MDIO_P0
	{GPIO_SKL_H_GPP_J10, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_J_10_LAN_I2C_SCL_MDC_P1
	{GPIO_SKL_H_GPP_J11, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_J_11_LAN_I2C_SDA_MDIO_P1
	{GPIO_SKL_H_GPP_J12, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_J_12_LAN_I2C_SCL_MDC_P2
	{GPIO_SKL_H_GPP_J13, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_J_13_LAN_I2C_SDA_MDIO_P2
	{GPIO_SKL_H_GPP_J14, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_J_14_LAN_I2C_SCL_MDC_P3
	{GPIO_SKL_H_GPP_J15, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_J_15_LAN_I2C_SDA_MDIO_P3
	{GPIO_SKL_H_GPP_J16, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_J_16_LAN_SDP_P0_0
	{GPIO_SKL_H_GPP_J17, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_J_17_LAN_SDP_P0_1
	{GPIO_SKL_H_GPP_J18, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_J_18_LAN_SDP_P1_0
	{GPIO_SKL_H_GPP_J19, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_J_19_LAN_SDP_P1_1
	{GPIO_SKL_H_GPP_J20, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_J_20_LAN_SDP_P2_0
	{GPIO_SKL_H_GPP_J21, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_J_21_LAN_SDP_P2_1
	{GPIO_SKL_H_GPP_J22, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_J_22_LAN_SDP_P3_0
	{GPIO_SKL_H_GPP_J23, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_J_23_LAN_SDP_P3_1

	{GPIO_SKL_H_GPP_K0, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_K_0_LAN_NCSI_CLK_IN
	{GPIO_SKL_H_GPP_K1, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_K_1_LAN_NCSI_TXD0
	{GPIO_SKL_H_GPP_K2, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_K_2_LAN_NCSI_TXD1
	{GPIO_SKL_H_GPP_K3, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_K_3_LAN_NCSI_TX_EN
	{GPIO_SKL_H_GPP_K4, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_K_4_LAN_NCSI_CRS_DV
	{GPIO_SKL_H_GPP_K5, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_K_5_LAN_NCSI_RXD0
	{GPIO_SKL_H_GPP_K6, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_K_6_LAN_NCSI_RXD1
	{GPIO_SKL_H_GPP_K7, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_K_7
	{GPIO_SKL_H_GPP_K8, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,  GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_K_8_LAN_NCSI_ARB_IN
	{GPIO_SKL_H_GPP_K9, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_K_9_LAN_NCSI_ARB_OUT
	{GPIO_SKL_H_GPP_K10, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_K_10_PE_RST_N

	{GPIO_SKL_H_GPP_L2, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_L_2_TESTCH0_D0
	{GPIO_SKL_H_GPP_L3, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_L_3_TESTCH0_D1
	{GPIO_SKL_H_GPP_L4, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_L_4_TESTCH0_D2
	{GPIO_SKL_H_GPP_L5, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_L_5_TESTCH0_D3
	{GPIO_SKL_H_GPP_L6, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_L_6_TESTCH0_D4
	{GPIO_SKL_H_GPP_L7, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_L_7_TESTCH0_D5
	{GPIO_SKL_H_GPP_L8, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_L_8_TESTCH0_D6
	{GPIO_SKL_H_GPP_L9, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_L_9_TESTCH0_D7
	{GPIO_SKL_H_GPP_L10, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_L_10_TESTCH0_CLK
	{GPIO_SKL_H_GPP_L11, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_L_11_TESTCH1_D0
	{GPIO_SKL_H_GPP_L12, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_L_12_TESTCH1_D1
	{GPIO_SKL_H_GPP_L13, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_L_13_TESTCH1_D2
	{GPIO_SKL_H_GPP_L14, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_L_14_TESTCH1_D3
	{GPIO_SKL_H_GPP_L15, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_L_15_TESTCH1_D4
	{GPIO_SKL_H_GPP_L16, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_L_16_TESTCH1_D5
	{GPIO_SKL_H_GPP_L17, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_L_17_TESTCH1_D6
	{GPIO_SKL_H_GPP_L18, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_L_18_TESTCH1_D7
	{GPIO_SKL_H_GPP_L19, {
		GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,   GpioOutDefault,
		GpioIntDis, GpioResetDeep,  GpioTermNone, GpioPadConfigLock
		} }, //GPP_L_19_TESTCH1_CLK

	{GPIO_SKL_H_GPD0,	{} }, //GPD_0, controlled by ME
	{GPIO_SKL_H_GPD1,	{
		GpioPadModeGpio,   GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock
		} }, //GPD_1_ACPRESENT
	{GPIO_SKL_H_GPD2,	{
		GpioPadModeGpio,   GpioHostOwnGpio, GpioDirIn,		GpioOutLow,
		GpioIntDis, GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock
		} }, //GPD_2_GBE_WAKE_N
	{GPIO_SKL_H_GPD3,	{
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock
		} }, //GPD_3_PWRBTNB_N
	{GPIO_SKL_H_GPD4,	{
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock
		} }, //GPD_4_SLP_S3B
	{GPIO_SKL_H_GPD5,	{
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,
		GpioIntDis, GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock
		} }, //GPD_5_SLP_S4B
	{GPIO_SKL_H_GPD6,	{
		GpioPadModeGpio,   GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock
		} }, //GPD_6_SLPA_N
	{GPIO_SKL_H_GPD7,	{
		GpioPadModeGpio,   GpioHostOwnGpio, GpioDirIn,		GpioOutLow,
		GpioIntDis, GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock
		} }, //GPD_7
	{GPIO_SKL_H_GPD8,	{
		GpioPadModeGpio,   GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock
		} }, //GPD_8_CLK_33K_PCH_SUSCLK_PLD
	{GPIO_SKL_H_GPD9,	{
		GpioPadModeGpio,   GpioHostOwnGpio, GpioDirIn,		GpioOutLow,
		GpioIntDis, GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock
		} }, //GPD_9
	{GPIO_SKL_H_GPD10, {
		GpioPadModeGpio,   GpioHostOwnGpio, GpioDirIn,		GpioOutDefault,
		GpioIntDis, GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock
		} }, //GPD_10_FM_SLPS5_N
	{GPIO_SKL_H_GPD11, {
		GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,		GpioOutLow,
		GpioIntDis, GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock
		} }, //GPD_11_GBEPHY
};

#endif /* _SKXSP_TP_GPIO_H_ */
