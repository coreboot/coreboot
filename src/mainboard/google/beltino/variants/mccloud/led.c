/* SPDX-License-Identifier: GPL-2.0-only */

#include <superio/ite/it8772f/it8772f.h>
#include "../../onboard.h"

void set_power_led(int state)
{
	it8772f_gpio_led(IT8772F_GPIO_DEV,
		1,					/* set */
		0x01,					/* select */
		state == LED_BLINK ? 0x01 : 0x00,	/* polarity */
		state == LED_BLINK ? 0x01 : 0x00,	/* pullup/pulldown */
		0x01,					/* output */
		state == LED_BLINK ? 0x00 : 0x01,	/* I/O function */
		SIO_GPIO_BLINK_GPIO10,
		IT8772F_GPIO_BLINK_FREQUENCY_1_HZ);
}
