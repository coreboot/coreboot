/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <superio/ite/it8772f/it8772f.h>
#include "onboard.h"

void set_power_led(int state)
{
	int polarity;

	if (CONFIG(BOARD_GOOGLE_TIDUS)) {
		polarity = state == LED_OFF ? 0x00 : 0x01;
	} else {
		polarity = state == LED_BLINK ? 0x01 : 0x00;
	}

	it8772f_gpio_led(IT8772F_GPIO_DEV,
		1,					/* set */
		0x01,					/* select */
		polarity,				/* polarity */
		state == LED_BLINK ? 0x01 : 0x00,	/* pullup/pulldown */
		0x01,					/* output */
		state == LED_BLINK ? 0x00 : 0x01,	/* I/O function */
		SIO_GPIO_BLINK_GPIO10,
		IT8772F_GPIO_BLINK_FREQUENCY_1_HZ);
}
