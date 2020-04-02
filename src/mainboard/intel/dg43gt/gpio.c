/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <southbridge/intel/common/gpio.h>

static const struct pch_gpio_set1 pch_gpio_set1_mode = {
	.gpio7 = GPIO_MODE_GPIO,
	.gpio8 = GPIO_MODE_GPIO,
	.gpio11 = GPIO_MODE_GPIO,
	.gpio12 = GPIO_MODE_GPIO,
	.gpio13 = GPIO_MODE_GPIO,
	.gpio18 = GPIO_MODE_GPIO,
	.gpio19 = GPIO_MODE_GPIO,
	.gpio20 = GPIO_MODE_GPIO,
	.gpio22 = GPIO_MODE_GPIO,
	.gpio27 = GPIO_MODE_GPIO,
	.gpio28 = GPIO_MODE_GPIO,
};

static const struct pch_gpio_set1 pch_gpio_set1_direction = {
	.gpio7 = GPIO_DIR_INPUT,
	.gpio8 = GPIO_DIR_OUTPUT,
	.gpio11 = GPIO_DIR_OUTPUT,
	.gpio12 = GPIO_DIR_OUTPUT,
	.gpio13 = GPIO_DIR_INPUT,
	.gpio18 = GPIO_DIR_INPUT,
	.gpio19 = GPIO_DIR_INPUT,
	.gpio20 = GPIO_DIR_OUTPUT,
	.gpio22 = GPIO_DIR_INPUT,
	.gpio27 = GPIO_DIR_OUTPUT,
	.gpio28 = GPIO_DIR_OUTPUT,
};

static const struct pch_gpio_set1 pch_gpio_set1_level = {
	.gpio8 = GPIO_LEVEL_LOW,
	.gpio11 = GPIO_LEVEL_LOW,
	.gpio12 = GPIO_LEVEL_HIGH,
	.gpio20 = GPIO_LEVEL_HIGH,
	.gpio27 = GPIO_LEVEL_HIGH,
	.gpio28 = GPIO_LEVEL_LOW,
};

static const struct pch_gpio_set1 pch_gpio_set1_invert = {
	.gpio13 = GPIO_INVERT,
};

static const struct pch_gpio_set1 pch_gpio_set1_blink = { };

static const struct pch_gpio_set2 pch_gpio_set2_mode = {
	.gpio32 = GPIO_MODE_GPIO,
	.gpio33 = GPIO_MODE_GPIO,
	.gpio34 = GPIO_MODE_GPIO,
	.gpio36 = GPIO_MODE_GPIO,
	.gpio37 = GPIO_MODE_GPIO,
	.gpio38 = GPIO_MODE_GPIO,
	.gpio39 = GPIO_MODE_GPIO,
	.gpio48 = GPIO_MODE_GPIO,
	.gpio49 = GPIO_MODE_GPIO,
	.gpio56 = GPIO_MODE_GPIO,
	.gpio57 = GPIO_MODE_GPIO,
	.gpio60 = GPIO_MODE_GPIO,
};

static const struct pch_gpio_set2 pch_gpio_set2_direction = {
	.gpio32 = GPIO_DIR_OUTPUT,
	.gpio33 = GPIO_DIR_INPUT,
	.gpio34 = GPIO_DIR_INPUT,
	.gpio36 = GPIO_DIR_OUTPUT,
	.gpio37 = GPIO_DIR_OUTPUT,
	.gpio38 = GPIO_DIR_OUTPUT,
	.gpio39 = GPIO_DIR_INPUT,
	.gpio48 = GPIO_DIR_INPUT,
	.gpio49 = GPIO_DIR_OUTPUT,
	.gpio56 = GPIO_DIR_OUTPUT,
	.gpio57 = GPIO_DIR_INPUT,
	.gpio60 = GPIO_DIR_OUTPUT,
};

static const struct pch_gpio_set2 pch_gpio_set2_level = {
	.gpio32 = GPIO_LEVEL_HIGH,
	.gpio36 = GPIO_LEVEL_LOW,
	.gpio37 = GPIO_LEVEL_HIGH,
	.gpio38 = GPIO_LEVEL_HIGH,
	.gpio49 = GPIO_LEVEL_LOW,
	.gpio56 = GPIO_LEVEL_HIGH,
	.gpio60 = GPIO_LEVEL_HIGH,
};

static const struct pch_gpio_set3 pch_gpio_set3_mode = { };

static const struct pch_gpio_set3 pch_gpio_set3_direction = { };

static const struct pch_gpio_set3 pch_gpio_set3_level = { };

const struct pch_gpio_map mainboard_gpio_map = {
	.set1 = {
		.mode		= &pch_gpio_set1_mode,
		.direction	= &pch_gpio_set1_direction,
		.level		= &pch_gpio_set1_level,
		.blink		= &pch_gpio_set1_blink,
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
