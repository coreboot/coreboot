/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <pc80/i8254.h>
#include <delay.h>
#include <gpio.h>

void die_notify(void)
{
	uint8_t blink = 0;

	if (ENV_POSTCAR)
		return;

	/* Make SATA LED blink */
	gpio_output(GPIO_4, 0);

	while (1) {
		gpio_set(GPIO_4, blink);
		delay(2);
		blink ^= 1;
	}
}
