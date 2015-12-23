/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google Inc.
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

#include <soc/gpio.h>

static const struct gpio_config mainboard_gpio_config[] = {
	PCH_GPIO_INPUT,		/* 0 */
	PCH_GPIO_INPUT,		/* 1 */
	PCH_GPIO_INPUT,		/* 2 */
	PCH_GPIO_INPUT,		/* 3 */
	PCH_GPIO_INPUT,		/* 4 */
	PCH_GPIO_INPUT,		/* 5 */
	PCH_GPIO_INPUT,		/* 6 */
	PCH_GPIO_INPUT,		/* 7 */
	PCH_GPIO_INPUT,		/* 8 */
	PCH_GPIO_INPUT,		/* 9 */
	PCH_GPIO_ACPI_SCI,	/* 10 */
	PCH_GPIO_INPUT,		/* 11 */
	PCH_GPIO_INPUT,		/* 12 */
	PCH_GPIO_INPUT,		/* 13 */
	PCH_GPIO_INPUT,		/* 14 */
	PCH_GPIO_INPUT,		/* 15 */
	PCH_GPIO_INPUT,		/* 16 */
	PCH_GPIO_INPUT,		/* 17 */
	PCH_GPIO_NATIVE,	/* 18 */
	PCH_GPIO_NATIVE,	/* 19 */
	PCH_GPIO_INPUT,		/* 20 */
	PCH_GPIO_INPUT,		/* 21 */
	PCH_GPIO_INPUT,		/* 22 */
	PCH_GPIO_INPUT,		/* 23 */
	PCH_GPIO_INPUT,		/* 24 */
	PCH_GPIO_INPUT,		/* 25 */
	PCH_GPIO_INPUT,		/* 26 */
	PCH_GPIO_INPUT,		/* 27 */
	PCH_GPIO_INPUT,		/* 28 */
	PCH_GPIO_NATIVE,	/* 29 */
	PCH_GPIO_NATIVE,	/* 30 */
	PCH_GPIO_NATIVE,	/* 31 */
	PCH_GPIO_INPUT,		/* 32 */
	PCH_GPIO_INPUT,		/* 33 */
	PCH_GPIO_INPUT,		/* 34 */
	PCH_GPIO_NATIVE,	/* 35 */
	PCH_GPIO_NATIVE,	/* 36 */
	PCH_GPIO_NATIVE,	/* 37 */
	PCH_GPIO_INPUT,		/* 38 */
	PCH_GPIO_NATIVE,	/* 39 */
	PCH_GPIO_NATIVE,	/* 40 */
	PCH_GPIO_INPUT,		/* 41 */
	PCH_GPIO_INPUT,		/* 42 */
	PCH_GPIO_INPUT,		/* 43 */
	PCH_GPIO_INPUT,		/* 44 */
	PCH_GPIO_INPUT,		/* 45 */
	PCH_GPIO_INPUT,		/* 46 */
	PCH_GPIO_INPUT,		/* 47 */
	PCH_GPIO_INPUT,		/* 48 */
	PCH_GPIO_INPUT,		/* 49 */
	PCH_GPIO_INPUT,		/* 50 */
	PCH_GPIO_INPUT,		/* 51 */
	PCH_GPIO_INPUT,		/* 52 */
	PCH_GPIO_INPUT,		/* 53 */
	PCH_GPIO_INPUT,		/* 54 */
	PCH_GPIO_INPUT,		/* 55 */
	PCH_GPIO_INPUT,		/* 56 */
	PCH_GPIO_INPUT,		/* 57 */
	PCH_GPIO_INPUT,		/* 58 */
	PCH_GPIO_INPUT,		/* 59 */
	PCH_GPIO_INPUT,		/* 60 */
	PCH_GPIO_NATIVE,	/* 61 */
	PCH_GPIO_NATIVE,	/* 62 */
	PCH_GPIO_NATIVE,	/* 63 */
	PCH_GPIO_INPUT,		/* 64 */
	PCH_GPIO_INPUT,		/* 65 */
	PCH_GPIO_INPUT,		/* 66 */
	PCH_GPIO_INPUT,		/* 67 */
	PCH_GPIO_INPUT,		/* 68 */
	PCH_GPIO_INPUT,		/* 69 */
	PCH_GPIO_INPUT,		/* 70 */
	PCH_GPIO_NATIVE,	/* 71 */
	PCH_GPIO_NATIVE,	/* 72 */
	PCH_GPIO_INPUT,		/* 73 */
	PCH_GPIO_NATIVE,	/* 74 */
	PCH_GPIO_NATIVE,	/* 75 */
	PCH_GPIO_NATIVE,	/* 76 */
	PCH_GPIO_INPUT,		/* 77 */
	PCH_GPIO_INPUT,		/* 78 */
	PCH_GPIO_INPUT,		/* 79 */
	PCH_GPIO_INPUT,		/* 80 */
	PCH_GPIO_NATIVE,	/* 81 */
	PCH_GPIO_NATIVE,	/* 82 */
	PCH_GPIO_INPUT,		/* 83 */
	PCH_GPIO_INPUT,		/* 84 */
	PCH_GPIO_INPUT,		/* 85 */
	PCH_GPIO_INPUT,		/* 86 */
	PCH_GPIO_INPUT,		/* 87 */
	PCH_GPIO_INPUT,		/* 88 */
	PCH_GPIO_INPUT,		/* 89 */
	PCH_GPIO_INPUT,		/* 90 */
	PCH_GPIO_INPUT,		/* 91 */
	PCH_GPIO_INPUT,		/* 92 */
	PCH_GPIO_INPUT,		/* 93 */
	PCH_GPIO_INPUT,		/* 94 */
	PCH_GPIO_END
};

#endif
