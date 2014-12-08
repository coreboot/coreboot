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
#include <gpio.h>
#include <stdlib.h>

uint8_t board_id(void)
{
	static int id = -1;
	static gpio_t pins[] = {[3] = GPIO(2, A, 7), [2] = GPIO(2, A, 2),
		[1] = GPIO(2, A, 1), [0] = GPIO(2, A, 0)}; /* GPIO2_A0 is LSB */

	if (id < 0) {
		id = gpio_base2_value(pins, ARRAY_SIZE(pins));
		printk(BIOS_SPEW, "Board ID: %d.\n", id);
	}

	return id;
}

uint32_t ram_code(void)
{
	uint32_t code;
	static gpio_t pins[] = {[3] = GPIO(8, A, 3), [2] = GPIO(8, A, 2),
		[1] = GPIO(8, A, 1), [0] = GPIO(8, A, 0)}; /* GPIO8_A0 is LSB */

	code = gpio_base2_value(pins, ARRAY_SIZE(pins));
	printk(BIOS_SPEW, "RAM Config: %u.\n", code);

	return code;
}
