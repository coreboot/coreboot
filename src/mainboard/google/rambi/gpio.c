/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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

#include <stdlib.h>
#include <baytrail/gpio.h>

/* SCORE GPIOs */
static const struct soc_gpio_map gpscore_gpio_map[] = {
	GPIO_DEFAULT,	/* GPIO 0 */
	GPIO_DEFAULT,	/* GPIO 1 */
	GPIO_DEFAULT,	/* GPIO 2 */
	GPIO_DEFAULT,	/* GPIO 3 */
	GPIO_DEFAULT,	/* GPIO 4 */
	GPIO_DEFAULT,	/* GPIO 5 */
	GPIO_DEFAULT,	/* GPIO 6 */
	GPIO_DEFAULT,	/* GPIO 7 */
	GPIO_DEFAULT,	/* GPIO 8 */
	GPIO_DEFAULT,	/* GPIO 9 */
	GPIO_DEFAULT,	/* GPIO 10 */
	GPIO_DEFAULT,	/* GPIO 11 */
	GPIO_DEFAULT,	/* GPIO 12 */
	GPIO_DEFAULT,	/* GPIO 13 */
	GPIO_DEFAULT,	/* GPIO 14 */
	GPIO_DEFAULT,	/* GPIO 15 */
	GPIO_DEFAULT,	/* GPIO 16 */
	GPIO_DEFAULT,	/* GPIO 17 */
	GPIO_DEFAULT,	/* GPIO 18 */
	GPIO_DEFAULT,	/* GPIO 19 */
	GPIO_DEFAULT,	/* GPIO 20 */
	GPIO_DEFAULT,	/* GPIO 21 */
	GPIO_DEFAULT,	/* GPIO 22 */
	GPIO_DEFAULT,	/* GPIO 23 */
	GPIO_DEFAULT,	/* GPIO 24 */
	GPIO_DEFAULT,	/* GPIO 25 */
	GPIO_DEFAULT,	/* GPIO 26 */
	GPIO_DEFAULT,	/* GPIO 27 */
	GPIO_DEFAULT,	/* GPIO 28 */
	GPIO_DEFAULT,	/* GPIO 29 */
	GPIO_DEFAULT,	/* GPIO 30 */
	GPIO_DEFAULT,	/* GPIO 31 */
	GPIO_DEFAULT,	/* GPIO 32 */
	GPIO_DEFAULT,	/* GPIO 33 */
	GPIO_DEFAULT,	/* GPIO 34 */
	GPIO_DEFAULT,	/* GPIO 35 */
	GPIO_DEFAULT,	/* GPIO 36 */
	GPIO_DEFAULT,	/* GPIO 37 */
	GPIO_DEFAULT,	/* GPIO 38 */
	GPIO_DEFAULT,	/* GPIO 39 */
	GPIO_DEFAULT,	/* GPIO 40 */
	GPIO_DEFAULT,	/* GPIO 41 */
	GPIO_DEFAULT,	/* GPIO 42 */
	GPIO_DEFAULT,	/* GPIO 43 */
	GPIO_DEFAULT,	/* GPIO 44 */
	GPIO_DEFAULT,	/* GPIO 45 */
	GPIO_DEFAULT,	/* GPIO 46 */
	GPIO_DEFAULT,	/* GPIO 47 */
	GPIO_DEFAULT,	/* GPIO 48 */
	GPIO_DEFAULT,	/* GPIO 49 */
	GPIO_DEFAULT,	/* GPIO 50 */
	GPIO_FUNC1,	/* GPIO 51 - SMBus DATA */
	GPIO_FUNC1,	/* GPIO 52 - SMBus CLK */
	GPIO_DEFAULT,	/* GPIO 53 */
	GPIO_DEFAULT,	/* GPIO 54 */
	GPIO_DEFAULT,	/* GPIO 55 */
	GPIO_DEFAULT,	/* GPIO 56 */
	GPIO_FUNC1,	/* GPIO 57 - COM1 TXD */
	GPIO_DEFAULT,	/* GPIO 58 */
	GPIO_DEFAULT,	/* GPIO 59 */
	GPIO_DEFAULT,	/* GPIO 60 */
	GPIO_FUNC1,	/* GPIO 61 - COM1 RXD */
	GPIO_DEFAULT,	/* GPIO 62 */
	GPIO_DEFAULT,	/* GPIO 63 */
	GPIO_DEFAULT,	/* GPIO 64 */
	GPIO_DEFAULT,	/* GPIO 65 */
	GPIO_DEFAULT,	/* GPIO 66 */
	GPIO_DEFAULT,	/* GPIO 67 */
	GPIO_DEFAULT,	/* GPIO 68 */
	GPIO_DEFAULT,	/* GPIO 69 */
	GPIO_DEFAULT,	/* GPIO 70 */
	GPIO_DEFAULT,	/* GPIO 71 */
	GPIO_DEFAULT,	/* GPIO 72 */
	GPIO_DEFAULT,	/* GPIO 73 */
	GPIO_DEFAULT,	/* GPIO 74 */
	GPIO_DEFAULT,	/* GPIO 75 */
	GPIO_DEFAULT,	/* GPIO 76 */
	GPIO_DEFAULT,	/* GPIO 77 */
	GPIO_DEFAULT,	/* GPIO 78 */
	GPIO_DEFAULT,	/* GPIO 79 */
	GPIO_DEFAULT,	/* GPIO 80 */
	GPIO_DEFAULT,	/* GPIO 81 */
	GPIO_DEFAULT,	/* GPIO 82 */
	GPIO_DEFAULT,	/* GPIO 83 */
	GPIO_DEFAULT,	/* GPIO 84 */
	GPIO_DEFAULT,	/* GPIO 85 */
	GPIO_DEFAULT,	/* GPIO 86 */
	GPIO_DEFAULT,	/* GPIO 87 */
	GPIO_DEFAULT,	/* GPIO 88 */
	GPIO_DEFAULT,	/* GPIO 89 */
	GPIO_DEFAULT,	/* GPIO 90 */
	GPIO_DEFAULT,	/* GPIO 91 */
	GPIO_DEFAULT,	/* GPIO 92 */
	GPIO_DEFAULT,	/* GPIO 93 */
	GPIO_DEFAULT,	/* GPIO 94 */
	GPIO_DEFAULT,	/* GPIO 95 */
	GPIO_DEFAULT,	/* GPIO 96 */
	GPIO_DEFAULT,	/* GPIO 97 */
	GPIO_DEFAULT,	/* GPIO 98 */
	GPIO_DEFAULT,	/* GPIO 99 */
	GPIO_DEFAULT,	/* GPIO 100 */
	GPIO_DEFAULT,	/* GPIO 101 */
	GPIO_END
};

/* SSUS GPIOs */
static const struct soc_gpio_map gpssus_gpio_map[] = {
	GPIO_DEFAULT,	/* GPIO 0 */
	GPIO_DEFAULT,	/* GPIO 1 */
	GPIO_DEFAULT,	/* GPIO 2 */
	GPIO_DEFAULT,	/* GPIO 3 */
	GPIO_DEFAULT,	/* GPIO 4 */
	GPIO_DEFAULT,	/* GPIO 5 */
	GPIO_DEFAULT,	/* GPIO 6 */
	GPIO_DEFAULT,	/* GPIO 7 */
	GPIO_DEFAULT,	/* GPIO 8 */
	GPIO_DEFAULT,	/* GPIO 9 */
	GPIO_DEFAULT,	/* GPIO 10 */
	GPIO_DEFAULT,	/* GPIO 11 */
	GPIO_DEFAULT,	/* GPIO 12 */
	GPIO_DEFAULT,	/* GPIO 13 */
	GPIO_DEFAULT,	/* GPIO 14 */
	GPIO_DEFAULT,	/* GPIO 15 */
	GPIO_DEFAULT,	/* GPIO 16 */
	GPIO_DEFAULT,	/* GPIO 17 */
	GPIO_DEFAULT,	/* GPIO 18 */
	GPIO_DEFAULT,	/* GPIO 19 */
	GPIO_DEFAULT,	/* GPIO 20 */
	GPIO_DEFAULT,	/* GPIO 21 */
	GPIO_DEFAULT,	/* GPIO 22 */
	GPIO_DEFAULT,	/* GPIO 23 */
	GPIO_DEFAULT,	/* GPIO 24 */
	GPIO_DEFAULT,	/* GPIO 25 */
	GPIO_DEFAULT,	/* GPIO 26 */
	GPIO_DEFAULT,	/* GPIO 27 */
	GPIO_DEFAULT,	/* GPIO 28 */
	GPIO_DEFAULT,	/* GPIO 29 */
	GPIO_DEFAULT,	/* GPIO 30 */
	GPIO_DEFAULT,	/* GPIO 31 */
	GPIO_DEFAULT,	/* GPIO 32 */
	GPIO_DEFAULT,	/* GPIO 33 */
	GPIO_DEFAULT,	/* GPIO 34 */
	GPIO_DEFAULT,	/* GPIO 35 */
	GPIO_DEFAULT,	/* GPIO 36 */
	GPIO_DEFAULT,	/* GPIO 37 */
	GPIO_DEFAULT,	/* GPIO 38 */
	GPIO_DEFAULT,	/* GPIO 39 */
	GPIO_DEFAULT,	/* GPIO 40 */
	GPIO_DEFAULT,	/* GPIO 41 */
	GPIO_DEFAULT,	/* GPIO 42 */
	GPIO_DEFAULT,	/* GPIO 43 */
	GPIO_END
};

static struct soc_gpio_config gpio_config = {
	.ncore = NULL,
	.score = gpscore_gpio_map,
	.ssus  = gpssus_gpio_map
};

struct soc_gpio_config* mainboard_get_gpios(void)
{
	return &gpio_config;
}
