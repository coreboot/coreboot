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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#ifndef INTEL_RANGELEY_GPIO_H
#define INTEL_RANGELEY_GPIO_H

#define GPIO_MODE_NATIVE	0
#define GPIO_MODE_GPIO		1
#define GPIO_MODE_NONE		1

#define GPIO_DIR_OUTPUT		0
#define GPIO_DIR_INPUT		1

#define GPIO_LEVEL_LOW		0
#define GPIO_LEVEL_HIGH		1

#define GPIO_TPE_DISABLE		0
#define GPIO_TPE_ENABLE		1

#define GPIO_TNE_DISABLE		0
#define GPIO_TNE_ENABLE		1

#define GPIO_TS_DISABLE		0
#define GPIO_TS_ENABLE		1

#define GPIO_WE_DISABLE		0
#define GPIO_WE_ENABLE		1

struct soc_gpio {
	u32 gpio0 : 1;
	u32 gpio1 : 1;
	u32 gpio2 : 1;
	u32 gpio3 : 1;
	u32 gpio4 : 1;
	u32 gpio5 : 1;
	u32 gpio6 : 1;
	u32 gpio7 : 1;
	u32 gpio8 : 1;
	u32 gpio9 : 1;
	u32 gpio10 : 1;
	u32 gpio11 : 1;
	u32 gpio12 : 1;
	u32 gpio13 : 1;
	u32 gpio14 : 1;
	u32 gpio15 : 1;
	u32 gpio16 : 1;
	u32 gpio17 : 1;
	u32 gpio18 : 1;
	u32 gpio19 : 1;
	u32 gpio20 : 1;
	u32 gpio21 : 1;
	u32 gpio22 : 1;
	u32 gpio23 : 1;
	u32 gpio24 : 1;
	u32 gpio25 : 1;
	u32 gpio26 : 1;
	u32 gpio27 : 1;
	u32 gpio28 : 1;
	u32 gpio29 : 1;
	u32 gpio30 : 1;
	u32 gpio31 : 1;
} __attribute__ ((packed));

struct soc_cfio {
	u32 pad_conf_0;
	u32 pad_conf_1;
	u32 pad_val;
	u32 pad_dft;
} __attribute__ ((packed));

struct soc_gpio_map {
	/* GPIO core */
	struct {
		const struct soc_gpio *mode;
		const struct soc_gpio *direction;
		const struct soc_gpio *level;
		const struct soc_gpio *tpe;
		const struct soc_gpio *tne;
		const struct soc_gpio *ts;
		const struct soc_cfio *cfio_init;
		const u32 cfio_entrynum;
	}core;

	/* GPIO SUS */
	struct {
		const struct soc_gpio *mode;
		const struct soc_gpio *direction;
		const struct soc_gpio *level;
		const struct soc_gpio *tpe;
		const struct soc_gpio *tne;
		const struct soc_gpio *ts;
		const struct soc_gpio *we;
		const struct soc_cfio *cfio_init;
		const u32 cfio_entrynum;
	}sus;


};

/* Configure GPIOs with mainboard provided settings */
void setup_soc_gpios(const struct soc_gpio_map *gpio);

/* Get GPIO pin value */
int get_gpio(int gpio_num);
/*
 * Get a number comprised of multiple GPIO values. gpio_num_array points to
 * the array of GPIO pin numbers to scan, terminated by -1.
 */
unsigned get_gpios(const int *gpio_num_array);

#endif
