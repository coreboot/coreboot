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

#include <delay.h>
#include <gpiolib.h>

int gpio_get_in_tristate_values(gpio_t gpio[], int num_gpio, int tertiary)
{
	/*
	 * GPIOs which are tied to stronger external pull up or pull down
	 * will stay there regardless of the internal pull up or pull
	 * down setting.
	 *
	 * GPIOs which are floating will go to whatever level they're
	 * internally pulled to.
	 */

	int temp;
	int index;
	int id = 0;
	char value[num_gpio];

	/* Enable internal pull up */
	for (index = 0; index < num_gpio; ++index)
		gpio_input_pullup(gpio[index]);

	/* Wait until signals become stable */
	udelay(10);

	/* Get gpio values at internal pull up */
	for (index = 0; index < num_gpio; ++index)
		value[index] = gpio_get_in_value(gpio[index]);

	/* Enable internal pull down */
	for (index = 0; index < num_gpio; ++index)
		gpio_input_pulldown(gpio[index]);

	/* Wait until signals become stable */
	udelay(10);

	/*
	 * Get gpio values at internal pull down.
	 * Compare with gpio pull up value and then
	 * determine a gpio final value/state:
	 *  0: pull down
	 *  1: pull up
	 *  2: floating
	 */
	for (index = num_gpio - 1; index >= 0; --index) {
		if (tertiary)
			id *= 3;
		else
			id <<= 2;
		temp = gpio_get_in_value(gpio[index]);
		id += ((value[index] ^ temp) << 1) | temp;
	}

	/* Disable pull up / pull down to conserve power */
	for (index = 0; index < num_gpio; ++index)
		gpio_input(gpio[index]);

	return id;
}
