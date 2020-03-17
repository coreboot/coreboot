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

#include <boardid.h>
#include <console/console.h>
#include <gpio.h>

uint32_t board_id(void)
{
	static int id = -1;

	if (id < 0) {
		id = gpio_get(GPIO(Q3)) << 0 |
		     gpio_get(GPIO(T1)) << 1 |
		     gpio_get(GPIO(X1)) << 2 |
		     gpio_get(GPIO(X4)) << 3;
		printk(BIOS_SPEW, "Board ID: %#x.\n", id);
	}

	return id;
}
