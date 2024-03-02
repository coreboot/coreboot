/* SPDX-License-Identifier: GPL-2.0-only */

#include "common_defines.h"
#include "led.h"

void set_power_led(int state)
{
	// Board has a dual color LED
	it8772f_gpio_setup(
		GPIO_DEV,
		2,			/* set */
		0xf3 | LED_BOTH,	/* select, 0xf3 is default */
		state,			/* polarity */
		0x00,			/* pullup */
		LED_BOTH,		/* output */
		0x00);			/* enable */
}
