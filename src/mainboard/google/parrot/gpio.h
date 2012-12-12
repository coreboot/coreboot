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

#ifndef PARROT_GPIO_H
#define PARROT_GPIO_H

#include "southbridge/intel/bd82x6x/gpio.h"

const struct pch_gpio_set1 pch_gpio_set1_mode = {
	.gpio0  = GPIO_MODE_NONE,	/* NOT USED */
	.gpio1  = GPIO_MODE_NONE,	/* NOT USED */
	.gpio2  = GPIO_MODE_NATIVE,	/* NOT USED / PIRQE# */
	.gpio3  = GPIO_MODE_NONE,	/* NOT USED / PIRQ#F */
	.gpio4  = GPIO_MODE_NONE,	/* NOT USED / PIRQG# */
	.gpio5  = GPIO_MODE_NONE,	/* NOT USED / PIRQH# */
	.gpio6  = GPIO_MODE_NONE,	/* NOT USED / FAN TACH2 */
	.gpio7  = GPIO_MODE_GPIO,	/* EC_SCI# */
	.gpio8  = GPIO_MODE_GPIO,	/* EC SMI# */
	.gpio9  = GPIO_MODE_NATIVE,	/* NOT USED / OC5# USB */
	.gpio10 = GPIO_MODE_NATIVE,	/* NOT USED / OC6# USB */
	.gpio11 = GPIO_MODE_NONE,	/* NOT USED / SMB_ALERT*/
	.gpio12 = GPIO_MODE_GPIO,	/* Track Pad IRQ / LAN_PHY_PWR_CTRL / SMB_ALERT */
	.gpio13 = GPIO_MODE_NONE,	/* NOT USED / HDA_DOCK_RST */
	.gpio14 = GPIO_MODE_NATIVE,	/* NOT USED / OC7# USB */
	.gpio15 = GPIO_MODE_GPIO,	/* EC_LID_OUT (INPUT to PantherPoint) */
	.gpio16 = GPIO_MODE_NONE,	/* NOT USED / SATA4GP */
	.gpio17 = GPIO_MODE_GPIO,	/* DEV MODE */
	.gpio18 = GPIO_MODE_NATIVE,	/* PCIECLKRQ1# */
	.gpio19 = GPIO_MODE_NONE,	/* BIOS BOOT STRAP (NOT USED)/ SATA1GP */
	.gpio20 = GPIO_MODE_NONE,	/* NOT USED / PCIECLKRQ2# */
	.gpio21 = GPIO_MODE_NONE,	/* NOT USED / SATA0GP */
	.gpio22 = GPIO_MODE_NONE,	/* NOT USED */
	.gpio23 = GPIO_MODE_NONE,	/* NOT USED */
	.gpio24 = GPIO_MODE_NONE,	/* NOT USED / MEM_LED */
	.gpio25 = GPIO_MODE_NATIVE,	/* PCIECLKRQ3# */
	.gpio26 = GPIO_MODE_NONE,	/* NOT USED / PCIECLKRQ4# */
	.gpio27 = GPIO_MODE_NONE,	/* S4,S5 WAKE? */
	.gpio28 = GPIO_MODE_NONE,	/* On-Die PLL Voltage Regulator */
	.gpio29 = GPIO_MODE_NONE,	/* NOT USED / SLP_LAN# */
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
	.gpio8  = GPIO_DIR_INPUT,
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
	.gpio22 = GPIO_DIR_INPUT,
	.gpio23 = GPIO_DIR_INPUT,
	.gpio24 = GPIO_DIR_INPUT,
	.gpio25 = GPIO_DIR_INPUT,
	.gpio26 = GPIO_DIR_INPUT,
	.gpio27 = GPIO_DIR_INPUT,
	.gpio28 = GPIO_DIR_INPUT,
	.gpio29 = GPIO_DIR_INPUT,
	.gpio30 = GPIO_DIR_OUTPUT,
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
	.gpio8  = GPIO_LEVEL_LOW,
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
	.gpio22 = GPIO_LEVEL_LOW,
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

const struct pch_gpio_set1 pch_gpio_set1_invert = {
	.gpio7 = GPIO_INVERT,
	.gpio8 = GPIO_INVERT,
	.gpio12 = GPIO_INVERT,
	.gpio15 = GPIO_INVERT,
};

const struct pch_gpio_set2 pch_gpio_set2_mode = {
	.gpio36 = GPIO_MODE_GPIO, /* W_DISABLE_L */
	.gpio41 = GPIO_MODE_GPIO, /* SPD vector D0 */
	.gpio42 = GPIO_MODE_GPIO, /* SPD vector D1 */
	.gpio43 = GPIO_MODE_GPIO, /* SPD vector D2 */
	.gpio57 = GPIO_MODE_GPIO, /* PCH_SPI_WP_D */
	.gpio60 = GPIO_MODE_GPIO, /* DRAMRST_CNTRL_PCH */
};

const struct pch_gpio_set2 pch_gpio_set2_direction = {
	.gpio32 = GPIO_DIR_INPUT,
	.gpio33 = GPIO_DIR_INPUT,
	.gpio34 = GPIO_DIR_INPUT,
	.gpio35 = GPIO_DIR_INPUT,
	.gpio36 = GPIO_DIR_INPUT,
	.gpio37 = GPIO_DIR_INPUT,
	.gpio38 = GPIO_DIR_INPUT,
	.gpio39 = GPIO_DIR_INPUT,
	.gpio40 = GPIO_DIR_INPUT,
	.gpio41 = GPIO_DIR_INPUT,
	.gpio42 = GPIO_DIR_INPUT,
	.gpio43 = GPIO_DIR_INPUT,
	.gpio44 = GPIO_DIR_INPUT,
	.gpio45 = GPIO_DIR_INPUT,
	.gpio46 = GPIO_DIR_INPUT,
	.gpio47 = GPIO_DIR_INPUT,
	.gpio48 = GPIO_DIR_INPUT,
	.gpio49 = GPIO_DIR_INPUT,
	.gpio50 = GPIO_DIR_INPUT,
	.gpio51 = GPIO_DIR_INPUT,
	.gpio52 = GPIO_DIR_INPUT,
	.gpio53 = GPIO_DIR_INPUT,
	.gpio54 = GPIO_DIR_INPUT,
	.gpio55 = GPIO_DIR_INPUT,
	.gpio56 = GPIO_DIR_INPUT,
	.gpio57 = GPIO_DIR_INPUT,
	.gpio58 = GPIO_DIR_INPUT,
	.gpio59 = GPIO_DIR_INPUT,
	.gpio60 = GPIO_DIR_INPUT,
	.gpio61 = GPIO_DIR_INPUT,
	.gpio62 = GPIO_DIR_INPUT,
	.gpio63 = GPIO_DIR_INPUT,
};

const struct pch_gpio_set2 pch_gpio_set2_level = {
	.gpio32 = GPIO_LEVEL_LOW,
	.gpio33 = GPIO_LEVEL_LOW,
	.gpio34 = GPIO_LEVEL_LOW,
	.gpio35 = GPIO_LEVEL_LOW,
	.gpio36 = GPIO_LEVEL_LOW,
	.gpio37 = GPIO_LEVEL_LOW,
	.gpio38 = GPIO_LEVEL_LOW,
	.gpio39 = GPIO_LEVEL_LOW,
	.gpio40 = GPIO_LEVEL_LOW,
	.gpio41 = GPIO_LEVEL_LOW,
	.gpio42 = GPIO_LEVEL_LOW,
	.gpio43 = GPIO_LEVEL_LOW,
	.gpio44 = GPIO_LEVEL_LOW,
	.gpio45 = GPIO_LEVEL_LOW,
	.gpio46 = GPIO_LEVEL_LOW,
	.gpio47 = GPIO_LEVEL_LOW,
	.gpio48 = GPIO_LEVEL_LOW,
	.gpio49 = GPIO_LEVEL_LOW,
	.gpio50 = GPIO_LEVEL_LOW,
	.gpio51 = GPIO_LEVEL_LOW,
	.gpio52 = GPIO_LEVEL_LOW,
	.gpio53 = GPIO_LEVEL_LOW,
	.gpio54 = GPIO_LEVEL_LOW,
	.gpio55 = GPIO_LEVEL_LOW,
	.gpio56 = GPIO_LEVEL_LOW,
	.gpio57 = GPIO_LEVEL_LOW,
	.gpio58 = GPIO_LEVEL_LOW,
	.gpio59 = GPIO_LEVEL_LOW,
	.gpio60 = GPIO_LEVEL_LOW,
	.gpio61 = GPIO_LEVEL_LOW,
	.gpio62 = GPIO_LEVEL_LOW,
	.gpio63 = GPIO_LEVEL_LOW,
};

const struct pch_gpio_set3 pch_gpio_set3_mode = {
	.gpio64 = GPIO_MODE_NONE,	/* NOT USED / CLK_FLEX0 */
	.gpio65 = GPIO_MODE_NONE,	/* NOT USED / CLK_FLEX1 */
	.gpio66 = GPIO_MODE_NONE,	/* NOT USED / CLK_FLEX2 */
	.gpio67 = GPIO_MODE_NONE,	/* NOT USED / CLK_FLEX3 */
	.gpio68 = GPIO_MODE_NONE,	/* NOT USED / FAN TACK4 */
	.gpio69 = GPIO_MODE_GPIO,	/* REC_MODE_L / FAN TACK5 */
	.gpio70 = GPIO_MODE_GPIO,	/* SPI_WP1#_RPCH / FAN TACK7 */
	.gpio71 = GPIO_MODE_GPIO,	/* LVDS/eDP / FAN TACK8 */
	.gpio72 = GPIO_MODE_NONE,	/* NOT USED / BATLOW# */
	.gpio73 = GPIO_MODE_NONE,	/* NOT USED / PCIECLKRQ0#*/
	.gpio74 = GPIO_MODE_NONE,	/* NOT USED / SML1ALERT# /PCHHOT# */
	.gpio75 = GPIO_MODE_NATIVE,	/* SML1DATA */
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
	.gpio73 = GPIO_DIR_INPUT,
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

const struct pch_gpio_map parrot_gpio_map = {
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
