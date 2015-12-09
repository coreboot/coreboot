/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2014 Sage Electronic Engineering, LLC.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdlib.h>
#include <soc/gpio.h>
#include "irqroute.h"

/*
 * For multiplexed functions, look in EDS:
 * 10.3 Ball Name and Function by Location
 *
 * The pads list is in the BWG_VOL2 Rev1p2:
 * Note that Pad # is not the same as GPIO#
 * 37 GPIO Handling:
 *  Table 37-1. SCORE Pads List
 *  Table 37-2. SSUSORE Pads List
 */

/* NCORE GPIOs */
static const struct soc_gpio_map gpncore_gpio_map[] = {
	GPIO_FUNC2,				/* GPIO_S0_NC[00] - HDMI_HPD */
	GPIO_FUNC2,				/* GPIO_S0_NC[01] - HDMI_DDCDAT */
	GPIO_FUNC2,				/* GPIO_S0_NC[02] - HDMI_DDCCLK */
	GPIO_NC,				/* GPIO_S0_NC[03] - No Connect */
	GPIO_NC,				/* GPIO_S0_NC[04] - No Connect */
	GPIO_NC,				/* GPIO_S0_NC[05] - No Connect */
	GPIO_NC,				/* GPIO_S0_NC[06] - No Connect */
	GPIO_FUNC2,				/* GPIO_S0_NC[07] - DDI1_DDCDAT */
	GPIO_NC,				/* GPIO_S0_NC[08] - No Connect */
	GPIO_NC,				/* GPIO_S0_NC[09] - No Connect */
	GPIO_NC,				/* GPIO_S0_NC[10] - No Connect */
	GPIO_NC,				/* GPIO_S0_NC[11] - No Connect */
	GPIO_FUNC(0, PULL_UP, 20K),		/* GPIO_S0_NC[12] - TP15 */
	GPIO_NC,				/* GPIO_S0_NC[13] - No Connect */
	GPIO_NC,				/* GPIO_S0_NC[14] - No Connect */
	GPIO_NC,				/* GPIO_S0_NC[15] - No Connect */
	GPIO_NC,				/* GPIO_S0_NC[16] - No Connect */
	GPIO_NC,				/* GPIO_S0_NC[17] - No Connect */
	GPIO_NC,				/* GPIO_S0_NC[18] - No Connect */
	GPIO_NC,				/* GPIO_S0_NC[19] - No Connect */
	GPIO_NC,				/* GPIO_S0_NC[20] - No Connect */
	GPIO_NC,				/* GPIO_S0_NC[21] - No Connect */
	GPIO_NC,				/* GPIO_S0_NC[22] - No Connect */
	GPIO_NC,				/* GPIO_S0_NC[23] - No Connect */
	GPIO_NC,				/* GPIO_S0_NC[24] - No Connect */
	GPIO_NC,				/* GPIO_S0_NC[25] - No Connect */
	GPIO_NC,				/* GPIO_S0_NC[26] - No Connect */
	GPIO_END
};

/* SCORE GPIOs (GPIO_S0_SC_XX)*/
static const struct soc_gpio_map gpscore_gpio_map[] = {
	GPIO_FUNC1,				/* GPIO_S0_SC[000] - SATA_GP0 */
	GPIO_FUNC1,				/* GPIO_S0_SC[001] - SATA_GP1 */
	GPIO_FUNC1,				/* GPIO_S0_SC[002] - SATA_LED_B */
	GPIO_FUNC1,				/* GPIO_S0_SC[003] - PCIE_CLKREQ_0 */
	GPIO_FUNC1,				/* GPIO_S0_SC[004] - PCIE_CLKREQ_1 */
	GPIO_FUNC1,				/* GPIO_S0_SC[005] - PCIE_CLKREQ_2 */
	GPIO_FUNC1,				/* GPIO_S0_SC[006] - PCIE_CLKREQ_3 */
	GPIO_FUNC2,				/* GPIO_S0_SC[007] - SD3_WP */
	GPIO_NC,				/* GPIO_S0_SC[008] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[009] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[010] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[011] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[012] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[013] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[014] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[015] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[016] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[017] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[018] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[019] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[020] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[021] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[022] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[023] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[024] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[025] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[026] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[027] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[028] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[029] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[030] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[031] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[032] - No Connect */
	GPIO_FUNC1,				/* GPIO_S0_SC[033] - SD3_CLK */
	GPIO_FUNC1,				/* GPIO_S0_SC[034] - SD3_D0 */
	GPIO_FUNC1,				/* GPIO_S0_SC[035] - SD3_D1 */
	GPIO_FUNC1,				/* GPIO_S0_SC[036] - SD3_D2 */
	GPIO_FUNC1,				/* GPIO_S0_SC[037] - SD3_D3 */
	GPIO_FUNC1,				/* GPIO_S0_SC[038] - SD3_CD# */
	GPIO_FUNC1,				/* GPIO_S0_SC[039] - SD3_CMD */
	GPIO_FUNC1,				/* GPIO_S0_SC[040] - No Connect */
	GPIO_FUNC1,				/* GPIO_S0_SC[041] - /SD3_PWREN */
	GPIO_NC,				/* GPIO_S0_SC[042] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[043] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[044] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[045] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[046] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[047] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[048] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[049] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[050] - No Connect */
	GPIO_FUNC1,				/* GPIO_S0_SC[051] - PCU_SMB_DATA */
	GPIO_FUNC1,				/* GPIO_S0_SC[052] - PCU_SMB_CLK */
	GPIO_FUNC1,				/* GPIO_S0_SC[053] - PCU_SMB_ALERT */
	GPIO_FUNC1,				/* GPIO_S0_SC[054] - ILB_8254_SPKR */
	GPIO_NC,				/* GPIO_S0_SC[055] - No Connect */
	GPIO_FUNC0,				/* GPIO_S0_SC[056] - GPIO_S0_SC_56 */
	GPIO_FUNC1,				/* GPIO_S0_SC[057] - PCU_UART3_TXD */
	GPIO_NC,				/* GPIO_S0_SC[058] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[059] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[060] - No Connect */
	GPIO_FUNC1,				/* GPIO_S0_SC[061] - PCU_UART3_RXD */
	GPIO_FUNC1,				/* GPIO_S0_SC[062] - LPE_I2S_CLK */
	GPIO_FUNC1,				/* GPIO_S0_SC[063] - LPE_I2S_FRM */
	GPIO_FUNC1,				/* GPIO_S0_SC[064] - LPE_I2S_DATIN */
	GPIO_FUNC1,				/* GPIO_S0_SC[065] - LPE_I2S_DATOUT */
	GPIO_FUNC1,				/* GPIO_S0_SC[066] - SOC_SIO_SPI_CS1 */
	GPIO_FUNC1,				/* GPIO_S0_SC[067] - SOC_SIO_SPI_MISO */
	GPIO_FUNC1,				/* GPIO_S0_SC[068] - SOC_SIO_SPI_MOSI */
	GPIO_FUNC1,				/* GPIO_S0_SC[069] - SOC_SIO_SPI_CLK */
	GPIO_FUNC1,				/* GPIO_S0_SC[070] - SIO_UART1_RXD */
	GPIO_FUNC1,				/* GPIO_S0_SC[071] - SIO_UART1_TXD */
	GPIO_NC,				/* GPIO_S0_SC[072] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[073] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[074] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[075] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[076] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[077] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[078] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[079] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[080] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[081] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[082] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[083] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[084] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[085] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[086] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[087] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[088] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[089] - No Connect */
	GPIO_FUNC1,				/* GPIO_S0_SC[090] - EXP_I2C_SDA */
	GPIO_FUNC1,				/* GPIO_S0_SC[091] - EXP_I2C_SCL */
	GPIO_FUNC1,		                /* GPIO_S0_SC[092] - 0R GND? */
	GPIO_FUNC1,                             /* GPIO_S0_SC[093] - 0R GND? */
	GPIO_FUNC1,				/* GPIO_S0_SC[094] - SOC_PWM0 */
	GPIO_FUNC1,				/* GPIO_S0_SC[095] - SOC_PWM1 */
	GPIO_NC,				/* GPIO_S0_SC[096] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[097] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[098] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[099] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[100] - No Connect */
	GPIO_NC,				/* GPIO_S0_SC[101] - No Connect */
	GPIO_END
};

/* SSUS GPIOs (GPIO_S5) */
static const struct soc_gpio_map gpssus_gpio_map[] = {
	GPIO_NC,				/* GPIO_S5[00] - No Connect */
	GPIO_FUNC6,				/* GPIO_S5[01] - PMC_WAKE_PCIE[1] */
	GPIO_FUNC6,				/* GPIO_S5[02] - PMC_WAKE_PCIE[2] */
	GPIO_FUNC6,				/* GPIO_S5[03] - PMC_WAKE_PCIE[3] */
	GPIO_NC,				/* GPIO_S5[04] - No Connect */
	GPIO_NC,				/* GPIO_S5[05] - No Connect */
	GPIO_NC,				/* GPIO_S5[06] - No Connect */
	GPIO_NC,				/* GPIO_S5[07] - No Connect */
	GPIO_NC,				/* GPIO_S5[08] - No Connect */
	GPIO_NC,				/* GPIO_S5[09] - No Connect */
	GPIO_OUT_HIGH,				/* GPIO_S5[10] - GPIO_S5_10_UNLOCK */
	GPIO_FUNC0,				/* GPIO_S5[11] - SUSPWRDNACK */
	GPIO_NC,				/* GPIO_S5[12] - No Connect */
	GPIO_NC,				/* GPIO_S5[13] - No Connect */
	GPIO_FUNC1,				/* GPIO_S5[14] - GPIO_S514_J20 */
	GPIO_FUNC0,				/* GPIO_S5[15] - PMC_WAKE_PCIE[0] */
	GPIO_FUNC(1, PULL_UP, 2K),		/* GPIO_S5[16] - No Connect */
	GPIO_NC,				/* GPIO_S5[17] - No Connect */
	GPIO_FUNC1,				/* GPIO_S5[18] - T360 */
	GPIO_FUNC0,				/* GPIO_S5[19] - SOC_USB_HOST_OC0 */
	GPIO_FUNC0,				/* GPIO_S5[20] - SOC_USB_HOST_OC1 */
	GPIO_FUNC0,				/* GPIO_S5[21] - SOC_SPI_CS1B */
	GPIO_NC,				/* GPIO_S5[22] - No Connect */
	GPIO_NC,				/* GPIO_S5[23] - No Connect */
	GPIO_NC,				/* GPIO_S5[24] - No Connect */
	GPIO_NC,				/* GPIO_S5[25] - No Connect */
	GPIO_NC,				/* GPIO_S5[26] - No Connect */
	GPIO_FUNC(0, PULL_DISABLE, 10K),	/* GPIO_S5[27] - SW450-1 */
	GPIO_FUNC(0, PULL_DISABLE, 10K),	/* GPIO_S5[28] - SW450-2 */
	GPIO_FUNC(0, PULL_DISABLE, 10K),	/* GPIO_S5[29] - SW450-3 */
	GPIO_FUNC(0, PULL_DISABLE, 10K),        /* GPIO_S5[30] - SW450-4 */
	GPIO_NC,				/* GPIO_S5[31] - No Connect */
	GPIO_NC,				/* GPIO_S5[32] - No Connect */
	GPIO_NC,				/* GPIO_S5[33] - No Connect */
	GPIO_NC,				/* GPIO_S5[34] - No Connect */
	GPIO_NC,				/* GPIO_S5[35] - No Connect */
	GPIO_NC,				/* GPIO_S5[36] - No Connect */
	GPIO_NC,				/* GPIO_S5[37] - No Connect */
	GPIO_NC,				/* GPIO_S5[38] - No Connect */
	GPIO_NC,				/* GPIO_S5[39] - No Connect */
	GPIO_NC,				/* GPIO_S5[40] - No Connect */
	GPIO_NC,				/* GPIO_S5[41] - No Connect */
	GPIO_NC,				/* GPIO_S5[42] - No Connect */
	GPIO_NC,				/* GPIO_S5[43] - No Connect */
	GPIO_END
};

static struct soc_gpio_config gpio_config = {
	.ncore = gpncore_gpio_map,
	.score = gpscore_gpio_map,
	.ssus = gpssus_gpio_map,
	.core_dirq = NULL,
	.sus_dirq = NULL,
};

struct soc_gpio_config* mainboard_get_gpios(void)
{
	return &gpio_config;
}
