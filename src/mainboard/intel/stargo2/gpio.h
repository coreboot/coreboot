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

#include <southbridge/intel/fsp_i89xx/gpio.h>

const struct pch_gpio_set1 pch_gpio_set1_mode = {

	/* 1. The following bits are always 1 because they are always
	 * unMultiplexed: 0, 8, 15, 24, 27, and 28.
	 */

	/*
	 * GPIO 17
	 *   This strap along with the fuse setting determines DMI TX
	 *   voltage level and TX/RX termination.
	 *   PU - SNB GLD (J1G7 Open)
	 *   PD - IVB GLD (J1G7 Jumpered)
	 *
	 * GPIO 25 - Output
	 *   Low = 1.35V DDR3
	 *   High = 1.5V DDR3
	 *
	 *   1.35V DDR3 is Forced by J9C2 Jumpered
	 *   1.35V / 1.5V switching by GPIO 25 if J9C2 is open
	 *
	 * GPIO 30 - Core OSC power strap
	 *   Low SFR OUT (INT)		J1J1 Jumpered (Default)
	 *   High VCCP1P0_STBY (EXT)	J1J1 Open
	 */

	.gpio0  = GPIO_MODE_NONE,	/* Unused */
	.gpio1  = GPIO_MODE_NONE,	/* Unused */
	.gpio2  = GPIO_MODE_NONE,	/* Unused */
	.gpio3  = GPIO_MODE_NONE,	/* Unused */
	.gpio4  = GPIO_MODE_NONE,	/* Unused */
	.gpio5  = GPIO_MODE_GPIO,	/* CONN_GBE_GPIO3 - MLR TODO: What does this do? */
	.gpio6  = GPIO_MODE_GPIO,	/* CONN_GBE_GPIO4 - MLR TODO: What does this do? */
	.gpio7  = GPIO_MODE_NONE,	/* Unused */
	.gpio8  = GPIO_MODE_NONE,	/* Unused */
	.gpio9  = GPIO_MODE_NONE,	/* Unused */
	.gpio10 = GPIO_MODE_NONE,	/* Unused */
	.gpio11 = GPIO_MODE_NONE,	/* Unused */
	.gpio12 = GPIO_MODE_NONE,	/* Unused */
	.gpio13 = GPIO_MODE_NONE,	/* Unused */
	.gpio14 = GPIO_MODE_NONE,	/* Unused */
	.gpio15 = GPIO_MODE_GPIO,	/* CONN_GBE_GPIO0_SUS - MLR TODO*/
	.gpio16 = GPIO_MODE_NONE,	/* Unused */
	.gpio17 = GPIO_MODE_GPIO,	/* PCH_TACH0_GP17 Note:- TODO: What register does this?*/
	.gpio18 = GPIO_MODE_NONE,	/* Unused */
	.gpio19 = GPIO_MODE_NONE,	/* Unused */
	.gpio20 = GPIO_MODE_NONE,	/* Unused */
	.gpio21 = GPIO_MODE_NONE,	/* Unused */
	.gpio22 = GPIO_MODE_NONE,	/* Unused */
	.gpio23 = GPIO_MODE_NONE,	/* Unused */
	.gpio24 = GPIO_MODE_NONE,	/* Unused */
	.gpio25 = GPIO_MODE_GPIO,	/* VDD1P5_DDR OUTPUT LEVEL - MLR TODO: Config for this?  Detect memory? */
	.gpio26 = GPIO_MODE_GPIO,	/* SIO_PME_N - MLR TODO: Configure this */
	.gpio27 = GPIO_MODE_GPIO,	/* FP_LED_YLW_N - MLR TODO: Configure this */
	.gpio28 = GPIO_MODE_NONE,	/* Unused */
	.gpio29 = GPIO_MODE_NONE,	/* NA */
	.gpio30 = GPIO_MODE_GPIO,	/* PCH_SUS_PWR_ACK_GP30 - MLR TODO */
	.gpio31 = GPIO_MODE_NONE,	/* Unused */
};

const struct pch_gpio_set1 pch_gpio_set1_direction = {
	.gpio0  = GPIO_DIR_INPUT,	/* Unused */
	.gpio1  = GPIO_DIR_INPUT,	/* Unused */
	.gpio2  = GPIO_DIR_INPUT,	/* Unused */
	.gpio3  = GPIO_DIR_INPUT,	/* Unused */
	.gpio4  = GPIO_DIR_INPUT,	/* Unused */
	.gpio5  = GPIO_DIR_OUTPUT,	/* GPIO   */
	.gpio6  = GPIO_DIR_OUTPUT,	/* GPIO   */
	.gpio7  = GPIO_DIR_INPUT,	/* Unused */
	.gpio8  = GPIO_DIR_INPUT,	/* Unused */
	.gpio9  = GPIO_DIR_INPUT,	/* Unused */
	.gpio10 = GPIO_DIR_INPUT,	/* Unused */
	.gpio11 = GPIO_DIR_INPUT,	/* Unused */
	.gpio12 = GPIO_DIR_INPUT,	/* Unused */
	.gpio13 = GPIO_DIR_INPUT,	/* Unused */
	.gpio14 = GPIO_DIR_INPUT,	/* Unused */
	.gpio15 = GPIO_DIR_OUTPUT,	/* GPIO   */
	.gpio16 = GPIO_DIR_INPUT,	/* Unused */
	.gpio17 = GPIO_DIR_INPUT,	/* GPIO   */
	.gpio18 = GPIO_DIR_INPUT,	/* Unused */
	.gpio19 = GPIO_DIR_INPUT,	/* Unused */
	.gpio20 = GPIO_DIR_INPUT,	/* Unused */
	.gpio21 = GPIO_DIR_INPUT,	/* Unused */
	.gpio22 = GPIO_DIR_INPUT,	/* Unused */
	.gpio23 = GPIO_DIR_INPUT,	/* Unused */
	.gpio24 = GPIO_DIR_INPUT,	/* Unused */
	.gpio25 = GPIO_DIR_OUTPUT,	/* GPIO   */
	.gpio26 = GPIO_DIR_INPUT,	/* GPIO   */
	.gpio27 = GPIO_DIR_OUTPUT,	/* GPIO   */
	.gpio28 = GPIO_DIR_INPUT,	/* Unused */
	.gpio29 = GPIO_DIR_INPUT,	/* NA     */
	.gpio30 = GPIO_DIR_OUTPUT,	/* GPIO   */
	.gpio31 = GPIO_DIR_INPUT,	/* Unused */
};

const struct pch_gpio_set1 pch_gpio_set1_level = {
	.gpio0  = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio1  = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio2  = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio3  = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio4  = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio5  = GPIO_LEVEL_LOW,	/* GPIO Out  */
	.gpio6  = GPIO_LEVEL_LOW,	/* GPIO Out  */
	.gpio7  = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio8  = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio9  = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio10 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio11 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio12 = GPIO_LEVEL_LOW,	/* Unused */
	.gpio13 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio14 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio15 = GPIO_LEVEL_LOW,	/* GPIO Out  */
	.gpio16 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio17 = GPIO_LEVEL_HIGH,	/* GPIO In  */
	.gpio18 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio19 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio20 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio21 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio22 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio23 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio24 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio25 = GPIO_LEVEL_HIGH,	/* GPIO Out */
	.gpio26 = GPIO_LEVEL_HIGH,	/* GPIO In  */
	.gpio27 = GPIO_LEVEL_LOW,	/* GPIO Out */
	.gpio28 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio29 = GPIO_LEVEL_HIGH,	/* NA     */
	.gpio30 = GPIO_LEVEL_LOW,	/* GPIO Out  */
	.gpio31 = GPIO_LEVEL_HIGH,	/* Unused */
};

const struct pch_gpio_set1 pch_gpio_set1_invert = {
};

const struct pch_gpio_set2 pch_gpio_set2_mode = {
	/*
	 * Gpio 46: Bios Recovery strap
	 * High = Normal mode   - J5A3 (Open)
	 * Low  = Recovery Mode - J5A3 (Jumpered)
	 */
	.gpio32 = GPIO_MODE_GPIO,	/* PCH_CLKRUN_N - MLR TODO:*/
	.gpio33 = GPIO_MODE_NONE,	/* Unused (SECURITY OVERRIDE STRAP (J1G4)) */
	.gpio34 = GPIO_MODE_NONE,	/* Unused*/
	.gpio35 = GPIO_MODE_GPIO,	/* CONN_GBE_GPIO2 : MLR TODO*/
	.gpio36 = GPIO_MODE_NONE,	/* Unused */
	.gpio37 = GPIO_MODE_NONE,	/* Unused */
	.gpio38 = GPIO_MODE_GPIO,	/* Dev Kit Board Version high bit */
	.gpio39 = GPIO_MODE_GPIO,	/* Dev Kit Board Version low bit */
	.gpio40 = GPIO_MODE_NATIVE,	/* PCH_GP40_OC_N<1> */
	.gpio41 = GPIO_MODE_NATIVE,	/* PCH_GP41_OC_N<2> */
	.gpio42 = GPIO_MODE_NATIVE,	/* PCH_GP42_OC_N<3> */
	.gpio43 = GPIO_MODE_NONE,	/* Unused */
	.gpio44 = GPIO_MODE_GPIO,	/* CONN_GBE_GPIO1_SUS : MLR TODO */
	.gpio45 = GPIO_MODE_NONE,	/* Unused */
	.gpio46 = GPIO_MODE_GPIO,	/* BIOS RECOVERY STRAP - Note : MLR TODO */
	.gpio47 = GPIO_MODE_NONE,	/* Unused */
	.gpio48 = GPIO_MODE_NONE,	/* Unused */
	.gpio49 = GPIO_MODE_NONE,	/* Unused (TEMP_ALERT# J2H2 test jumper) */
	.gpio50 = GPIO_MODE_NONE,	/* Unused */
	.gpio51 = GPIO_MODE_NONE,	/* Unused */
	.gpio52 = GPIO_MODE_NONE,	/* Unused */
	.gpio53 = GPIO_MODE_NONE,	/* Unused */
	.gpio54 = GPIO_MODE_NONE,	/* Unused */
	.gpio55 = GPIO_MODE_NONE,	/* Unused */
	.gpio56 = GPIO_MODE_GPIO,	/* CONN_GBE_RESET_N */
	.gpio57 = GPIO_MODE_NONE,	/* Unused */
	.gpio58 = GPIO_MODE_NATIVE,	/* PCH_SML1_CLK */
	.gpio59 = GPIO_MODE_NATIVE,	/* PCH_GP59_OC_N<0> */
	.gpio60 = GPIO_MODE_NONE,	/* Unused */
	.gpio61 = GPIO_MODE_NATIVE,	/* PCH_SUS_STAT_N */
	.gpio62 = GPIO_MODE_NATIVE,	/* PCH_SUSCLK */
	.gpio63 = GPIO_MODE_NATIVE,	/* PCH_SLP_S5_N */
};

const struct pch_gpio_set2 pch_gpio_set2_direction = {
	.gpio32 = GPIO_DIR_OUTPUT,	/* GPIO Out  */
	.gpio33 = GPIO_DIR_INPUT,	/* Unused */
	.gpio34 = GPIO_DIR_INPUT,	/* Unused */
	.gpio35 = GPIO_DIR_OUTPUT,	/* GPIO Out  */
	.gpio36 = GPIO_DIR_INPUT,	/* Unused */
	.gpio37 = GPIO_DIR_INPUT,	/* Unused */
	.gpio38 = GPIO_DIR_INPUT,	/* GPIO In  */
	.gpio39 = GPIO_DIR_INPUT,	/* GPIO In  */
	.gpio40 = GPIO_DIR_INPUT,	/* Native */
	.gpio41 = GPIO_DIR_INPUT,	/* Native */
	.gpio42 = GPIO_DIR_INPUT,	/* Native */
	.gpio43 = GPIO_DIR_INPUT,	/* Unused */
	.gpio44 = GPIO_DIR_OUTPUT,	/* GPIO Out  */
	.gpio45 = GPIO_DIR_INPUT,	/* Unused */
	.gpio46 = GPIO_DIR_INPUT,	/* GPIO In  */
	.gpio47 = GPIO_DIR_INPUT,	/* Unused */
	.gpio48 = GPIO_DIR_INPUT,	/* Unused */
	.gpio49 = GPIO_DIR_INPUT,	/* Unused */
	.gpio50 = GPIO_DIR_INPUT,	/* Unused */
	.gpio51 = GPIO_DIR_INPUT,	/* Unused */
	.gpio52 = GPIO_DIR_INPUT,	/* Unused */
	.gpio53 = GPIO_DIR_INPUT,	/* Unused */
	.gpio54 = GPIO_DIR_INPUT,	/* Unused */
	.gpio55 = GPIO_DIR_INPUT,	/* Unused */
	.gpio56 = GPIO_DIR_OUTPUT,	/* GPIO Out  */
	.gpio57 = GPIO_DIR_INPUT,	/* Unused */
	.gpio58 = GPIO_DIR_INPUT,	/* Native */
	.gpio59 = GPIO_DIR_INPUT,	/* Native */
	.gpio60 = GPIO_DIR_INPUT,	/* Unused */
	.gpio61 = GPIO_DIR_INPUT,	/* Native */
	.gpio62 = GPIO_DIR_INPUT,	/* Native */
	.gpio63 = GPIO_DIR_INPUT,	/* Native */
};

const struct pch_gpio_set2 pch_gpio_set2_level = {
	.gpio32 = GPIO_LEVEL_HIGH,	/* GPIO Out  */
	.gpio33 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio34 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio35 = GPIO_LEVEL_LOW,	/* GPIO Out  */
	.gpio36 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio37 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio38 = GPIO_LEVEL_HIGH,	/* GPIO In  */
	.gpio39 = GPIO_LEVEL_HIGH,	/* GPIO In  */
	.gpio40 = GPIO_LEVEL_HIGH,	/* Native */
	.gpio41 = GPIO_LEVEL_HIGH,	/* Native */
	.gpio42 = GPIO_LEVEL_HIGH,	/* Native */
	.gpio43 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio44 = GPIO_LEVEL_LOW,	/* GPIO Out  */
	.gpio45 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio46 = GPIO_LEVEL_HIGH,	/* GPIO In  */
	.gpio47 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio48 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio49 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio50 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio51 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio52 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio53 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio54 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio55 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio56 = GPIO_LEVEL_LOW,	/* GPIO Out  */
	.gpio57 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio58 = GPIO_LEVEL_HIGH,	/* Native */
	.gpio59 = GPIO_LEVEL_HIGH,	/* Native */
	.gpio60 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio61 = GPIO_LEVEL_HIGH,	/* Native */
	.gpio62 = GPIO_LEVEL_HIGH,	/* Native */
	.gpio63 = GPIO_LEVEL_HIGH,	/* Native */
};

const struct pch_gpio_set3 pch_gpio_set3_mode = {
	.gpio72 = GPIO_MODE_NONE,	/* Unused */
	.gpio73 = GPIO_MODE_NONE,	/* Unused */
	.gpio74 = GPIO_MODE_NATIVE,	/* PCH_SML1ALERT */
	.gpio75 = GPIO_MODE_NATIVE,	/* PCH_SML1_DAT */
};

const struct pch_gpio_set3 pch_gpio_set3_direction = {
	.gpio72 = GPIO_DIR_INPUT,	/* Unused */
	.gpio73 = GPIO_DIR_INPUT,	/* Unused */
	.gpio74 = GPIO_DIR_INPUT,	/* Native */
	.gpio75 = GPIO_DIR_INPUT,	/* Native */
};

const struct pch_gpio_set3 pch_gpio_set3_level = {
	.gpio72 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio73 = GPIO_LEVEL_HIGH,	/* Unused */
	.gpio74 = GPIO_LEVEL_HIGH,	/* Native */
	.gpio75 = GPIO_LEVEL_HIGH,	/* Native */
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
