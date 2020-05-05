/* SPDX-License-Identifier: GPL-2.0-or-later */

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
