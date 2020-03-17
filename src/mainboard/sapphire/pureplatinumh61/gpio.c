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

#include <southbridge/intel/common/gpio.h>

static const struct pch_gpio_set1 pch_gpio_set1_mode = {
	.gpio6 = GPIO_MODE_GPIO,
	.gpio7 = GPIO_MODE_GPIO,
	.gpio8 = GPIO_MODE_GPIO,
	.gpio9 = GPIO_MODE_GPIO,
	.gpio10 = GPIO_MODE_GPIO,
	.gpio12 = GPIO_MODE_GPIO,
	.gpio13 = GPIO_MODE_GPIO,
	.gpio14 = GPIO_MODE_GPIO,
	.gpio15 = GPIO_MODE_GPIO,
	.gpio22 = GPIO_MODE_GPIO,
	.gpio23 = GPIO_MODE_GPIO,
	.gpio24 = GPIO_MODE_GPIO,
	.gpio25 = GPIO_MODE_GPIO,
	.gpio26 = GPIO_MODE_GPIO,
	.gpio27 = GPIO_MODE_GPIO,
	.gpio28 = GPIO_MODE_GPIO,
	.gpio29 = GPIO_MODE_GPIO,
};

static const struct pch_gpio_set1 pch_gpio_set1_direction = {
	.gpio6 = GPIO_DIR_OUTPUT,
	.gpio7 = GPIO_DIR_INPUT,
	.gpio8 = GPIO_DIR_INPUT,
	.gpio9 = GPIO_DIR_OUTPUT,
	.gpio10 = GPIO_DIR_INPUT,
	.gpio12 = GPIO_DIR_INPUT,
	.gpio13 = GPIO_DIR_INPUT,
	.gpio14 = GPIO_DIR_INPUT,
	.gpio15 = GPIO_DIR_OUTPUT,
	.gpio22 = GPIO_DIR_OUTPUT,
	.gpio23 = GPIO_DIR_INPUT,
	.gpio24 = GPIO_DIR_OUTPUT,
	.gpio25 = GPIO_DIR_OUTPUT,
	.gpio26 = GPIO_DIR_OUTPUT,
	.gpio27 = GPIO_DIR_OUTPUT,
	.gpio28 = GPIO_DIR_INPUT,
	.gpio29 = GPIO_DIR_INPUT,
};

static const struct pch_gpio_set1 pch_gpio_set1_level = {
	.gpio6 = GPIO_LEVEL_LOW,
	.gpio9 = GPIO_LEVEL_LOW,
	.gpio15 = GPIO_LEVEL_LOW,
	.gpio22 = GPIO_LEVEL_HIGH,
	.gpio24 = GPIO_LEVEL_HIGH,
	.gpio25 = GPIO_LEVEL_LOW,
	.gpio26 = GPIO_LEVEL_LOW,
	.gpio27 = GPIO_LEVEL_LOW,
};

static const struct pch_gpio_set1 pch_gpio_set1_blink = {
	.gpio6 = GPIO_NO_BLINK,
	.gpio9 = GPIO_NO_BLINK,
	.gpio15 = GPIO_NO_BLINK,
	.gpio22 = GPIO_NO_BLINK,
	.gpio24 = GPIO_NO_BLINK,
	.gpio25 = GPIO_NO_BLINK,
	.gpio26 = GPIO_NO_BLINK,
	.gpio27 = GPIO_NO_BLINK,
};

static const struct pch_gpio_set1 pch_gpio_set1_invert = {
	.gpio7 = GPIO_INVERT,
	.gpio8 = GPIO_INVERT,
	.gpio10 = GPIO_NO_INVERT,
	.gpio12 = GPIO_NO_INVERT,
	.gpio13 = GPIO_INVERT,
	.gpio14 = GPIO_NO_INVERT,
	.gpio28 = GPIO_NO_INVERT,
	.gpio29 = GPIO_NO_INVERT,
};

static const struct pch_gpio_set1 pch_gpio_set1_reset = {
	.gpio6 = GPIO_RESET_PWROK,
	.gpio7 = GPIO_RESET_PWROK,
	.gpio8 = GPIO_RESET_PWROK,
	.gpio9 = GPIO_RESET_PWROK,
	.gpio10 = GPIO_RESET_PWROK,
	.gpio12 = GPIO_RESET_PWROK,
	.gpio13 = GPIO_RESET_PWROK,
	.gpio14 = GPIO_RESET_PWROK,
	.gpio15 = GPIO_RESET_PWROK,
	.gpio22 = GPIO_RESET_PWROK,
	.gpio23 = GPIO_RESET_PWROK,
	.gpio24 = GPIO_RESET_PWROK,
	.gpio25 = GPIO_RESET_PWROK,
	.gpio26 = GPIO_RESET_PWROK,
	.gpio27 = GPIO_RESET_PWROK,
	.gpio28 = GPIO_RESET_PWROK,
	.gpio29 = GPIO_RESET_PWROK,
};

static const struct pch_gpio_set2 pch_gpio_set2_mode = {
	.gpio35 = GPIO_MODE_GPIO,
	.gpio37 = GPIO_MODE_GPIO,
	.gpio38 = GPIO_MODE_GPIO,
	.gpio39 = GPIO_MODE_GPIO,
	.gpio48 = GPIO_MODE_GPIO,
	.gpio60 = GPIO_MODE_GPIO,
};

static const struct pch_gpio_set2 pch_gpio_set2_direction = {
	.gpio35 = GPIO_DIR_OUTPUT,
	.gpio37 = GPIO_DIR_INPUT,
	.gpio38 = GPIO_DIR_OUTPUT,
	.gpio39 = GPIO_DIR_OUTPUT,
	.gpio48 = GPIO_DIR_OUTPUT,
	.gpio60 = GPIO_DIR_OUTPUT,
};

static const struct pch_gpio_set2 pch_gpio_set2_level = {
	.gpio35 = GPIO_LEVEL_LOW,
	.gpio38 = GPIO_LEVEL_LOW,
	.gpio39 = GPIO_LEVEL_LOW,
	.gpio48 = GPIO_LEVEL_HIGH,
	.gpio60 = GPIO_LEVEL_HIGH,
};

static const struct pch_gpio_set2 pch_gpio_set2_reset = {
	.gpio35 = GPIO_RESET_PWROK,
	.gpio37 = GPIO_RESET_PWROK,
	.gpio38 = GPIO_RESET_PWROK,
	.gpio39 = GPIO_RESET_PWROK,
	.gpio48 = GPIO_RESET_PWROK,
	.gpio60 = GPIO_RESET_PWROK,
};

static const struct pch_gpio_set3 pch_gpio_set3_mode = {
	.gpio68 = GPIO_MODE_GPIO,
	.gpio69 = GPIO_MODE_GPIO,
	.gpio72 = GPIO_MODE_GPIO,
};

static const struct pch_gpio_set3 pch_gpio_set3_direction = {
	.gpio68 = GPIO_DIR_INPUT,
	.gpio69 = GPIO_DIR_INPUT,
	.gpio72 = GPIO_DIR_INPUT,
};

static const struct pch_gpio_set3 pch_gpio_set3_level = {
};

static const struct pch_gpio_set3 pch_gpio_set3_reset = {
	.gpio68 = GPIO_RESET_PWROK,
	.gpio69 = GPIO_RESET_PWROK,
	.gpio72 = GPIO_RESET_PWROK,
};

const struct pch_gpio_map mainboard_gpio_map = {
	.set1 = {
		.mode		= &pch_gpio_set1_mode,
		.direction	= &pch_gpio_set1_direction,
		.level		= &pch_gpio_set1_level,
		.blink		= &pch_gpio_set1_blink,
		.invert		= &pch_gpio_set1_invert,
		.reset		= &pch_gpio_set1_reset,
	},
	.set2 = {
		.mode		= &pch_gpio_set2_mode,
		.direction	= &pch_gpio_set2_direction,
		.level		= &pch_gpio_set2_level,
		.reset		= &pch_gpio_set2_reset,
	},
	.set3 = {
		.mode		= &pch_gpio_set3_mode,
		.direction	= &pch_gpio_set3_direction,
		.level		= &pch_gpio_set3_level,
		.reset		= &pch_gpio_set3_reset,
	},
};
