/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <superio/ite/it8772f/it8772f.h>
#include "../../onboard.h"

void set_power_led(int state)
{
	it8772f_gpio_led(IT8772F_GPIO_DEV,
		2,					/* set */
		0xF7,					/* select */
		state == LED_OFF ? 0x00 : 0x04,		/* polarity */
		state == LED_BLINK ? 0x04 : 0x00,	/* pullup/pulldown */
		0x04,					/* output */
		state == LED_BLINK ? 0x00 : 0x04,	/* I/O function */
		SIO_GPIO_BLINK_GPIO22,
		IT8772F_GPIO_BLINK_FREQUENCY_1_HZ);
}
