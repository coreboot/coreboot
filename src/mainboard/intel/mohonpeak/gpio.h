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
 * Foundation, Inc.
 */

#ifndef MOHONPEAK_GPIO_H
#define MOHONPEAK_GPIO_H

#include <southbridge/intel/fsp_rangeley/gpio.h>

/* Core GPIO */
const struct soc_gpio soc_gpio_mode = {
	.gpio15 = GPIO_MODE_GPIO, /* Board ID GPIO */
	.gpio17 = GPIO_MODE_GPIO, /* Board ID GPIO */
};

const struct soc_gpio soc_gpio_direction = {
	.gpio15 = GPIO_DIR_INPUT, /* Board ID GPIO */
	.gpio17 = GPIO_DIR_INPUT, /* Board ID GPIO */
};

const struct soc_gpio soc_gpio_level = {
};

const struct soc_gpio soc_gpio_tpe = {
};

const struct soc_gpio soc_gpio_tne = {
};

const struct soc_gpio soc_gpio_ts = {
};

/* Keep the CFIO struct in register order, not gpio order. */
const struct soc_cfio soc_cfio_core[] = {
	{ 0x8000, 0x0000, 0x0004, 0x040c },  /* CFIO gpios_28 */
	{ 0x8000, 0x0000, 0x0002, 0x040c },  /* CFIO gpios_27 */
	{ 0x8500, 0x0000, 0x0002, 0x040c },  /* CFIO gpios_26 */
	{ 0x8480, 0x0000, 0x0002, 0x040c },  /* CFIO gpios_21 */
	{ 0x8480, 0x0000, 0x0002, 0x040c },  /* CFIO gpios_22 */
	{ 0x8480, 0x0000, 0x0002, 0x040c },  /* CFIO gpios_23 */
	{ 0x8000, 0x0000, 0x0004, 0x040c },  /* CFIO gpios_25 */
	{ 0x8480, 0x0000, 0x0002, 0x040c },  /* CFIO gpios_24 */
	{ 0x80c028, 0x30003, 0x0004, 0x040c },  /* CFIO gpios_19 */
	{ 0x80c028, 0x30003, 0x0004, 0x040c },  /* CFIO gpios_20 */
	{ 0xc028, 0x30003, 0x0004, 0x040c },  /* CFIO gpios_18 */
	{ 0x04a9, 0x30003, 0x0002, 0x040c },  /* CFIO gpios_17 */
	{ 0x80c028, 0x30003, 0x0004, 0x040c },  /* CFIO gpios_7 */
	{ 0xc028, 0x30003, 0x0004, 0x040c },  /* CFIO gpios_4 */
	{ 0xc028, 0x30003, 0x0004, 0x040c },  /* CFIO gpios_5 */
	{ 0xc528, 0x30003, 0x0002, 0x040c },  /* CFIO gpios_6 */
	{ 0xc028, 0x30003, 0x0004, 0x040c },  /* CFIO gpios_1 */
	{ 0xc028, 0x20002, 0x0004, 0x040c },  /* CFIO gpios_2 */
	{ 0xc028, 0x20002, 0x0004, 0x040c },  /* CFIO gpios_3 */
	{ 0xc528, 0x30003, 0x0002, 0x040c },  /* CFIO gpios_0 */
	{ 0x0000, 0x0000, 0x0000, 0x0000 },  /* CFIO Reserved */
	{ 0xc4a8, 0x30003, 0x0002, 0x040c },  /* CFIO gpios_10 */
	{ 0xc4a8, 0x30003, 0x0002, 0x040c },  /* CFIO gpios_13 */
	{ 0xc4a8, 0x30003, 0x0000, 0x040c },  /* CFIO gpios_14 */
	{ 0xc4a8, 0x30003, 0x0002, 0x040c },  /* CFIO gpios_11 */
	{ 0xc4a8, 0x30003, 0x0000, 0x040c },  /* CFIO gpios_8 */
	{ 0xc4a8, 0x30003, 0x0002, 0x040c },  /* CFIO gpios_9 */
	{ 0xc4a8, 0x30003, 0x0000, 0x040c },  /* CFIO gpios_12 */
	{ 0xc4a8, 0x30003, 0x0002, 0x040c },  /* CFIO gpios_29 */
	{ 0xc4a8, 0x30003, 0x0002, 0x040c },  /* CFIO gpios_30 */
	{ 0x04a9, 0x30003, 0x0002, 0x040c },  /* CFIO gpios_15 */
	{ 0xc028, 0x30003, 0x0004, 0x040c },  /* CFIO gpios_16 */
};

/* SUS GPIO */
const struct soc_gpio soc_gpio_sus_mode  = {
	.gpio2 = GPIO_MODE_GPIO,
};

const struct soc_gpio soc_gpio_sus_direction = {
	.gpio2 = GPIO_DIR_INPUT,
};

const struct soc_gpio soc_gpio_sus_level = {
};

const struct soc_gpio soc_gpio_sus_tpe = {
};

const struct soc_gpio soc_gpio_sus_tne = {
};

const struct soc_gpio soc_gpio_sus_ts = {
};

const struct soc_gpio soc_gpio_sus_we = {
};


/* Keep the CFIO struct in register order, not gpio order. */
const struct soc_cfio soc_cfio_sus[] = {
	{ 0xc028, 0x30003, 0x0004, 0x040c },  /* CFIO SUS gpios_21 */
	{ 0xc028, 0x30003, 0x0004, 0x040c },  /* CFIO SUS gpios_20 */
	{ 0xc028, 0x30003, 0x0004, 0x040c },  /* CFIO SUS gpios_19 */
	{ 0xc028, 0x30003, 0x0004, 0x040c },  /* CFIO SUS gpios_22 */
	{ 0xc4a8, 0x30003, 0x0002, 0x040c },  /* CFIO SUS gpios_17 */
	{ 0xc4a8, 0x30003, 0x0002, 0x040c },  /* CFIO SUS gpios_18 */
	{ 0x0000, 0x0000, 0x0000, 0x0000 },  /* CFIO Reserved */
	{ 0x0000, 0x0000, 0x0000, 0x0000 },  /* CFIO Reserved */
	{ 0x0000, 0x0000, 0x0000, 0x0000 },  /* CFIO Reserved */
	{ 0xc4a8, 0x30003, 0x0002, 0x040c },  /* CFIO SUS gpios_14 */
	{ 0xc028, 0x30003, 0x0004, 0x040c },  /* CFIO SUS gpios_13 */
	{ 0xc028, 0x30003, 0x0004, 0x040c },  /* CFIO SUS gpios_15 */
	{ 0xc028, 0x30003, 0x0004, 0x040c },  /* CFIO SUS gpios_16 */
	{ 0xc4a8, 0x30003, 0x0002, 0x040c },  /* CFIO SUS gpios_25 */
	{ 0xc4a8, 0x30003, 0x0002, 0x040c },  /* CFIO SUS gpios_24 */
	{ 0xc4a8, 0x30003, 0x0002, 0x040c },  /* CFIO SUS gpios_26 */
	{ 0xc4a8, 0x30003, 0x0002, 0x040c },  /* CFIO SUS gpios_27 */
	{ 0xc4a8, 0x30003, 0x0002, 0x040c },  /* CFIO SUS gpios_23 */
	{ 0xc4a8, 0x30003, 0x0003, 0x040c },  /* CFIO SUS gpios_2 */
	{ 0xc4a8, 0x30003, 0x0003, 0x040c },  /* CFIO SUS gpios_1 */
	{ 0x8050, 0x0000, 0x0004, 0x040c },  /* CFIO SUS gpios_7 */
	{ 0x0000, 0x0000, 0x0000, 0x0000 },  /* CFIO Reserved */
	{ 0x0000, 0x0000, 0x0000, 0x0000 },  /* CFIO Reserved */
	{ 0xc028, 0x30003, 0x0004, 0x040c },  /* CFIO SUS gpios_3 */
	{ 0xc4a8, 0x30003, 0x0003, 0x040c },  /* CFIO SUS gpios_0 */
	{ 0x0000, 0x0000, 0x0000, 0x0000 },  /* CFIO Reserved */
	{ 0x0000, 0x0000, 0x0000, 0x0000 },  /* CFIO Reserved */
	{ 0x8000, 0x0000, 0x0004, 0x040c },  /* CFIO SUS gpios_12 */
	{ 0x8050, 0x0000, 0x0004, 0x040c },  /* CFIO SUS gpios_6 */
	{ 0xc028, 0x30003, 0x0004, 0x040c },  /* CFIO SUS gpios_10 */
	{ 0xc4a8, 0x30003, 0x0002, 0x040c },  /* CFIO SUS gpios_9 */
	{ 0xc4a8, 0x30003, 0x0002, 0x040c },  /* CFIO SUS gpios_8 */
	{ 0x0000, 0x0000, 0x0000, 0x0000 },  /* CFIO Reserved */
	{ 0x0000, 0x0000, 0x0000, 0x0000 },  /* CFIO Reserved */
	{ 0x8050, 0x0000, 0x0004, 0x040c },  /* CFIO SUS gpios_4 */
	{ 0xc4a8, 0x30003, 0x0002, 0x040c },  /* CFIO SUS gpios_11 */
	{ 0x0000, 0x0000, 0x0000, 0x0000 },  /* CFIO Reserved */
	{ 0x0000, 0x0000, 0x0000, 0x0000 },  /* CFIO Reserved */
	{ 0x0000, 0x0000, 0x0000, 0x0000 },  /* CFIO Reserved */
	{ 0x0000, 0x0000, 0x0000, 0x0000 },  /* CFIO Reserved */
	{ 0xc028, 0x30003, 0x0004, 0x040c },  /* CFIO SUS gpios_5 */
};

const struct soc_gpio_map gpio_map = {
	.core = {
		.mode      = &soc_gpio_mode,
		.direction = &soc_gpio_direction,
		.level     = &soc_gpio_level,
		.tpe       = &soc_gpio_tpe,
		.tne       = &soc_gpio_tne,
		.ts        = &soc_gpio_ts,
		.cfio_init = &soc_cfio_core[0],
		.cfio_entrynum = sizeof(soc_cfio_core) / sizeof(struct soc_cfio),
	},
	.sus = {
		.mode      = &soc_gpio_sus_mode,
		.direction = &soc_gpio_sus_direction,
		.level     = &soc_gpio_sus_level,
		.tpe       = &soc_gpio_sus_tpe,
		.tne       = &soc_gpio_sus_tne,
		.ts        = &soc_gpio_sus_ts,
		.we        = &soc_gpio_sus_we,
		.cfio_init = &soc_cfio_sus[0],
		.cfio_entrynum = sizeof(soc_cfio_sus) / sizeof(struct soc_cfio),
	},
};

#endif /* MOHONPEAK_GPIO_H */
