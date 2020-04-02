/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <soc/gpio.h>
#include <console/console.h>
#include <delay.h>
#include "mainboard.h"

void die_notify(void)
{
	const u32 rate = 1000 / 10; // blink at 5 Hz

	do {
		gpio_output(ELGON_GPIO_ERROR_LED, 0);
		mdelay(rate);
		gpio_output(ELGON_GPIO_ERROR_LED, 1);
		mdelay(rate);
	} while (1);
}
