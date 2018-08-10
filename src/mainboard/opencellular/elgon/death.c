/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017-present Facebook, Inc.
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
 */

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
