/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The Chromium OS Authors. All rights reserved.
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef STUMPY_GPIO_H
#define STUMPY_GPIO_H

#include "southbridge/intel/bd82x6x/gpio.h"

/*
 * GPIO SET 1 includes GPIO0 to GPIO31
 */

const struct pch_gpio_set1 pch_gpio_set1_mode = {
	.gpio0  = GPIO_MODE_GPIO,	/* CHP3_SERDBG */
	.gpio1  = GPIO_MODE_GPIO,	/* SIO3_EXTSMI# */
	.gpio2  = GPIO_MODE_NONE,
	.gpio3  = GPIO_MODE_NONE,
	.gpio4  = GPIO_MODE_NONE,
	.gpio5  = GPIO_MODE_NONE,
	.gpio6  = GPIO_MODE_NONE,
	.gpio7  = GPIO_MODE_NONE,
	.gpio8  = GPIO_MODE_GPIO,	/* CHP3_INTELBT_OFF# */
	.gpio9  = GPIO_MODE_NATIVE,	/* USB_OC13# */
	.gpio10 = GPIO_MODE_NATIVE,	/* USB_OC12# */
	.gpio11 = GPIO_MODE_NONE,
	.gpio12 = GPIO_MODE_NONE,
	.gpio13 = GPIO_MODE_GPIO,	/* CHP3_DEBUG13 */
	.gpio14 = GPIO_MODE_GPIO,	/* SIO3_WAKESCI# */
	.gpio15 = GPIO_MODE_NONE,
	.gpio16 = GPIO_MODE_NONE,
	.gpio17 = GPIO_MODE_GPIO,	/* KBC3_DVP_MODE */
	.gpio18 = GPIO_MODE_NATIVE,	/* MIN3_CLKREQ1# */
	.gpio19 = GPIO_MODE_NONE,
	.gpio20 = GPIO_MODE_NONE,
	.gpio21 = GPIO_MODE_NONE,
	.gpio22 = GPIO_MODE_GPIO,	/* CHP3_BIOS_CRISIS# */
	.gpio23 = GPIO_MODE_NONE,
	.gpio24 = GPIO_MODE_NONE,
	.gpio25 = GPIO_MODE_NATIVE,	/* MIN3_CLKREQ3# */
	.gpio26 = GPIO_MODE_NATIVE,	/* LAN3_CLKREQ# */
	.gpio27 = GPIO_MODE_NONE,
	.gpio28 = GPIO_MODE_NONE,
	.gpio29 = GPIO_MODE_NONE,
	.gpio30 = GPIO_MODE_NATIVE,	/* CHP3_SUSWARN# */
	.gpio31 = GPIO_MODE_NATIVE,	/* ACPRESENT (pullup) */
};

const struct pch_gpio_set1 pch_gpio_set1_direction = {
	.gpio0  = GPIO_DIR_OUTPUT,
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
	.gpio12 = GPIO_DIR_INPUT,
	.gpio13 = GPIO_DIR_INPUT,
	.gpio14 = GPIO_DIR_INPUT,
	.gpio15 = GPIO_DIR_INPUT,
	.gpio16 = GPIO_DIR_INPUT,
	.gpio17 = GPIO_DIR_INPUT,
	.gpio18 = GPIO_DIR_INPUT,
	.gpio19 = GPIO_DIR_INPUT,
	.gpio20 = GPIO_DIR_INPUT,
	.gpio21 = GPIO_DIR_INPUT,
	.gpio22 = GPIO_DIR_OUTPUT,
	.gpio23 = GPIO_DIR_INPUT,
	.gpio24 = GPIO_DIR_INPUT,
	.gpio25 = GPIO_DIR_INPUT,
	.gpio26 = GPIO_DIR_INPUT,
	.gpio27 = GPIO_DIR_INPUT,
	.gpio28 = GPIO_DIR_INPUT,
	.gpio29 = GPIO_DIR_INPUT,
	.gpio30 = GPIO_DIR_INPUT,
	.gpio31 = GPIO_DIR_INPUT,
};

const struct pch_gpio_set1 pch_gpio_set1_level = {
	.gpio0  = GPIO_LEVEL_LOW,
	.gpio1  = GPIO_LEVEL_LOW,
	.gpio2  = GPIO_LEVEL_LOW,
	.gpio3  = GPIO_LEVEL_LOW,
	.gpio4  = GPIO_LEVEL_LOW,
	.gpio5  = GPIO_LEVEL_LOW,
	.gpio6  = GPIO_LEVEL_LOW,
	.gpio7  = GPIO_LEVEL_LOW,
	.gpio8  = GPIO_LEVEL_HIGH,
	.gpio9  = GPIO_LEVEL_LOW,
	.gpio10 = GPIO_LEVEL_LOW,
	.gpio11 = GPIO_LEVEL_LOW,
	.gpio12 = GPIO_LEVEL_LOW,
	.gpio13 = GPIO_LEVEL_LOW,
	.gpio14 = GPIO_LEVEL_LOW,
	.gpio15 = GPIO_LEVEL_LOW,
	.gpio16 = GPIO_LEVEL_LOW,
	.gpio17 = GPIO_LEVEL_LOW,
	.gpio18 = GPIO_LEVEL_LOW,
	.gpio19 = GPIO_LEVEL_LOW,
	.gpio20 = GPIO_LEVEL_LOW,
	.gpio21 = GPIO_LEVEL_LOW,
	.gpio22 = GPIO_LEVEL_HIGH,
	.gpio23 = GPIO_LEVEL_LOW,
	.gpio24 = GPIO_LEVEL_LOW,
	.gpio25 = GPIO_LEVEL_LOW,
	.gpio26 = GPIO_LEVEL_LOW,
	.gpio27 = GPIO_LEVEL_LOW,
	.gpio28 = GPIO_LEVEL_LOW,
	.gpio29 = GPIO_LEVEL_LOW,
	.gpio30 = GPIO_LEVEL_LOW,
	.gpio31 = GPIO_LEVEL_LOW,
};

/*
 * GPIO SET 2 includes GPIO32 to GPIO63
 */

const struct pch_gpio_set2 pch_gpio_set2_mode = {
	.gpio32 = GPIO_MODE_NATIVE,	/* PCI3_CLKRUN# */
	.gpio33 = GPIO_MODE_NONE,
	.gpio34 = GPIO_MODE_NONE,
	.gpio35 = GPIO_MODE_GPIO,	/* CHP3_WLAN_OFF# */
	.gpio36 = GPIO_MODE_GPIO,
	.gpio37 = GPIO_MODE_GPIO,	/* CHP3_FDI_OVRVLTG */
	.gpio38 = GPIO_MODE_NONE,
	.gpio39 = GPIO_MODE_NONE,
	.gpio40 = GPIO_MODE_NATIVE,	/* USB3_OC1# */
	.gpio41 = GPIO_MODE_NATIVE,	/* USB3_OC4# */
	.gpio42 = GPIO_MODE_GPIO,	/* CHP3_REC_MODE# */
	.gpio43 = GPIO_MODE_NATIVE,	/* USB3_OC8# */
	.gpio44 = GPIO_MODE_GPIO,	/* CHP3_SMRT_CHG0_CTL2# */
	.gpio45 = GPIO_MODE_GPIO,	/* CHP3_SMRT_CHG0_CTL3# */
	.gpio46 = GPIO_MODE_GPIO,	/* CHP3_SMRT_CHG4_CTL2# */
	.gpio47 = GPIO_MODE_GPIO,	/* CHP3_CHG_ENABLE0 */
	.gpio48 = GPIO_MODE_GPIO,	/* CHP3_BT_OFF# */
	.gpio49 = GPIO_MODE_NONE,
	.gpio50 = GPIO_MODE_NONE,
	.gpio51 = GPIO_MODE_NONE,
	.gpio52 = GPIO_MODE_NONE,
	.gpio53 = GPIO_MODE_NATIVE,
	.gpio54 = GPIO_MODE_NONE,
	.gpio55 = GPIO_MODE_GPIO,	/* STP_A16OVR */
	.gpio56 = GPIO_MODE_GPIO,	/* CHP3_CHG_ENABLE4 */
	.gpio57 = GPIO_MODE_GPIO,	/* CHP3_DEBUG10 */
	.gpio58 = GPIO_MODE_NATIVE,	/* SIO3_THERM_SMCLK# */
	.gpio59 = GPIO_MODE_NATIVE,	/* USB3_OC0# */
	.gpio60 = GPIO_MODE_GPIO,	/* CHP3_DRAMRST_GATE */
	.gpio61 = GPIO_MODE_NONE,
	.gpio62 = GPIO_MODE_NATIVE,	/* CHP3_SUSCLK */
	.gpio63 = GPIO_MODE_NATIVE,	/* CHP3_SLPS5# */
};

const struct pch_gpio_set2 pch_gpio_set2_direction = {
	.gpio32 = GPIO_DIR_INPUT,
	.gpio33 = GPIO_DIR_INPUT,
	.gpio34 = GPIO_DIR_INPUT,
	.gpio35 = GPIO_DIR_OUTPUT,
	.gpio36 = GPIO_DIR_INPUT,
	.gpio37 = GPIO_DIR_INPUT,
	.gpio38 = GPIO_DIR_INPUT,
	.gpio39 = GPIO_DIR_INPUT,
	.gpio40 = GPIO_DIR_INPUT,
	.gpio41 = GPIO_DIR_INPUT,
	.gpio42 = GPIO_DIR_INPUT,
	.gpio43 = GPIO_DIR_INPUT,
	.gpio44 = GPIO_DIR_OUTPUT,
	.gpio45 = GPIO_DIR_OUTPUT,
	.gpio46 = GPIO_DIR_OUTPUT,
	.gpio47 = GPIO_DIR_OUTPUT,
	.gpio48 = GPIO_DIR_OUTPUT,
	.gpio49 = GPIO_DIR_INPUT,
	.gpio50 = GPIO_DIR_INPUT,
	.gpio51 = GPIO_DIR_INPUT,
	.gpio52 = GPIO_DIR_INPUT,
	.gpio53 = GPIO_DIR_INPUT,
	.gpio54 = GPIO_DIR_INPUT,
	.gpio55 = GPIO_DIR_INPUT,
	.gpio56 = GPIO_DIR_OUTPUT,
	.gpio57 = GPIO_DIR_OUTPUT,
	.gpio58 = GPIO_DIR_INPUT,
	.gpio59 = GPIO_DIR_INPUT,
	.gpio60 = GPIO_DIR_OUTPUT,
	.gpio61 = GPIO_DIR_INPUT,
	.gpio62 = GPIO_DIR_INPUT,
	.gpio63 = GPIO_DIR_INPUT,
};

const struct pch_gpio_set2 pch_gpio_set2_level = {
	.gpio32 = GPIO_LEVEL_LOW,
	.gpio33 = GPIO_LEVEL_LOW,
	.gpio34 = GPIO_LEVEL_LOW,
	.gpio35 = GPIO_LEVEL_HIGH,
	.gpio36 = GPIO_LEVEL_LOW,
	.gpio37 = GPIO_LEVEL_LOW,
	.gpio38 = GPIO_LEVEL_LOW,
	.gpio39 = GPIO_LEVEL_LOW,
	.gpio40 = GPIO_LEVEL_LOW,
	.gpio41 = GPIO_LEVEL_LOW,
	.gpio42 = GPIO_LEVEL_LOW,
	.gpio43 = GPIO_LEVEL_LOW,
	.gpio44 = GPIO_LEVEL_HIGH,
	.gpio45 = GPIO_LEVEL_LOW,
	.gpio46 = GPIO_LEVEL_HIGH,
	.gpio47 = GPIO_LEVEL_HIGH,
	.gpio48 = GPIO_LEVEL_HIGH,
	.gpio49 = GPIO_LEVEL_LOW,
	.gpio50 = GPIO_LEVEL_LOW,
	.gpio51 = GPIO_LEVEL_LOW,
	.gpio52 = GPIO_LEVEL_LOW,
	.gpio53 = GPIO_LEVEL_LOW,
	.gpio54 = GPIO_LEVEL_LOW,
	.gpio55 = GPIO_LEVEL_LOW,
	.gpio56 = GPIO_LEVEL_HIGH,
	.gpio57 = GPIO_LEVEL_LOW,
	.gpio58 = GPIO_LEVEL_LOW,
	.gpio59 = GPIO_LEVEL_LOW,
	.gpio60 = GPIO_LEVEL_HIGH,
	.gpio61 = GPIO_LEVEL_LOW,
	.gpio62 = GPIO_LEVEL_LOW,
	.gpio63 = GPIO_LEVEL_LOW,
};

/*
 * GPIO SET 3 includes GPIO64 to GPIO75
 */

const struct pch_gpio_set3 pch_gpio_set3_mode = {
	.gpio64 = GPIO_MODE_NATIVE,	/* CLK3_SIO48 */
	.gpio65 = GPIO_MODE_NONE,
	.gpio66 = GPIO_MODE_NONE,
	.gpio67 = GPIO_MODE_NONE,
	.gpio68 = GPIO_MODE_GPIO,	/* CHP3_SPI_WP */
	.gpio69 = GPIO_MODE_NONE,
	.gpio70 = GPIO_MODE_NONE,
	.gpio71 = GPIO_MODE_NONE,
	.gpio72 = GPIO_MODE_NATIVE,	/* BATLOW# (pullup) */
	.gpio73 = GPIO_MODE_GPIO,	/* CHP3_SMRT_CHG4_CTL3# */
	.gpio74 = GPIO_MODE_NONE,
	.gpio75 = GPIO_MODE_NATIVE,	/* SIO3_THERM_SMDATA# */
};

const struct pch_gpio_set3 pch_gpio_set3_direction = {
	.gpio64 = GPIO_DIR_INPUT,
	.gpio65 = GPIO_DIR_INPUT,
	.gpio66 = GPIO_DIR_INPUT,
	.gpio67 = GPIO_DIR_INPUT,
	.gpio68 = GPIO_DIR_INPUT,
	.gpio69 = GPIO_DIR_INPUT,
	.gpio70 = GPIO_DIR_INPUT,
	.gpio71 = GPIO_DIR_INPUT,
	.gpio72 = GPIO_DIR_INPUT,
	.gpio73 = GPIO_DIR_OUTPUT,
	.gpio74 = GPIO_DIR_INPUT,
	.gpio75 = GPIO_DIR_INPUT,
};

const struct pch_gpio_set3 pch_gpio_set3_level = {
	.gpio64 = GPIO_LEVEL_LOW,
	.gpio65 = GPIO_LEVEL_LOW,
	.gpio66 = GPIO_LEVEL_LOW,
	.gpio67 = GPIO_LEVEL_LOW,
	.gpio68 = GPIO_LEVEL_LOW,
	.gpio69 = GPIO_LEVEL_LOW,
	.gpio70 = GPIO_LEVEL_LOW,
	.gpio71 = GPIO_LEVEL_LOW,
	.gpio72 = GPIO_LEVEL_LOW,
	.gpio73 = GPIO_LEVEL_LOW,
	.gpio74 = GPIO_LEVEL_LOW,
	.gpio75 = GPIO_LEVEL_LOW,
};

const struct pch_gpio_map stumpy_gpio_map = {
	.set1 = {
		.mode      = &pch_gpio_set1_mode,
		.direction = &pch_gpio_set1_direction,
		.level     = &pch_gpio_set1_level,
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
