/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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

#include <boardid.h>
#include <console/console.h>
#include <gpio.h>
#include <stdlib.h>

uint8_t board_id(void)
{
	static int id = -1;
	gpio_t gpio[] = {[3] = GPIO(X4), [2] = GPIO(X1),	/* X4 is MSB */
			 [1] = GPIO(T1), [0] = GPIO(Q3),};	/* Q3 is LSB */

	if (id < 0) {
		id = gpio_base3_value(gpio, ARRAY_SIZE(gpio));

		printk(BIOS_SPEW, "Board TRISTATE ID: %d.\n", id);
	}

	return id;
}
