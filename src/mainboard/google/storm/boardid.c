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

#include <stdlib.h>
#include <boardid.h>
#include <gpiolib.h>
#include <console/console.h>

/*
 * Storm boards dedicate to the board ID three GPIOs in tertiary mode: 29, 30
 * and 68. On proto0 GPIO68 is used and tied low, so it reads as 'zero' by
 * gpio_get_in_tristate_values(), whereas the other two pins are not connected
 * and read as 'two'. This results in gpio_get_in_tristate_values() returning
 * 8 on proto0.
 *
 * Three tertitiary signals could represent 27 different values. To make
 * calculated board ID value continuous and starting at zero, offset the
 * calculated value by 19 (i.e. 27 - 8) and return modulo 27 of the offset
 * number. This results in proto0 returning zero as the board ID, the future
 * revisions will have the inputs configured to match the actual board
 * revision.
 */

static int board_id_value = -1;

static uint8_t get_board_id(void)
{
	uint8_t bid;
	gpio_t hw_rev_gpios[] = {29, 30, 68};
	int offset = 19;

	bid = gpio_get_in_tristate_values(hw_rev_gpios,
					  ARRAY_SIZE(hw_rev_gpios), 1);
	bid = (bid + offset) % 27;
	printk(BIOS_INFO, "Board ID %d\n", bid);

	return bid;
}

uint8_t board_id(void)
{
	if (board_id_value < 0)
		board_id_value = get_board_id();

	return board_id_value;
}
