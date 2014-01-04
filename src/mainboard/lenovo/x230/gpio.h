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

#ifndef BUTTERFLY_GPIO_H
#define BUTTERFLY_GPIO_H

#include "southbridge/intel/bd82x6x/gpio.h"

const struct pch_gpio_set1 pch_gpio_set1_mode = {
	.gpio0  = GPIO_MODE_NONE,   /* Unused */
	.gpio1  = GPIO_MODE_NONE,   /* Unused */
	.gpio2  = GPIO_MODE_NONE,   /* Unused */
	.gpio3  = GPIO_MODE_NONE,   /* Unused */
	.gpio4  = GPIO_MODE_NATIVE, /* Native - TPSINT# for TP SMBus IRQ */
	.gpio5  = GPIO_MODE_NONE,   /* Unused */
	.gpio6  = GPIO_MODE_GPIO,   /* Input - BOARD_ID4 */
	.gpio7  = GPIO_MODE_GPIO,   /* Input - BOARD_ID5 */
	.gpio8  = GPIO_MODE_GPIO,   /* Output - BT on/off */
	.gpio9  = GPIO_MODE_NONE,   /* Unused */
	.gpio10 = GPIO_MODE_NONE,   /* Unused */
	.gpio11 = GPIO_MODE_GPIO,   /* Input - TP WAKEUP Event */
	.gpio12 = GPIO_MODE_NONE,   /* Unused */
	.gpio13 = GPIO_MODE_GPIO,   /* Input - SCI from EC */
	.gpio14 = GPIO_MODE_GPIO,   /* Output - AOAC WLAN  power control */
	.gpio15 = GPIO_MODE_GPIO,   /* Unused - Do not control WLAN*/
	.gpio16 = GPIO_MODE_NONE,   /* Unused */
	.gpio17 = GPIO_MODE_GPIO,   /* Input - DGPU_PWROK */
	.gpio18 = GPIO_MODE_NATIVE, /* Native - PCIECLKRQ1# LAN clock pin*/
	.gpio19 = GPIO_MODE_GPIO,   /* Input - Boot BIOS  Selection 0 */
	.gpio20 = GPIO_MODE_NATIVE, /* Native - PCIECLKRQ2# SDCard clock pin */
	.gpio21 = GPIO_MODE_GPIO,   /* Input - EC_ENTERING_RW for Google OS */
	.gpio22 = GPIO_MODE_GPIO,   /* Input - BIOS RECOVERY */
	.gpio23 = GPIO_MODE_NONE,   /* Unused */
	.gpio24 = GPIO_MODE_GPIO,   /* Output - DGPU_HOLD_RST# */
	.gpio25 = GPIO_MODE_NONE,   /* Unused */
	.gpio26 = GPIO_MODE_NONE,   /* Unused */
	.gpio27 = GPIO_MODE_NONE,   /* Unused */
	.gpio28 = GPIO_MODE_NONE,   /* Unused */
	.gpio29 = GPIO_MODE_NONE,   /* Unused */
	.gpio30 = GPIO_MODE_NATIVE, /* Native - SUSWARN_EC# */
	.gpio31 = GPIO_MODE_NONE,   /* Unused */
};

const struct pch_gpio_set1 pch_gpio_set1_direction = {
	.gpio0  = GPIO_DIR_INPUT,  /* Unused */
	.gpio1  = GPIO_DIR_INPUT,  /* Unused */
	.gpio2  = GPIO_DIR_INPUT,  /* Unused */
	.gpio3  = GPIO_DIR_INPUT,  /* Unused */
	.gpio4  = GPIO_DIR_INPUT,  /* Native */
	.gpio5  = GPIO_DIR_INPUT,  /* Unused */
	.gpio6  = GPIO_DIR_INPUT,  /* Input */
	.gpio7  = GPIO_DIR_INPUT,  /* Input */
	.gpio8  = GPIO_DIR_INPUT,  /* Output HIGH - set in mainboard.c */
	.gpio9  = GPIO_DIR_INPUT,  /* Unused */
	.gpio10 = GPIO_DIR_INPUT,  /* Unused */
	.gpio11 = GPIO_DIR_INPUT,  /* Input */
	.gpio12 = GPIO_DIR_INPUT,  /* Unused */
	.gpio13 = GPIO_DIR_INPUT,  /* Input */
	.gpio14 = GPIO_DIR_OUTPUT, /* Output HIGH */
	.gpio15 = GPIO_DIR_INPUT,  /* Unused */
	.gpio16 = GPIO_DIR_INPUT,  /* Unused */
	.gpio17 = GPIO_DIR_INPUT,  /* Input */
	.gpio18 = GPIO_DIR_INPUT,  /* Native */
	.gpio19 = GPIO_DIR_INPUT,  /* Input */
	.gpio20 = GPIO_DIR_INPUT,  /* Native */
	.gpio21 = GPIO_DIR_INPUT,  /* Input */
	.gpio22 = GPIO_DIR_INPUT,  /* Input */
	.gpio23 = GPIO_DIR_INPUT,  /* Unused */
	.gpio24 = GPIO_DIR_OUTPUT, /* Output HIGH */
	.gpio25 = GPIO_DIR_INPUT,  /* Unused */
	.gpio26 = GPIO_DIR_INPUT,  /* Unused */
	.gpio27 = GPIO_DIR_INPUT,  /* Unused */
	.gpio28 = GPIO_DIR_INPUT,  /* Unused */
	.gpio29 = GPIO_DIR_INPUT,  /* Unused */
	.gpio30 = GPIO_DIR_INPUT,  /* Native */
	.gpio31 = GPIO_DIR_INPUT,  /* Unused */
};

const struct pch_gpio_set1 pch_gpio_set1_level = {
	.gpio0  = GPIO_LEVEL_LOW,  /* Unused */
	.gpio1  = GPIO_LEVEL_LOW,  /* Unused */
	.gpio2  = GPIO_LEVEL_LOW,  /* Unused */
	.gpio3  = GPIO_LEVEL_LOW,  /* Unused */
	.gpio4  = GPIO_LEVEL_LOW,  /* Native */
	.gpio5  = GPIO_LEVEL_LOW,  /* Unused */
	.gpio6  = GPIO_LEVEL_LOW,  /* Input */
	.gpio7  = GPIO_LEVEL_LOW,  /* Input */
	.gpio8  = GPIO_LEVEL_HIGH, /* Output HIGH - set in mainboard.c */
	.gpio9  = GPIO_LEVEL_LOW,  /* Unused */
	.gpio10 = GPIO_LEVEL_LOW,  /* Unused */
	.gpio11 = GPIO_LEVEL_LOW,  /* Input */
	.gpio12 = GPIO_LEVEL_LOW,  /* Unused */
	.gpio13 = GPIO_LEVEL_LOW,  /* Input */
	.gpio14 = GPIO_LEVEL_HIGH, /* Output HIGH */
	.gpio15 = GPIO_LEVEL_HIGH, /* Unused */
	.gpio16 = GPIO_LEVEL_LOW,  /* Unused */
	.gpio17 = GPIO_LEVEL_LOW,  /* Input */
	.gpio18 = GPIO_LEVEL_LOW,  /* Native */
	.gpio19 = GPIO_LEVEL_LOW,  /* Input */
	.gpio20 = GPIO_LEVEL_LOW,  /* Native */
	.gpio21 = GPIO_LEVEL_LOW,  /* Input */
	.gpio22 = GPIO_LEVEL_LOW,  /* Input */
	.gpio23 = GPIO_LEVEL_LOW,  /* Unused */
	.gpio24 = GPIO_LEVEL_HIGH, /* Output HIGH */
	.gpio25 = GPIO_LEVEL_LOW,  /* Unused */
	.gpio26 = GPIO_LEVEL_LOW,  /* Unused */
	.gpio27 = GPIO_LEVEL_LOW,  /* Unused */
	.gpio28 = GPIO_LEVEL_LOW,  /* Unused */
	.gpio29 = GPIO_LEVEL_LOW,  /* Unused */
	.gpio30 = GPIO_LEVEL_LOW,  /* Native */
	.gpio31 = GPIO_LEVEL_LOW,  /* Unused */
};

const struct pch_gpio_set1 pch_gpio_set1_invert = {
	.gpio11 = GPIO_INVERT,	/* invert touchpad wakeup pin */
	.gpio13 = GPIO_INVERT,	/* invert EC SCI pin */
};

const struct pch_gpio_set2 pch_gpio_set2_mode = {
	.gpio32 = GPIO_MODE_NATIVE, /* Native - Connect to EC Clock Run */
	.gpio33 = GPIO_MODE_GPIO,   /* Input - (Google protect BIOS ROM) */
	.gpio34 = GPIO_MODE_NONE,   /* Unused */
	.gpio35 = GPIO_MODE_NONE,   /* Unused */
	.gpio36 = GPIO_MODE_GPIO,   /* Output - DGPU_PWR_EN */
	.gpio37 = GPIO_MODE_GPIO,   /* Input - FDI TERM / VOLTAGE OVERRIDE */
	.gpio38 = GPIO_MODE_GPIO,   /* Input - MFG_MODE test */
	.gpio39 = GPIO_MODE_GPIO,   /* Input - DGPU_PRSNT */
	.gpio40 = GPIO_MODE_NONE,   /* Unused */
	.gpio41 = GPIO_MODE_NONE,   /* Unused */
	.gpio42 = GPIO_MODE_NONE,   /* Unused */
	.gpio43 = GPIO_MODE_NONE,   /* Unused */
	.gpio44 = GPIO_MODE_GPIO,   /* Input - BOARD_ID0 */
	.gpio45 = GPIO_MODE_GPIO,   /* Input - BOARD_ID1 */
	.gpio46 = GPIO_MODE_GPIO,   /* Input - BOARD_ID2 */
	.gpio47 = GPIO_MODE_NATIVE, /* Native - PEGA_GPU clock request */
	.gpio48 = GPIO_MODE_NONE,   /* Unused */
	.gpio49 = GPIO_MODE_NONE,   /* Unused */
	.gpio50 = GPIO_MODE_NONE,   /* Unused */
	.gpio51 = GPIO_MODE_GPIO,   /* Input - Boot BIOS  Selection 1 */
	.gpio52 = GPIO_MODE_GPIO,   /* Input - Google recovery, Pull up +3V */
	.gpio53 = GPIO_MODE_GPIO,   /* Output - G Sensor LED */
	.gpio54 = GPIO_MODE_GPIO,   /* Input - Google Development */
	.gpio55 = GPIO_MODE_GPIO,   /* Input - Top-Block Swap Override */
	.gpio56 = GPIO_MODE_NONE,   /* Unused */
	.gpio57 = GPIO_MODE_GPIO,   /* Input - SV_DET */
	.gpio58 = GPIO_MODE_NONE,   /* Unused */
	.gpio59 = GPIO_MODE_NONE,   /* Unused */
	.gpio60 = GPIO_MODE_NONE,   /* GPO - DRAMRST_CNTRL_PCH */
	.gpio61 = GPIO_MODE_NONE,   /* Unused */
	.gpio62 = GPIO_MODE_NATIVE, /* Native - Connect to EC 32.768KHz */
	.gpio63 = GPIO_MODE_NATIVE, /* Native - SLP_S5 */
};

const struct pch_gpio_set2 pch_gpio_set2_direction = {
	.gpio32 = GPIO_DIR_INPUT,  /* Native */
	.gpio33 = GPIO_DIR_INPUT,  /* Input */
	.gpio34 = GPIO_DIR_INPUT,  /* Unused */
	.gpio35 = GPIO_DIR_INPUT,  /* Unused */
	.gpio36 = GPIO_DIR_OUTPUT, /* Output HIGH */
	.gpio37 = GPIO_DIR_INPUT,  /* Input */
	.gpio38 = GPIO_DIR_INPUT,  /* Input */
	.gpio39 = GPIO_DIR_INPUT,  /* Input */
	.gpio40 = GPIO_DIR_INPUT,  /* Unused */
	.gpio41 = GPIO_DIR_INPUT,  /* Unused */
	.gpio42 = GPIO_DIR_INPUT,  /* Unused */
	.gpio43 = GPIO_DIR_INPUT,  /* Unused */
	.gpio44 = GPIO_DIR_INPUT,  /* Input */
	.gpio45 = GPIO_DIR_INPUT,  /* Input */
	.gpio46 = GPIO_DIR_INPUT,  /* Input */
	.gpio47 = GPIO_DIR_INPUT,  /* Native */
	.gpio48 = GPIO_DIR_INPUT,  /* Unused */
	.gpio49 = GPIO_DIR_INPUT,  /* Unused */
	.gpio50 = GPIO_DIR_INPUT,  /* Unused */
	.gpio51 = GPIO_DIR_INPUT,  /* Input */
	.gpio52 = GPIO_DIR_INPUT,  /* Input */
	.gpio53 = GPIO_DIR_OUTPUT, /* Input */
	.gpio54 = GPIO_DIR_INPUT,  /* Input */
	.gpio55 = GPIO_DIR_INPUT,  /* Input */
	.gpio56 = GPIO_DIR_INPUT,  /* Unused */
	.gpio57 = GPIO_DIR_INPUT,  /* Input */
	.gpio58 = GPIO_DIR_INPUT,  /* Unused */
	.gpio59 = GPIO_DIR_INPUT,  /* Unused */
	.gpio60 = GPIO_DIR_OUTPUT, /* Output HIGH */
	.gpio61 = GPIO_DIR_INPUT,  /* Unused */
	.gpio62 = GPIO_DIR_INPUT,  /* Native */
	.gpio63 = GPIO_DIR_INPUT,  /* Native */
};

const struct pch_gpio_set2 pch_gpio_set2_level = {
	.gpio32 = GPIO_LEVEL_LOW,  /* Native */
	.gpio33 = GPIO_LEVEL_LOW,  /* Input */
	.gpio34 = GPIO_LEVEL_LOW,  /* Unused */
	.gpio35 = GPIO_LEVEL_LOW,  /* Unused */
	.gpio36 = GPIO_LEVEL_HIGH, /* Output HIGH */
	.gpio37 = GPIO_LEVEL_LOW,  /* Input */
	.gpio38 = GPIO_LEVEL_LOW,  /* Input */
	.gpio39 = GPIO_LEVEL_LOW,  /* Input */
	.gpio40 = GPIO_LEVEL_LOW,  /* Unused */
	.gpio41 = GPIO_LEVEL_LOW,  /* Unused */
	.gpio42 = GPIO_LEVEL_LOW,  /* Unused */
	.gpio43 = GPIO_LEVEL_LOW,  /* Unused */
	.gpio44 = GPIO_LEVEL_LOW,  /* Input */
	.gpio45 = GPIO_LEVEL_LOW,  /* Input */
	.gpio46 = GPIO_LEVEL_LOW,  /* Input */
	.gpio47 = GPIO_LEVEL_LOW,  /* Native */
	.gpio48 = GPIO_LEVEL_LOW,  /* Unused */
	.gpio49 = GPIO_LEVEL_LOW,  /* Unused */
	.gpio50 = GPIO_LEVEL_LOW,  /* Unused */
	.gpio51 = GPIO_LEVEL_LOW,  /* Input */
	.gpio52 = GPIO_LEVEL_LOW,  /* Input */
	.gpio53 = GPIO_LEVEL_HIGH, /* Input */
	.gpio54 = GPIO_LEVEL_LOW,  /* Input */
	.gpio55 = GPIO_LEVEL_LOW,  /* Input */
	.gpio56 = GPIO_LEVEL_LOW,  /* Unused */
	.gpio57 = GPIO_LEVEL_LOW,  /* Input */
	.gpio58 = GPIO_LEVEL_LOW,  /* Unused */
	.gpio59 = GPIO_LEVEL_LOW,  /* Unused */
	.gpio60 = GPIO_LEVEL_HIGH, /* Output HIGH */
	.gpio61 = GPIO_LEVEL_LOW,  /* Unused */
	.gpio62 = GPIO_LEVEL_LOW,  /* Native */
	.gpio63 = GPIO_LEVEL_LOW,  /* Native */
};

const struct pch_gpio_set3 pch_gpio_set3_mode = {
	.gpio64 = GPIO_MODE_NONE,   /* Unused */
	.gpio65 = GPIO_MODE_NONE,   /* Unused */
	.gpio66 = GPIO_MODE_NONE,   /* Unused */
	.gpio67 = GPIO_MODE_NONE,   /* Unused */
	.gpio68 = GPIO_MODE_GPIO,   /* Input - DGPU_PWR_EN */
	.gpio69 = GPIO_MODE_NONE,   /* Unused */
	.gpio70 = GPIO_MODE_NONE,   /* Unused */
	.gpio71 = GPIO_MODE_NONE,   /* Unused */
	.gpio72 = GPIO_MODE_NONE,   /* Unused */
	.gpio73 = GPIO_MODE_NATIVE, /* Native - PCIECLKRQ0# WLAN clock request */
	.gpio74 = GPIO_MODE_NONE,   /* Unused */
	.gpio75 = GPIO_MODE_GPIO,   /* Input - SMB_ME1_DAT */
};

const struct pch_gpio_set3 pch_gpio_set3_direction = {
	.gpio64 = GPIO_DIR_INPUT,   /* Unused */
	.gpio65 = GPIO_DIR_INPUT,   /* Unused */
	.gpio66 = GPIO_DIR_INPUT,   /* Unused */
	.gpio67 = GPIO_DIR_INPUT,   /* Unused */
	.gpio68 = GPIO_DIR_INPUT,   /* Input */
	.gpio69 = GPIO_DIR_INPUT,   /* Unused */
	.gpio70 = GPIO_DIR_INPUT,   /* Unused */
	.gpio71 = GPIO_DIR_INPUT,   /* Unused */
	.gpio72 = GPIO_DIR_INPUT,   /* Unused */
	.gpio73 = GPIO_DIR_INPUT,   /* Native */
	.gpio74 = GPIO_DIR_INPUT,   /* Unused */
	.gpio75 = GPIO_DIR_INPUT,   /* Input */
};

const struct pch_gpio_set3 pch_gpio_set3_level = {
	.gpio64 = GPIO_LEVEL_LOW,   /* Unused */
	.gpio65 = GPIO_LEVEL_LOW,   /* Unused */
	.gpio66 = GPIO_LEVEL_LOW,   /* Unused */
	.gpio67 = GPIO_LEVEL_LOW,   /* Unused */
	.gpio68 = GPIO_LEVEL_LOW,   /* Input */
	.gpio69 = GPIO_LEVEL_LOW,   /* Unused */
	.gpio70 = GPIO_LEVEL_LOW,   /* Unused */
	.gpio71 = GPIO_LEVEL_LOW,   /* Unused */
	.gpio72 = GPIO_LEVEL_LOW,   /* Unused */
	.gpio73 = GPIO_LEVEL_LOW,   /* Native */
	.gpio74 = GPIO_LEVEL_LOW,   /* Unused */
	.gpio75 = GPIO_LEVEL_LOW,   /* Input */
};

const struct pch_gpio_map butterfly_gpio_map = {
	.set1 = {
		.mode		= &pch_gpio_set1_mode,
		.direction	= &pch_gpio_set1_direction,
		.level		= &pch_gpio_set1_level,
		.invert		= &pch_gpio_set1_invert,

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
