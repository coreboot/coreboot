/*
 * Copyright 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <assert.h>
#include <cpu/ti/am335x/gpio.h>

#include "leds.h"

static const int led_gpios[BEAGLEBONE_LED_COUNT] = {
	[BEAGLEBONE_LED_USR0] = AM335X_GPIO_BITS_PER_BANK + 21,
	[BEAGLEBONE_LED_USR1] = AM335X_GPIO_BITS_PER_BANK + 22,
	[BEAGLEBONE_LED_USR2] = AM335X_GPIO_BITS_PER_BANK + 23,
	[BEAGLEBONE_LED_USR3] = AM335X_GPIO_BITS_PER_BANK + 24
};

void beaglebone_leds_init(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(led_gpios); i++)
		gpio_direction_output(led_gpios[i], 0);
}

void beaglebone_leds_set(enum beaglebone_led led, int on)
{
	int res;

	ASSERT(led < ARRAY_SIZE(led_gpios) && led_gpios[led]);
	res = gpio_set_value(led_gpios[led], on);
	ASSERT(res != -1);
}
