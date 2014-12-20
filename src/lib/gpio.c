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

#include <assert.h>
#include <base3.h>
#include <console/console.h>
#include <delay.h>
#include <gpio.h>

int gpio_base2_value(gpio_t gpio[], int num_gpio)
{
	int i, result = 0;

	for (i = 0; i < num_gpio; i++)
		gpio_input(gpio[i]);

	/* Wait until signals become stable */
	udelay(10);

	for (i = 0; i < num_gpio; i++)
		result |= gpio_get(gpio[i]) << i;

	return result;
}

int gpio_base3_value(gpio_t gpio[], int num_gpio)
{
	/*
	 * GPIOs which are tied to stronger external pull up or pull down
	 * will stay there regardless of the internal pull up or pull
	 * down setting.
	 *
	 * GPIOs which are floating will go to whatever level they're
	 * internally pulled to.
	 */

	static const char tristate_char[] = {[0] = '0', [1] = '1', [Z] = 'Z'};
	int temp;
	int index;
	int result = 0;
	char value[32];
	assert(num_gpio <= 32);

	/* Enable internal pull up */
	for (index = 0; index < num_gpio; ++index)
		gpio_input_pullup(gpio[index]);

	/* Wait until signals become stable */
	udelay(10);

	/* Get gpio values at internal pull up */
	for (index = 0; index < num_gpio; ++index)
		value[index] = gpio_get(gpio[index]);

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
	printk(BIOS_DEBUG, "Reading tristate GPIOs: ");
	for (index = num_gpio - 1; index >= 0; --index) {
		temp = gpio_get(gpio[index]);
		temp |= ((value[index] ^ temp) << 1);
		printk(BIOS_DEBUG, "%c ", tristate_char[temp]);
		result = (result * 3) + temp;
	}
	printk(BIOS_DEBUG, "= %d\n", result);

	/* Disable pull up / pull down to conserve power */
	for (index = 0; index < num_gpio; ++index)
		gpio_input(gpio[index]);

	return result;
}
