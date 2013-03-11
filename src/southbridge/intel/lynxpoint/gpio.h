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

#ifndef INTEL_LYNXPOINT_GPIO_H
#define INTEL_LYNXPOINT_GPIO_H

/* ICH7 GPIOBASE */
#define GPIO_USE_SEL		0x00
#define GP_IO_SEL		0x04
#define GP_LVL			0x0c
#define GPO_BLINK		0x18
#define GPI_INV			0x2c
#define GPIO_USE_SEL2		0x30
#define GP_IO_SEL2		0x34
#define GP_LVL2			0x38
#define GPIO_USE_SEL3		0x40
#define GP_IO_SEL3		0x44
#define GP_LVL3			0x48
#define GP_RST_SEL1		0x60
#define GP_RST_SEL2		0x64
#define GP_RST_SEL3		0x68

#define GPIO_MODE_NATIVE	0
#define GPIO_MODE_GPIO		1
#define GPIO_MODE_NONE		1

#define GPIO_DIR_OUTPUT		0
#define GPIO_DIR_INPUT		1

#define GPIO_NO_INVERT		0
#define GPIO_INVERT		1

#define GPIO_LEVEL_LOW		0
#define GPIO_LEVEL_HIGH		1

#define GPIO_NO_BLINK		0
#define GPIO_BLINK		1

#define GPIO_RESET_PWROK	0
#define GPIO_RESET_RSMRST	1

struct pch_gpio_set1 {
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

struct pch_gpio_set2 {
	u32 gpio32 : 1;
	u32 gpio33 : 1;
	u32 gpio34 : 1;
	u32 gpio35 : 1;
	u32 gpio36 : 1;
	u32 gpio37 : 1;
	u32 gpio38 : 1;
	u32 gpio39 : 1;
	u32 gpio40 : 1;
	u32 gpio41 : 1;
	u32 gpio42 : 1;
	u32 gpio43 : 1;
	u32 gpio44 : 1;
	u32 gpio45 : 1;
	u32 gpio46 : 1;
	u32 gpio47 : 1;
	u32 gpio48 : 1;
	u32 gpio49 : 1;
	u32 gpio50 : 1;
	u32 gpio51 : 1;
	u32 gpio52 : 1;
	u32 gpio53 : 1;
	u32 gpio54 : 1;
	u32 gpio55 : 1;
	u32 gpio56 : 1;
	u32 gpio57 : 1;
	u32 gpio58 : 1;
	u32 gpio59 : 1;
	u32 gpio60 : 1;
	u32 gpio61 : 1;
	u32 gpio62 : 1;
	u32 gpio63 : 1;
} __attribute__ ((packed));

struct pch_gpio_set3 {
	u32 gpio64 : 1;
	u32 gpio65 : 1;
	u32 gpio66 : 1;
	u32 gpio67 : 1;
	u32 gpio68 : 1;
	u32 gpio69 : 1;
	u32 gpio70 : 1;
	u32 gpio71 : 1;
	u32 gpio72 : 1;
	u32 gpio73 : 1;
	u32 gpio74 : 1;
	u32 gpio75 : 1;
} __attribute__ ((packed));

struct pch_gpio_map {
	struct {
		const struct pch_gpio_set1 *mode;
		const struct pch_gpio_set1 *direction;
		const struct pch_gpio_set1 *level;
		const struct pch_gpio_set1 *reset;
		const struct pch_gpio_set1 *invert;
		const struct pch_gpio_set1 *blink;
	} set1;
	struct {
		const struct pch_gpio_set2 *mode;
		const struct pch_gpio_set2 *direction;
		const struct pch_gpio_set2 *level;
		const struct pch_gpio_set2 *reset;
	} set2;
	struct {
		const struct pch_gpio_set3 *mode;
		const struct pch_gpio_set3 *direction;
		const struct pch_gpio_set3 *level;
		const struct pch_gpio_set3 *reset;
	} set3;
};

/* Configure GPIOs with mainboard provided settings */
void setup_pch_gpios(const struct pch_gpio_map *gpio);

#endif
