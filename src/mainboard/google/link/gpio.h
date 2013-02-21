/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Google Inc.
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

#ifndef LINK_GPIO_H
#define LINK_GPIO_H

#include "southbridge/intel/bd82x6x/gpio.h"

const struct pch_gpio_set1 pch_gpio_set1_mode = {
	.gpio0 = GPIO_MODE_GPIO,  /* NMI_DBG# */
	.gpio3 = GPIO_MODE_GPIO,  /* ALS_INT# */
	.gpio5 = GPIO_MODE_GPIO,  /* SIM_DET */
	.gpio7 = GPIO_MODE_GPIO,  /* EC_SCI# */
	.gpio8 = GPIO_MODE_GPIO,  /* EC_SMI# */
	.gpio9 = GPIO_MODE_GPIO,  /* RECOVERY# */
	.gpio10 = GPIO_MODE_GPIO, /* SPD vector D3 */
	.gpio11 = GPIO_MODE_GPIO, /* smbalert#, let's keep it initialized */
	.gpio12 = GPIO_MODE_GPIO, /* TP_INT# */
	.gpio14 = GPIO_MODE_GPIO, /* Touch_INT_L */
	.gpio15 = GPIO_MODE_GPIO, /* EC_LID_OUT# (EC_WAKE#) */
	.gpio21 = GPIO_MODE_GPIO, /* EC_IN_RW */
	.gpio24 = GPIO_MODE_GPIO, /* DDR3L_EN */
	.gpio28 = GPIO_MODE_GPIO, /* SLP_ME_CSW_DEV# */
};

const struct pch_gpio_set1 pch_gpio_set1_direction = {
	.gpio0 = GPIO_DIR_INPUT,
	.gpio3 = GPIO_DIR_INPUT,
	.gpio5 = GPIO_DIR_INPUT,
	.gpio7 = GPIO_DIR_INPUT,
	.gpio8 = GPIO_DIR_INPUT,
	.gpio9 = GPIO_DIR_INPUT,
	.gpio10 = GPIO_DIR_INPUT,
	.gpio11 = GPIO_DIR_INPUT,
	.gpio12 = GPIO_DIR_INPUT,
	.gpio14 = GPIO_DIR_INPUT,
	.gpio15 = GPIO_DIR_INPUT,
	.gpio21 = GPIO_DIR_INPUT,
	.gpio24 = GPIO_DIR_OUTPUT,
	.gpio28 = GPIO_DIR_INPUT,
};

const struct pch_gpio_set1 pch_gpio_set1_level = {
	.gpio1 = GPIO_LEVEL_HIGH,
	.gpio6 = GPIO_LEVEL_HIGH,
	.gpio24 = GPIO_LEVEL_LOW,
};

const struct pch_gpio_set1 pch_gpio_set1_invert = {
	.gpio7 = GPIO_INVERT,
	.gpio8 = GPIO_INVERT,
	.gpio12 = GPIO_INVERT,
	.gpio14 = GPIO_INVERT,
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
	.gpio36 = GPIO_DIR_OUTPUT,
	.gpio41 = GPIO_DIR_INPUT,
	.gpio42 = GPIO_DIR_INPUT,
	.gpio43 = GPIO_DIR_INPUT,
	.gpio57 = GPIO_DIR_INPUT,
	.gpio60 = GPIO_DIR_OUTPUT,
};

const struct pch_gpio_set2 pch_gpio_set2_level = {
	.gpio36 = GPIO_LEVEL_HIGH,
	.gpio60 = GPIO_LEVEL_HIGH,
};

const struct pch_gpio_set3 pch_gpio_set3_mode = {
};

const struct pch_gpio_set3 pch_gpio_set3_direction = {
};

const struct pch_gpio_set3 pch_gpio_set3_level = {
};

const struct pch_gpio_map link_gpio_map = {
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
