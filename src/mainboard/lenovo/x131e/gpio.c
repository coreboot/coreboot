/*
 * This file is part of the coreboot project.
 *
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

#include <southbridge/intel/common/gpio.h>

static const struct pch_gpio_set1 pch_gpio_set1_mode = {
	.gpio0 = GPIO_MODE_GPIO,
	.gpio1 = GPIO_MODE_GPIO,
	.gpio2 = GPIO_MODE_GPIO,
	.gpio3 = GPIO_MODE_GPIO,
	.gpio4 = GPIO_MODE_GPIO,
	.gpio5 = GPIO_MODE_GPIO,
	.gpio6 = GPIO_MODE_GPIO,
	.gpio7 = GPIO_MODE_GPIO,
	.gpio8 = GPIO_MODE_GPIO,
	.gpio12 = GPIO_MODE_GPIO,
	.gpio13 = GPIO_MODE_GPIO,
	.gpio15 = GPIO_MODE_GPIO,
	.gpio16 = GPIO_MODE_GPIO,
	.gpio17 = GPIO_MODE_GPIO,
	.gpio19 = GPIO_MODE_GPIO,
	.gpio21 = GPIO_MODE_GPIO,
	.gpio22 = GPIO_MODE_GPIO,
	.gpio23 = GPIO_MODE_GPIO,
	.gpio24 = GPIO_MODE_GPIO,
	.gpio27 = GPIO_MODE_GPIO,
	.gpio28 = GPIO_MODE_GPIO,
	.gpio29 = GPIO_MODE_GPIO,
};

static const struct pch_gpio_set1 pch_gpio_set1_direction = {
	.gpio0 = GPIO_DIR_OUTPUT,
	.gpio1 = GPIO_DIR_INPUT,
	.gpio2 = GPIO_DIR_OUTPUT,
	.gpio3 = GPIO_DIR_OUTPUT,
	.gpio4 = GPIO_DIR_OUTPUT,
	.gpio5 = GPIO_DIR_OUTPUT,
	.gpio6 = GPIO_DIR_INPUT,
	.gpio7 = GPIO_DIR_INPUT,
	.gpio8 = GPIO_DIR_OUTPUT,
	.gpio12 = GPIO_DIR_OUTPUT,
	.gpio13 = GPIO_DIR_OUTPUT,
	.gpio15 = GPIO_DIR_INPUT,
	.gpio16 = GPIO_DIR_OUTPUT,
	.gpio17 = GPIO_DIR_OUTPUT,
	.gpio19 = GPIO_DIR_OUTPUT,
	.gpio21 = GPIO_DIR_OUTPUT,
	.gpio22 = GPIO_DIR_INPUT,
	.gpio23 = GPIO_DIR_OUTPUT,
	.gpio24 = GPIO_DIR_OUTPUT,
	.gpio27 = GPIO_DIR_INPUT,
	.gpio28 = GPIO_DIR_OUTPUT,
	.gpio29 = GPIO_DIR_OUTPUT,
};

static const struct pch_gpio_set1 pch_gpio_set1_level = {
	.gpio0 = GPIO_LEVEL_HIGH,
	.gpio2 = GPIO_LEVEL_HIGH,
	.gpio3 = GPIO_LEVEL_HIGH,
	.gpio4 = GPIO_LEVEL_HIGH,
	.gpio5 = GPIO_LEVEL_HIGH,
	.gpio8 = GPIO_LEVEL_LOW,
	.gpio12 = GPIO_LEVEL_LOW,
	.gpio13 = GPIO_LEVEL_LOW,
	.gpio16 = GPIO_LEVEL_HIGH,
	.gpio17 = GPIO_LEVEL_HIGH,
	.gpio19 = GPIO_LEVEL_LOW,
	.gpio21 = GPIO_LEVEL_LOW,
	.gpio23 = GPIO_LEVEL_LOW,
	.gpio24 = GPIO_LEVEL_HIGH,
	.gpio28 = GPIO_LEVEL_HIGH,
	.gpio29 = GPIO_LEVEL_HIGH,
};

static const struct pch_gpio_set1 pch_gpio_set1_reset = {
	.gpio24 = GPIO_RESET_RSMRST,
};

static const struct pch_gpio_set1 pch_gpio_set1_invert = {
	.gpio1 = GPIO_INVERT,
	.gpio6 = GPIO_INVERT,
};

static const struct pch_gpio_set1 pch_gpio_set1_blink = {
};

static const struct pch_gpio_set2 pch_gpio_set2_mode = {
	.gpio33 = GPIO_MODE_GPIO,
	.gpio34 = GPIO_MODE_GPIO,
	.gpio35 = GPIO_MODE_GPIO,
	.gpio36 = GPIO_MODE_GPIO,
	.gpio37 = GPIO_MODE_GPIO,
	.gpio38 = GPIO_MODE_GPIO,
	.gpio39 = GPIO_MODE_GPIO,
	.gpio40 = GPIO_MODE_GPIO,
	.gpio41 = GPIO_MODE_GPIO,
	.gpio48 = GPIO_MODE_GPIO,
	.gpio49 = GPIO_MODE_GPIO,
	.gpio51 = GPIO_MODE_GPIO,
	.gpio53 = GPIO_MODE_GPIO,
	.gpio54 = GPIO_MODE_GPIO,
	.gpio57 = GPIO_MODE_GPIO,
	.gpio59 = GPIO_MODE_GPIO,
	.gpio60 = GPIO_MODE_GPIO,
	.gpio61 = GPIO_MODE_GPIO,
};

static const struct pch_gpio_set2 pch_gpio_set2_direction = {
	.gpio33 = GPIO_DIR_OUTPUT,
	.gpio34 = GPIO_DIR_OUTPUT,
	.gpio35 = GPIO_DIR_OUTPUT,
	.gpio36 = GPIO_DIR_INPUT,
	.gpio37 = GPIO_DIR_INPUT,
	.gpio38 = GPIO_DIR_INPUT,
	.gpio39 = GPIO_DIR_INPUT,
	.gpio40 = GPIO_DIR_INPUT,
	.gpio41 = GPIO_DIR_INPUT,
	.gpio48 = GPIO_DIR_OUTPUT,
	.gpio49 = GPIO_DIR_INPUT,
	.gpio51 = GPIO_DIR_OUTPUT,
	.gpio53 = GPIO_DIR_OUTPUT,
	.gpio54 = GPIO_DIR_INPUT,
	.gpio57 = GPIO_DIR_INPUT,
	.gpio59 = GPIO_DIR_INPUT,
	.gpio60 = GPIO_DIR_OUTPUT,
	.gpio61 = GPIO_DIR_OUTPUT,
};

static const struct pch_gpio_set2 pch_gpio_set2_level = {
	.gpio33 = GPIO_LEVEL_LOW,
	.gpio34 = GPIO_LEVEL_HIGH,
	.gpio35 = GPIO_LEVEL_HIGH,
	.gpio48 = GPIO_LEVEL_LOW,
	.gpio51 = GPIO_LEVEL_HIGH,
	.gpio53 = GPIO_LEVEL_HIGH,
	.gpio60 = GPIO_LEVEL_HIGH,
	.gpio61 = GPIO_LEVEL_LOW,
};

static const struct pch_gpio_set2 pch_gpio_set2_reset = {
};

static const struct pch_gpio_set3 pch_gpio_set3_mode = {
	.gpio64 = GPIO_MODE_GPIO,
	.gpio65 = GPIO_MODE_GPIO,
	.gpio66 = GPIO_MODE_GPIO,
	.gpio67 = GPIO_MODE_GPIO,
	.gpio68 = GPIO_MODE_GPIO,
	.gpio69 = GPIO_MODE_GPIO,
	.gpio70 = GPIO_MODE_GPIO,
	.gpio71 = GPIO_MODE_GPIO,
	.gpio72 = GPIO_MODE_GPIO,
};

static const struct pch_gpio_set3 pch_gpio_set3_direction = {
	.gpio64 = GPIO_DIR_OUTPUT,
	.gpio65 = GPIO_DIR_OUTPUT,
	.gpio66 = GPIO_DIR_OUTPUT,
	.gpio67 = GPIO_DIR_INPUT,
	.gpio68 = GPIO_DIR_INPUT,
	.gpio69 = GPIO_DIR_OUTPUT,
	.gpio70 = GPIO_DIR_OUTPUT,
	.gpio71 = GPIO_DIR_OUTPUT,
	.gpio72 = GPIO_DIR_OUTPUT,
};

static const struct pch_gpio_set3 pch_gpio_set3_level = {
	.gpio64 = GPIO_LEVEL_HIGH,
	.gpio65 = GPIO_LEVEL_LOW,
	.gpio66 = GPIO_LEVEL_HIGH,
	.gpio69 = GPIO_LEVEL_LOW,
	.gpio70 = GPIO_LEVEL_HIGH,
	.gpio71 = GPIO_LEVEL_HIGH,
	.gpio72 = GPIO_LEVEL_HIGH,
};

static const struct pch_gpio_set3 pch_gpio_set3_reset = {
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
