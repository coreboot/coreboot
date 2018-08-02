/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Tristan Corrick <tristan@corrick.kiwi>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <southbridge/intel/common/gpio.h>

static const struct pch_gpio_set1 pch_gpio_set1_mode = {
	.gpio0  = GPIO_MODE_GPIO,
	.gpio1  = GPIO_MODE_GPIO,
	.gpio6  = GPIO_MODE_GPIO,
	.gpio7  = GPIO_MODE_GPIO,
	.gpio8  = GPIO_MODE_GPIO,
	.gpio12 = GPIO_MODE_GPIO,
	.gpio13 = GPIO_MODE_GPIO,
	.gpio15 = GPIO_MODE_GPIO,
	.gpio16 = GPIO_MODE_GPIO,
	.gpio17 = GPIO_MODE_GPIO,
	.gpio24 = GPIO_MODE_GPIO,
	.gpio27 = GPIO_MODE_GPIO,
	.gpio28 = GPIO_MODE_GPIO,
	.gpio29 = GPIO_MODE_GPIO,
	.gpio31 = GPIO_MODE_GPIO,
};

static const struct pch_gpio_set1 pch_gpio_set1_direction = {
	.gpio0  = GPIO_DIR_OUTPUT,
	.gpio1  = GPIO_DIR_INPUT,
	.gpio6  = GPIO_DIR_INPUT,
	.gpio7  = GPIO_DIR_INPUT,
	.gpio8  = GPIO_DIR_INPUT,
	.gpio12 = GPIO_DIR_INPUT,
	.gpio13 = GPIO_DIR_INPUT,
	.gpio15 = GPIO_DIR_OUTPUT,
	.gpio16 = GPIO_DIR_INPUT,
	.gpio17 = GPIO_DIR_INPUT,
	.gpio24 = GPIO_DIR_OUTPUT,
	.gpio27 = GPIO_DIR_INPUT,
	.gpio28 = GPIO_DIR_OUTPUT,
	.gpio29 = GPIO_DIR_INPUT,
	.gpio31 = GPIO_DIR_OUTPUT,
};

static const struct pch_gpio_set1 pch_gpio_set1_level = {
	.gpio0  = GPIO_LEVEL_LOW,
	.gpio15 = GPIO_LEVEL_LOW,
	.gpio24 = GPIO_LEVEL_LOW,
	.gpio28 = GPIO_LEVEL_LOW,
	.gpio31 = GPIO_LEVEL_LOW,
};

static const struct pch_gpio_set1 pch_gpio_set1_reset = {
};

static const struct pch_gpio_set1 pch_gpio_set1_invert = {
	.gpio1  = GPIO_INVERT,
	.gpio6  = GPIO_INVERT,
	.gpio13 = GPIO_INVERT,
};

static const struct pch_gpio_set1 pch_gpio_set1_blink = {
};

static const struct pch_gpio_set2 pch_gpio_set2_mode = {
	.gpio32 = GPIO_MODE_GPIO,
	.gpio33 = GPIO_MODE_GPIO,
	.gpio34 = GPIO_MODE_GPIO,
	.gpio46 = GPIO_MODE_GPIO,
	.gpio49 = GPIO_MODE_GPIO,
	.gpio57 = GPIO_MODE_GPIO,
};

static const struct pch_gpio_set2 pch_gpio_set2_direction = {
	.gpio32 = GPIO_DIR_OUTPUT,
	.gpio33 = GPIO_DIR_OUTPUT,
	.gpio34 = GPIO_DIR_INPUT,
	.gpio46 = GPIO_DIR_INPUT,
	.gpio49 = GPIO_DIR_INPUT,
	.gpio57 = GPIO_DIR_INPUT,
};

static const struct pch_gpio_set2 pch_gpio_set2_level = {
	.gpio32 = GPIO_LEVEL_HIGH,
	.gpio33 = GPIO_LEVEL_HIGH,
};

static const struct pch_gpio_set2 pch_gpio_set2_reset = {
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
