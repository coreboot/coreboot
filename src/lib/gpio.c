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
 */

#include <assert.h>
#include <base3.h>
#include <compiler.h>
#include <console/console.h>
#include <delay.h>
#include <gpio.h>

static void _check_num(const char *name, int num)
{
	if ((num > 31) || (num < 1)) {
		printk(BIOS_EMERG, "%s: %d ", name, num);
		die("is an invalid number of GPIOs");
	}
}

static uint32_t _gpio_base2_value(const gpio_t gpio[], int num_gpio)
{
	uint32_t result = 0;
	int i;

	/* Wait until signals become stable */
	udelay(10);

	for (i = 0; i < num_gpio; i++)
		result |= gpio_get(gpio[i]) << i;

	return result;
}

uint32_t gpio_base2_value(const gpio_t gpio[], int num_gpio)
{
	int i;

	_check_num(__func__, num_gpio);
	for (i = 0; i < num_gpio; i++)
		gpio_input(gpio[i]);

	return _gpio_base2_value(gpio, num_gpio);
}

uint32_t gpio_pulldown_base2_value(const gpio_t gpio[], int num_gpio)
{
	int i;

	_check_num(__func__, num_gpio);
	for (i = 0; i < num_gpio; i++)
		gpio_input_pulldown(gpio[i]);

	return _gpio_base2_value(gpio, num_gpio);
}

uint32_t gpio_pullup_base2_value(const gpio_t gpio[], int num_gpio)
{
	int i;

	_check_num(__func__, num_gpio);
	for (i = 0; i < num_gpio; i++)
		gpio_input_pullup(gpio[i]);

	return _gpio_base2_value(gpio, num_gpio);
}

uint32_t _gpio_base3_value(const gpio_t gpio[], int num_gpio, int binary_first)
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
	uint32_t result = 0;
	int has_z = 0;
	int binary_below = 0;
	int index;
	int temp;
	char value[32];

	_check_num(__func__, num_gpio);

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

		/*
		 * For binary_first we keep track of the normal ternary result
		 * and whether we found any pin that was a Z. We also determine
		 * the amount of numbers that can be represented with only
		 * binary digits (no Z) whose value in the normal ternary system
		 * is lower than the one we are parsing. Counting from the left,
		 * we add 2^i for any '1' digit to account for the binary
		 * numbers whose values would be below it if all following
		 * digits we parsed would be '0'. As soon as we find a '2' digit
		 * we can total the remaining binary numbers below as 2^(i+1)
		 * because we know that all binary representations counting only
		 * this and following digits must have values below our number
		 * (since 1xxx is always smaller than 2xxx).
		 *
		 * Example: 1 0 2 1 (counting from the left / most significant)
		 * '1' at 3^3: Add 2^3 = 8 to account for binaries 0000-0111
		 * '0' at 3^2: Ignore (not all binaries 1000-1100 are below us)
		 * '2' at 3^1: Add 2^(1+1) = 4 to account for binaries 1000-1011
		 * Stop adding for lower digits (3^0), all already accounted
		 * now. We know that there can be no binary numbers 1020-102X.
		 */
		if (binary_first && !has_z) {
			switch (temp) {
			case 0:	/* Ignore '0' digits. */
				break;
			case 1:	/* Account for binaries 0 to 2^index - 1. */
				binary_below += 1 << index;
				break;
			case 2:	/* Account for binaries 0 to 2^(index+1) - 1. */
				binary_below += 1 << (index + 1);
				has_z = 1;
			}
		}
	}

	if (binary_first) {
		if (has_z)
			result = result + (1 << num_gpio) - binary_below;
		else /* binary_below is normal binary system value if !has_z. */
			result = binary_below;
	}

	printk(BIOS_DEBUG, "= %d (%s base3 number system)\n", result,
	       binary_first ? "binary_first" : "standard");

	/* Disable pull up / pull down to conserve power */
	for (index = 0; index < num_gpio; ++index)
		gpio_input(gpio[index]);

	return result;
}

/* Default handler for ACPI path is to return NULL */
__weak const char *gpio_acpi_path(gpio_t gpio)
{
	return NULL;
}

/* Default handler returns 0 because type of gpio_t is unknown */
__weak uint16_t gpio_acpi_pin(gpio_t gpio)
{
	return 0;
}
