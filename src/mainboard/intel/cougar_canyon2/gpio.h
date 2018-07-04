/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The Chromium OS Authors. All rights reserved.
 * Copyright (C) 2013 Sage Electronic Engineering, LLC.
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

#ifndef MAINBOARD_GPIO_H
#define MAINBOARD_GPIO_H

#include <southbridge/intel/fsp_bd82x6x/gpio.h>

const struct pch_gpio_set1 pch_gpio_set1_mode = {
	.gpio0  = GPIO_MODE_GPIO,	/* SINAI */
	.gpio1  = GPIO_MODE_GPIO,	/* SMC_SMI */
	.gpio2  = GPIO_MODE_GPIO,	/* PIRQE# */
	.gpio3  = GPIO_MODE_GPIO,	/* PIRQF# */
	.gpio4  = GPIO_MODE_GPIO,	/* PIRQG# */
	.gpio5  = GPIO_MODE_GPIO,	/* PIRQH# */
	.gpio6  = GPIO_MODE_GPIO,	/* DGPU_HPD_INTR*/
	.gpio7  = GPIO_MODE_GPIO,	/* SMC_SCI# */
	.gpio8  = GPIO_MODE_NONE,	/* NOT USED */
	.gpio9  = GPIO_MODE_NATIVE,	/* USB OC10-11*/
	.gpio10 = GPIO_MODE_NATIVE,	/* USB OC12-13 */
	.gpio11 = GPIO_MODE_GPIO,	/* SMB_ALERT*/
	.gpio12 = GPIO_MODE_NATIVE,	/* LAN DISABLE */
	.gpio13 = GPIO_MODE_NATIVE,	/* HDA_DOCK_RST */
	.gpio14 = GPIO_MODE_GPIO,	/* SMC_WAKE */
	.gpio15 = GPIO_MODE_GPIO,	/* HOST ALERT */
	.gpio16 = GPIO_MODE_NATIVE,	/* SATA4GP */
	.gpio17 = GPIO_MODE_GPIO,	/* DGPU POWEROK */
	.gpio18 = GPIO_MODE_NATIVE,	/* PCIECLKRQ1# */
	.gpio19 = GPIO_MODE_NATIVE,	/* BBS0 */
	.gpio20 = GPIO_MODE_NATIVE,	/* CIECLKRQ2# */
	.gpio21 = GPIO_MODE_NATIVE,	/* SATA0GP */
	.gpio22 = GPIO_MODE_GPIO,	/* BIOS Recovery */
	.gpio23 = GPIO_MODE_NATIVE,	/* LDRQ1 */
	.gpio24 = GPIO_MODE_NONE,	/* HOST ALERT */
	.gpio25 = GPIO_MODE_NATIVE,	/* PCIECLKRQ3# */
	.gpio26 = GPIO_MODE_NATIVE,	/* PCIECLKRQ4# */
	.gpio27 = GPIO_MODE_GPIO,	/* SATA0 PWR EN */
	.gpio28 = GPIO_MODE_GPIO,	/* PLL ODVR */
	.gpio29 = GPIO_MODE_GPIO,	/* SLP_LAN# */
	.gpio30 = GPIO_MODE_NATIVE,	/* SUS_WARN# */
	.gpio31 = GPIO_MODE_NATIVE,	/* ACPRESENT */
};

const struct pch_gpio_set1 pch_gpio_set1_direction = {
	.gpio0  = GPIO_DIR_INPUT,
	.gpio1  = GPIO_DIR_INPUT,
	.gpio2  = GPIO_DIR_INPUT,
	.gpio3  = GPIO_DIR_INPUT,
	.gpio4  = GPIO_DIR_INPUT,
	.gpio5  = GPIO_DIR_INPUT,
	.gpio6  = GPIO_DIR_INPUT,
	.gpio7  = GPIO_DIR_INPUT,
	.gpio8  = GPIO_DIR_OUTPUT,
	.gpio9  = GPIO_DIR_INPUT,
	.gpio10 = GPIO_DIR_INPUT,
	.gpio11 = GPIO_DIR_INPUT,
	.gpio12 = GPIO_DIR_OUTPUT,
	.gpio13 = GPIO_DIR_OUTPUT,
	.gpio14 = GPIO_DIR_INPUT,
	.gpio15 = GPIO_DIR_INPUT,
	.gpio16 = GPIO_DIR_INPUT,
	.gpio17 = GPIO_DIR_INPUT,
	.gpio18 = GPIO_DIR_INPUT,
	.gpio19 = GPIO_DIR_INPUT,
	.gpio20 = GPIO_DIR_INPUT,
	.gpio21 = GPIO_DIR_INPUT,
	.gpio22 = GPIO_DIR_INPUT,
	.gpio23 = GPIO_DIR_INPUT,
	.gpio24 = GPIO_DIR_OUTPUT,
	.gpio25 = GPIO_DIR_INPUT,
	.gpio26 = GPIO_DIR_INPUT,
	.gpio27 = GPIO_DIR_OUTPUT,
	.gpio28 = GPIO_DIR_OUTPUT,
	.gpio29 = GPIO_DIR_OUTPUT,
	.gpio30 = GPIO_DIR_OUTPUT,
	.gpio31 = GPIO_DIR_INPUT,
};

const struct pch_gpio_set1 pch_gpio_set1_level = {
	.gpio0  = GPIO_LEVEL_LOW,
	.gpio1  = GPIO_LEVEL_HIGH,
	.gpio2  = GPIO_LEVEL_HIGH,
	.gpio3  = GPIO_LEVEL_HIGH,
	.gpio4  = GPIO_LEVEL_LOW,
	.gpio5  = GPIO_LEVEL_LOW,
	.gpio6  = GPIO_LEVEL_HIGH,
	.gpio7  = GPIO_LEVEL_HIGH,
	.gpio8  = GPIO_LEVEL_HIGH,
	.gpio9  = GPIO_LEVEL_HIGH,
	.gpio10 = GPIO_LEVEL_HIGH,
	.gpio11 = GPIO_LEVEL_HIGH,
	.gpio12 = GPIO_LEVEL_HIGH,
	.gpio13 = GPIO_LEVEL_LOW,
	.gpio14 = GPIO_LEVEL_HIGH,
	.gpio15 = GPIO_LEVEL_HIGH,
	.gpio16 = GPIO_LEVEL_LOW,
	.gpio17 = GPIO_LEVEL_LOW,
	.gpio18 = GPIO_LEVEL_HIGH,
	.gpio19 = GPIO_LEVEL_HIGH,
	.gpio20 = GPIO_LEVEL_HIGH,
	.gpio21 = GPIO_LEVEL_LOW,
	.gpio22 = GPIO_LEVEL_HIGH,
	.gpio23 = GPIO_LEVEL_HIGH,
	.gpio24 = GPIO_LEVEL_LOW,
	.gpio25 = GPIO_LEVEL_HIGH,
	.gpio26 = GPIO_LEVEL_HIGH,
	.gpio27 = GPIO_LEVEL_LOW,
	.gpio28 = GPIO_LEVEL_LOW,
	.gpio29 = GPIO_LEVEL_HIGH,
	.gpio30 = GPIO_LEVEL_HIGH,
	.gpio31 = GPIO_LEVEL_HIGH,
};

const struct pch_gpio_set1 pch_gpio_set1_invert = {
	.gpio1 = GPIO_INVERT,
	.gpio3 = GPIO_INVERT,
	.gpio7 = GPIO_INVERT,
	.gpio14 = GPIO_INVERT,
	.gpio15 = GPIO_INVERT,
};

const struct pch_gpio_set2 pch_gpio_set2_mode = {
	.gpio32 = GPIO_MODE_NATIVE,	/* SIO CLKREQ */
	.gpio33 = GPIO_MODE_NATIVE,	/* DOCK ENABLE*/
	.gpio34 = GPIO_MODE_GPIO,	/* STP PCI LED */
	.gpio35 = GPIO_MODE_NATIVE,	/* SATA POWER EN */
	.gpio36 = GPIO_MODE_NATIVE,	/* SATA2 PRESENT DET */
	.gpio37 = GPIO_MODE_NONE,	/* NOT USED */
	.gpio38 = GPIO_MODE_GPIO,	/* MFG MODE */
	.gpio39 = GPIO_MODE_GPIO,	/* GP39 GFX CRB DET */
	.gpio40 = GPIO_MODE_NATIVE,	/* USB OC 2-3 */
	.gpio41 = GPIO_MODE_NATIVE,	/* USB OC 4-5 */
	.gpio42 = GPIO_MODE_NATIVE,	/* USB OC 6-7 */
	.gpio43 = GPIO_MODE_NATIVE,	/* USB OC 8-9 */
	.gpio44 = GPIO_MODE_NATIVE,	/* PCI SLOT5 CLKREQ5 */
	.gpio45 = GPIO_MODE_NATIVE,	/* LAN CLKREQ6 */
	.gpio46 = GPIO_MODE_GPIO,	/* PCI SLOT5 CLKREQ5 */
	.gpio47 = GPIO_MODE_NATIVE,	/* PEG CLKREQ7 */
	.gpio48 = GPIO_MODE_GPIO,	/* SV_ADVANCE_GP48 */
	.gpio49 = GPIO_MODE_GPIO,	/* CRIT_TEMP */
	.gpio50 = GPIO_MODE_GPIO,	/* DGPU RESET */
	.gpio51 = GPIO_MODE_NONE,	/* NOT USED */
	.gpio52 = GPIO_MODE_GPIO,	/* DGPU SEL */
	.gpio53 = GPIO_MODE_GPIO,	/* DGPU PWM SEL */
	.gpio54 = GPIO_MODE_GPIO,	/* DGPU PWM EN */
	.gpio55 = GPIO_MODE_NONE,	/* NOT USED */
	.gpio56 = GPIO_MODE_NATIVE,	/* NOT USED */
	.gpio57 = GPIO_MODE_NATIVE,	/* GP57_SV_DETECT */
	.gpio58 = GPIO_MODE_NATIVE,	/* SML1CLK_PCH */
	.gpio59 = GPIO_MODE_NATIVE,	/* USB OC 0-1 */
	.gpio60 = GPIO_MODE_GPIO,	/* DDR RST CTRL */
	.gpio61 = GPIO_MODE_NATIVE,	/* LPC SUSTAT */
	.gpio62 = GPIO_MODE_NATIVE,	/* LPC SUSCLK */
	.gpio63 = GPIO_MODE_NATIVE,	/* SLP S5*/

};

const struct pch_gpio_set2 pch_gpio_set2_direction = {
	.gpio32 = GPIO_DIR_INPUT,
	.gpio33 = GPIO_DIR_OUTPUT,
	.gpio34 = GPIO_DIR_OUTPUT,
	.gpio35 = GPIO_DIR_OUTPUT,
	.gpio36 = GPIO_DIR_INPUT,
	.gpio37 = GPIO_DIR_OUTPUT,
	.gpio38 = GPIO_DIR_INPUT,
	.gpio39 = GPIO_DIR_INPUT,
	.gpio40 = GPIO_DIR_INPUT,
	.gpio41 = GPIO_DIR_INPUT,
	.gpio42 = GPIO_DIR_INPUT,
	.gpio43 = GPIO_DIR_INPUT,
	.gpio44 = GPIO_DIR_INPUT,
	.gpio45 = GPIO_DIR_INPUT,
	.gpio46 = GPIO_DIR_OUTPUT,
	.gpio47 = GPIO_DIR_INPUT,
	.gpio48 = GPIO_DIR_INPUT,
	.gpio49 = GPIO_DIR_INPUT,
	.gpio50 = GPIO_DIR_OUTPUT,
	.gpio51 = GPIO_DIR_OUTPUT,
	.gpio52 = GPIO_DIR_OUTPUT,
	.gpio53 = GPIO_DIR_OUTPUT,
	.gpio54 = GPIO_DIR_OUTPUT,
	.gpio55 = GPIO_DIR_OUTPUT,
	.gpio56 = GPIO_DIR_INPUT,
	.gpio57 = GPIO_DIR_INPUT,
	.gpio58 = GPIO_DIR_INPUT,
	.gpio59 = GPIO_DIR_INPUT,
	.gpio60 = GPIO_DIR_OUTPUT,
	.gpio61 = GPIO_DIR_OUTPUT,
	.gpio62 = GPIO_DIR_OUTPUT,
	.gpio63 = GPIO_DIR_OUTPUT,
};

const struct pch_gpio_set2 pch_gpio_set2_level = {
	.gpio32 = GPIO_LEVEL_HIGH,
	.gpio33 = GPIO_LEVEL_HIGH,
	.gpio34 = GPIO_LEVEL_LOW,
	.gpio35 = GPIO_LEVEL_LOW,
	.gpio36 = GPIO_LEVEL_HIGH,
	.gpio37 = GPIO_LEVEL_LOW,
	.gpio38 = GPIO_LEVEL_HIGH,
	.gpio39 = GPIO_LEVEL_LOW,
	.gpio40 = GPIO_LEVEL_HIGH,
	.gpio41 = GPIO_LEVEL_HIGH,
	.gpio42 = GPIO_LEVEL_HIGH,
	.gpio43 = GPIO_LEVEL_HIGH,
	.gpio44 = GPIO_LEVEL_HIGH,
	.gpio45 = GPIO_LEVEL_HIGH,
	.gpio46 = GPIO_LEVEL_HIGH,
	.gpio47 = GPIO_LEVEL_LOW,
	.gpio48 = GPIO_LEVEL_HIGH,
	.gpio49 = GPIO_LEVEL_HIGH,
	.gpio50 = GPIO_LEVEL_HIGH,
	.gpio51 = GPIO_LEVEL_HIGH,
	.gpio52 = GPIO_LEVEL_HIGH,
	.gpio53 = GPIO_LEVEL_HIGH,
	.gpio54 = GPIO_LEVEL_HIGH,
	.gpio55 = GPIO_LEVEL_LOW,
	.gpio56 = GPIO_LEVEL_HIGH,
	.gpio57 = GPIO_LEVEL_LOW,
	.gpio58 = GPIO_LEVEL_HIGH,
	.gpio59 = GPIO_LEVEL_HIGH,
	.gpio60 = GPIO_LEVEL_HIGH,
	.gpio61 = GPIO_LEVEL_HIGH,
	.gpio62 = GPIO_LEVEL_HIGH,
	.gpio63 = GPIO_LEVEL_HIGH,
};

const struct pch_gpio_set3 pch_gpio_set3_mode = {
	.gpio64 = GPIO_MODE_NATIVE,	/* CLK_FLEX0 */
	.gpio65 = GPIO_MODE_NATIVE,	/* NOT USED / CLK_FLEX1 */
	.gpio66 = GPIO_MODE_GPIO,	/* CLK_FLEX2 */
	.gpio67 = GPIO_MODE_GPIO,	/* GPU PRSNT */
	.gpio68 = GPIO_MODE_GPIO,	/* SATA PORT2 PWR EN*/
	.gpio69 = GPIO_MODE_GPIO,	/* TESTMODE */
	.gpio70 = GPIO_MODE_NATIVE,	/* USB3 SLOT 2DET */
	.gpio71 = GPIO_MODE_NATIVE,	/* USB3 SLOT 1 DET */
	.gpio72 = GPIO_MODE_NATIVE,	/* BATLOW# */
	.gpio73 = GPIO_MODE_NATIVE,	/* PCIECLKRQ0#*/
	.gpio74 = GPIO_MODE_NATIVE,	/* SML1ALERT# /PCHHOT# */
	.gpio75 = GPIO_MODE_NATIVE,	/* SML1DATA */
};

const struct pch_gpio_set3 pch_gpio_set3_direction = {
	.gpio64 = GPIO_DIR_OUTPUT,
	.gpio65 = GPIO_DIR_OUTPUT,
	.gpio66 = GPIO_DIR_OUTPUT,
	.gpio67 = GPIO_DIR_INPUT,
	.gpio68 = GPIO_DIR_OUTPUT,
	.gpio69 = GPIO_DIR_INPUT,
	.gpio70 = GPIO_DIR_INPUT,
	.gpio71 = GPIO_DIR_INPUT,
	.gpio72 = GPIO_DIR_INPUT,
	.gpio73 = GPIO_DIR_INPUT,
	.gpio74 = GPIO_DIR_INPUT,
	.gpio75 = GPIO_DIR_INPUT,
};

const struct pch_gpio_set3 pch_gpio_set3_level = {
	.gpio64 = GPIO_LEVEL_HIGH,
	.gpio65 = GPIO_LEVEL_HIGH,
	.gpio66 = GPIO_LEVEL_LOW,
	.gpio67 = GPIO_LEVEL_HIGH,
	.gpio68 = GPIO_LEVEL_HIGH,
	.gpio69 = GPIO_LEVEL_HIGH,
	.gpio70 = GPIO_LEVEL_HIGH,
	.gpio71 = GPIO_LEVEL_HIGH,
	.gpio72 = GPIO_LEVEL_HIGH,
	.gpio73 = GPIO_LEVEL_HIGH,
	.gpio74 = GPIO_LEVEL_HIGH,
	.gpio75 = GPIO_LEVEL_HIGH,
};

const struct pch_gpio_map gpio_map = {
	.set1 = {
		.mode      = &pch_gpio_set1_mode,
		.direction = &pch_gpio_set1_direction,
		.level     = &pch_gpio_set1_level,
		.invert    = &pch_gpio_set1_invert,
	},
	.set2 = {
		.mode      = &pch_gpio_set2_mode,
		.direction = &pch_gpio_set2_direction,
		.level     = &pch_gpio_set2_level,
	},
	.set3 = {
		.mode      = &pch_gpio_set3_mode,
		.direction = &pch_gpio_set3_direction,
		.level     = &pch_gpio_set3_level,
	},
};
#endif
