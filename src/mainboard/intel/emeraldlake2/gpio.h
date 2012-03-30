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

#ifndef EMERALDLAKE2_GPIO_H
#define EMERALDLAKE2_GPIO_H

#include "southbridge/intel/bd82x6x/gpio.h"

const struct pch_gpio_set1 pch_gpio_set1_mode = {
        .gpio0 = GPIO_MODE_GPIO,
        .gpio1 = GPIO_MODE_GPIO,
        .gpio3 = GPIO_MODE_GPIO,
        .gpio5 = GPIO_MODE_GPIO,
        .gpio6 = GPIO_MODE_GPIO,
        .gpio7 = GPIO_MODE_GPIO,
        .gpio8 = GPIO_MODE_GPIO,
        .gpio9 = GPIO_MODE_GPIO,
        .gpio12 = GPIO_MODE_GPIO,
        .gpio15 = GPIO_MODE_GPIO,
        .gpio21 = GPIO_MODE_GPIO,
	.gpio22 = GPIO_MODE_GPIO,
        .gpio24 = GPIO_MODE_GPIO,
        .gpio27 = GPIO_MODE_GPIO,
        .gpio28 = GPIO_MODE_GPIO,
};

const struct pch_gpio_set1 pch_gpio_set1_direction = {
        .gpio0 = GPIO_DIR_INPUT,
        .gpio3 = GPIO_DIR_INPUT,
        .gpio5 = GPIO_DIR_INPUT,
        .gpio7 = GPIO_DIR_INPUT,
        .gpio8 = GPIO_DIR_INPUT,
        .gpio9 = GPIO_DIR_INPUT,
        .gpio12 = GPIO_DIR_INPUT,
        .gpio15 = GPIO_DIR_INPUT,
        .gpio21 = GPIO_DIR_INPUT,
	.gpio22 = GPIO_DIR_INPUT,
        .gpio27 = GPIO_DIR_INPUT,
};

const struct pch_gpio_set1 pch_gpio_set1_level = {
};

const struct pch_gpio_set1 pch_gpio_set1_invert = {
};

const struct pch_gpio_set2 pch_gpio_set2_mode = {
        .gpio36 = GPIO_MODE_GPIO,
	.gpio48 = GPIO_MODE_GPIO,
        .gpio57 = GPIO_MODE_GPIO,
        .gpio60 = GPIO_MODE_GPIO,
};

const struct pch_gpio_set2 pch_gpio_set2_direction = {
	.gpio48 = GPIO_DIR_INPUT,
        .gpio57 = GPIO_DIR_INPUT,
};

const struct pch_gpio_set2 pch_gpio_set2_level = {
};

const struct pch_gpio_set3 pch_gpio_set3_mode = {
};

const struct pch_gpio_set3 pch_gpio_set3_direction = {
};

const struct pch_gpio_set3 pch_gpio_set3_level = {
};

const struct pch_gpio_map emeraldlake2_gpio_map = {
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
