/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 secunet Security Networks AG
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

#ifndef KTQM77_GPIO_H
#define KTQM77_GPIO_H

#include "southbridge/intel/bd82x6x/gpio.h"

/*
 * TODO: Investigate somehow... Current values are taken from a running
 *       system with vendor supplied firmware.
 */

const struct pch_gpio_set1 pch_gpio_set1_mode = {
	.gpio0  = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio1  = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio2  = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio3  = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio4  = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio5  = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio6  = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio7  = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio8  = GPIO_MODE_GPIO,   /* Unknown Output LOW*/
	.gpio9  = GPIO_MODE_NATIVE, /* Native - OC5# pin */
	.gpio10 = GPIO_MODE_NATIVE, /* Native - OC6# pin */
	.gpio11 = GPIO_MODE_NATIVE, /* Native - SMBALERT# pin */
	.gpio12 = GPIO_MODE_NATIVE, /* Native - LAN_PHY_PWR_CTRL */
	.gpio13 = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio14 = GPIO_MODE_NATIVE, /* Native - OC7# pin */
	.gpio15 = GPIO_MODE_GPIO,   /* Unknown Output LOW */
	.gpio16 = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio17 = GPIO_MODE_GPIO,   /* Unknown Output LOW */
	.gpio18 = GPIO_MODE_NATIVE, /* Native - PCIECLKRQ1# LAN clock pin */
	.gpio19 = GPIO_MODE_GPIO,   /* Unknown Output HIGH */
	.gpio20 = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio21 = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio22 = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio23 = GPIO_MODE_NATIVE, /* Native - LDRQ1# pin */
	.gpio24 = GPIO_MODE_GPIO,   /* Unknown Output HIGH */
	.gpio25 = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio26 = GPIO_MODE_NATIVE, /* Native - PCIECLKRQ4# pin */
	.gpio27 = GPIO_MODE_GPIO,   /* Unknown Input */ /* Vendor supplied DSDT sets this conditionally
							   when going to suspend (S3, S4, S5). */
	.gpio28 = GPIO_MODE_GPIO,   /* Unknown Output HIGH */
	.gpio29 = GPIO_MODE_NATIVE,   /* Native - SLP_LAN# pin, forced by soft strap */
	.gpio30 = GPIO_MODE_NATIVE, /* Native - SUSWARN_EC# pin */
	.gpio31 = GPIO_MODE_NATIVE  /* Native - ACPRESENT */
};

const struct pch_gpio_set1 pch_gpio_set1_direction = {
	.gpio0  = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio1  = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio2  = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio3  = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio4  = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio5  = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio6  = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio7  = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio8  = GPIO_DIR_OUTPUT, /* Unknown Output LOW */
	.gpio9  = GPIO_DIR_INPUT,  /* Native */
	.gpio10 = GPIO_DIR_INPUT,  /* Native */
	.gpio11 = GPIO_DIR_INPUT,  /* Native */
	.gpio12 = GPIO_DIR_INPUT,  /* Native */
	.gpio13 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio14 = GPIO_DIR_INPUT,  /* Native */
	.gpio15 = GPIO_DIR_OUTPUT, /* Unknown Output LOW */
	.gpio16 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio17 = GPIO_DIR_OUTPUT, /* Unknown Output LOW */
	.gpio18 = GPIO_DIR_INPUT,  /* Native */
	.gpio19 = GPIO_DIR_OUTPUT, /* Unknown Output HIGH */
	.gpio20 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio21 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio22 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio23 = GPIO_DIR_INPUT,  /* Native */
	.gpio24 = GPIO_DIR_OUTPUT, /* Unknown Output HIGH */
	.gpio25 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio26 = GPIO_DIR_INPUT,  /* Native */
	.gpio27 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio28 = GPIO_DIR_OUTPUT, /* Unknown Output HIGH */
	.gpio29 = GPIO_DIR_INPUT,  /* Native */
	.gpio30 = GPIO_DIR_INPUT,  /* Native */
	.gpio31 = GPIO_DIR_INPUT,  /* Native */
};

const struct pch_gpio_set1 pch_gpio_set1_level = {
	.gpio0  = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio1  = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio2  = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio3  = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio4  = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio5  = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio6  = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio7  = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio8  = GPIO_LEVEL_LOW,  /* Unknown Output LOW */
	.gpio9  = GPIO_LEVEL_LOW,  /* Native */
	.gpio10 = GPIO_LEVEL_LOW,  /* Native */
	.gpio11 = GPIO_LEVEL_LOW,  /* Native */
	.gpio12 = GPIO_LEVEL_LOW,  /* Native */
	.gpio13 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio14 = GPIO_LEVEL_LOW,  /* Native */
	.gpio15 = GPIO_LEVEL_LOW,  /* Unknown Output LOW */
	.gpio16 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio17 = GPIO_LEVEL_LOW,  /* Unknown Output LOW */
	.gpio18 = GPIO_LEVEL_LOW,  /* Native */
	.gpio19 = GPIO_LEVEL_HIGH, /* Unknown Output HIGH */
	.gpio20 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio21 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio22 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio23 = GPIO_LEVEL_LOW,  /* Native */
	.gpio24 = GPIO_LEVEL_HIGH, /* Unknown Output HIGH */
	.gpio25 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio26 = GPIO_LEVEL_LOW,  /* Native */
	.gpio27 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio28 = GPIO_LEVEL_HIGH, /* Unknown Output HIGH */
	.gpio29 = GPIO_LEVEL_LOW,  /* Native */
	.gpio30 = GPIO_LEVEL_LOW,  /* Native */
	.gpio31 = GPIO_LEVEL_LOW,  /* Native */
};

const struct pch_gpio_set2 pch_gpio_set2_mode = {
	.gpio32 = GPIO_MODE_NATIVE, /* Native - CLKRUN# pin */
	.gpio33 = GPIO_MODE_GPIO,   /* Unknown Output LOW */
	.gpio34 = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio35 = GPIO_MODE_GPIO,   /* Unknown Output HIGH */
	.gpio36 = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio37 = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio38 = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio39 = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio40 = GPIO_MODE_NATIVE, /* Native - OC1# pin */
	.gpio41 = GPIO_MODE_NATIVE, /* Native - OC2# pin */
	.gpio42 = GPIO_MODE_NATIVE, /* Native - OC3# pin */
	.gpio43 = GPIO_MODE_NATIVE, /* Native - OC4# pin */
	.gpio44 = GPIO_MODE_NATIVE, /* Native - PCIECLKRQ5# pin */
	.gpio45 = GPIO_MODE_NATIVE, /* Native - PCIECLKRQ6# pin */
	.gpio46 = GPIO_MODE_NATIVE, /* Native - PCIECLKRQ7# pin */
	.gpio47 = GPIO_MODE_NATIVE, /* Native - PEG_A_CLKRQ# pin */
	.gpio48 = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio49 = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio50 = GPIO_MODE_GPIO,   /* Unknown Output LOW */
	.gpio51 = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio52 = GPIO_MODE_GPIO,   /* Unknown Output HIGH */
	.gpio53 = GPIO_MODE_GPIO,   /* Unknown Output HIGH */
	.gpio54 = GPIO_MODE_GPIO,   /* Unknown Output LOW */
	.gpio55 = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio56 = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio57 = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio58 = GPIO_MODE_NATIVE, /* Native - SML1CLK */
	.gpio59 = GPIO_MODE_NATIVE, /* Native - OC0# pin */
	.gpio60 = GPIO_MODE_GPIO,   /* Unknown Output HIGH */
	.gpio61 = GPIO_MODE_NATIVE, /* Native - SUS_STAT# pin*/
	.gpio62 = GPIO_MODE_NATIVE, /* Native - SUSCLK */
	.gpio63 = GPIO_MODE_NATIVE, /* Native - SLP_S5# */
};

const struct pch_gpio_set2 pch_gpio_set2_direction = {
	.gpio32 = GPIO_DIR_INPUT,  /* Native */
	.gpio33 = GPIO_DIR_OUTPUT, /* Unknown Output LOW */
	.gpio34 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio35 = GPIO_DIR_OUTPUT, /* Unknown Output HIGH */
	.gpio36 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio37 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio38 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio39 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio40 = GPIO_DIR_INPUT,  /* Native */
	.gpio41 = GPIO_DIR_INPUT,  /* Native */
	.gpio42 = GPIO_DIR_INPUT,  /* Native */
	.gpio43 = GPIO_DIR_INPUT,  /* Native */
	.gpio44 = GPIO_DIR_INPUT,  /* Native */
	.gpio45 = GPIO_DIR_INPUT,  /* Native */
	.gpio46 = GPIO_DIR_INPUT,  /* Native */
	.gpio47 = GPIO_DIR_INPUT,  /* Native */
	.gpio48 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio49 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio50 = GPIO_DIR_OUTPUT, /* Unknown Output LOW */
	.gpio51 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio52 = GPIO_DIR_OUTPUT, /* Unknown Output HIGH */
	.gpio53 = GPIO_DIR_OUTPUT, /* Unknown Output HIGH */
	.gpio54 = GPIO_DIR_OUTPUT, /* Unknown Output LOW */
	.gpio55 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio56 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio57 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio58 = GPIO_DIR_INPUT,  /* Native */
	.gpio59 = GPIO_DIR_INPUT,  /* Native */
	.gpio60 = GPIO_DIR_OUTPUT, /* Unknown Output HIGH */
	.gpio61 = GPIO_DIR_INPUT,  /* Native */
	.gpio62 = GPIO_DIR_INPUT,  /* Native */
	.gpio63 = GPIO_DIR_INPUT,  /* Native */
};

const struct pch_gpio_set2 pch_gpio_set2_level = {
	.gpio32 = GPIO_LEVEL_LOW,  /* Native */
	.gpio33 = GPIO_LEVEL_LOW,  /* Unknown Output LOW */
	.gpio34 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio35 = GPIO_LEVEL_HIGH, /* Unknown Output HIGH */
	.gpio36 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio37 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio38 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio39 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio40 = GPIO_LEVEL_LOW,  /* Native */
	.gpio41 = GPIO_LEVEL_LOW,  /* Native */
	.gpio42 = GPIO_LEVEL_LOW,  /* Native */
	.gpio43 = GPIO_LEVEL_LOW,  /* Native */
	.gpio44 = GPIO_LEVEL_LOW,  /* Native */
	.gpio45 = GPIO_LEVEL_LOW,  /* Native */
	.gpio46 = GPIO_LEVEL_LOW,  /* Native */
	.gpio47 = GPIO_LEVEL_LOW,  /* Native */
	.gpio48 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio49 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio50 = GPIO_LEVEL_LOW,  /* Unknown Output LOW */
	.gpio51 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio52 = GPIO_LEVEL_HIGH, /* Unknown Output HIGH */
	.gpio53 = GPIO_LEVEL_HIGH, /* Unknown Output HIGH */
	.gpio54 = GPIO_LEVEL_LOW,  /* Unknown Output LOW */
	.gpio55 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio56 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio57 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio58 = GPIO_LEVEL_LOW,  /* Native */
	.gpio59 = GPIO_LEVEL_LOW,  /* Native */
	.gpio60 = GPIO_LEVEL_HIGH, /* Unknown Output HIGH */
	.gpio61 = GPIO_LEVEL_LOW,  /* Native */
	.gpio62 = GPIO_LEVEL_LOW,  /* Native */
	.gpio63 = GPIO_LEVEL_LOW,  /* Native */
};

const struct pch_gpio_set3 pch_gpio_set3_mode = {
	.gpio64 = GPIO_MODE_GPIO,   /* Unknown Output LOW */
	.gpio65 = GPIO_MODE_GPIO,   /* Unknown Output LOW */
	.gpio66 = GPIO_MODE_GPIO,   /* Unknown Output LOW */
	.gpio67 = GPIO_MODE_NATIVE, /* Native - CLKOUTFLEX3 */
	.gpio68 = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio69 = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio70 = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio71 = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio72 = GPIO_MODE_NATIVE, /* Native - nothing on mobile */
	.gpio73 = GPIO_MODE_NATIVE, /* Native - PCIECLKRQ0# pin */
	.gpio74 = GPIO_MODE_NATIVE, /* Native - SML1ALERT#/PCHHOT# pin */
	.gpio75 = GPIO_MODE_NATIVE, /* Native - SML1DATA */
};

const struct pch_gpio_set3 pch_gpio_set3_direction = {
	.gpio64 = GPIO_DIR_OUTPUT, /* Unknown Output LOW */
	.gpio65 = GPIO_DIR_OUTPUT, /* Unknown Output LOW */
	.gpio66 = GPIO_DIR_OUTPUT, /* Unknown Output LOW */
	.gpio67 = GPIO_DIR_INPUT,  /* Native */
	.gpio68 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio69 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio70 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio71 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio72 = GPIO_DIR_INPUT,  /* Native */
	.gpio73 = GPIO_DIR_INPUT,  /* Native */
	.gpio74 = GPIO_DIR_INPUT,  /* Native */
	.gpio75 = GPIO_DIR_INPUT,  /* Native */
};

const struct pch_gpio_set3 pch_gpio_set3_level = {
	.gpio64 = GPIO_LEVEL_LOW,  /* Unknown Output LOW */
	.gpio65 = GPIO_LEVEL_LOW,  /* Unknown Output LOW */
	.gpio66 = GPIO_LEVEL_LOW,  /* Unknown Output LOW */
	.gpio67 = GPIO_LEVEL_LOW,  /* Native */
	.gpio68 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio69 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio70 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio71 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio72 = GPIO_LEVEL_LOW,  /* Native */
	.gpio73 = GPIO_LEVEL_LOW,  /* Native */
	.gpio74 = GPIO_LEVEL_LOW,  /* Native */
	.gpio75 = GPIO_LEVEL_LOW,  /* Native */
};

const struct pch_gpio_map ktqm77_gpio_map = {
	.set1 = {
		.mode		= &pch_gpio_set1_mode,
		.direction	= &pch_gpio_set1_direction,
		.level		= &pch_gpio_set1_level,
	},
	.set2 = {
		.mode		= &pch_gpio_set2_mode,
		.direction	= &pch_gpio_set2_direction,
		.level		= &pch_gpio_set2_level,
	},
	.set3 = {
		.mode		= &pch_gpio_set3_mode,
		.direction	= &pch_gpio_set3_direction,
		.level		= &pch_gpio_set3_level,
	},
};
#endif
