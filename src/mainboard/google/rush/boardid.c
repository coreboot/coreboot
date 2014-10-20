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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <boardid.h>
#include <console/console.h>
#include <soc/gpio.h>

uint8_t board_id(void)
{
	static int id = -1;

	if (id < 0) {
		id = gpio_get_in_value(GPIO(Q3)) << 0 |
		     gpio_get_in_value(GPIO(T1)) << 1 |
		     gpio_get_in_value(GPIO(X1)) << 2 |
		     gpio_get_in_value(GPIO(X4)) << 3;
		printk(BIOS_SPEW, "Board ID: %#x.\n", id);
	}

	return id;
}
