/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Inc.
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

#include <types.h>
#include <superio/ite/it8772f/it8772f.h>
#include "../../onboard.h"

void set_power_led(int state)
{
	switch (state) {
	case LED_ON:
	case LED_OFF:
		it8772f_gpio_led(IT8772F_GPIO_DEV,
			1 /* set */,
			0x01 /* select */,
			state /* polarity: state dependent */,
			0x00 /* 0=pulldown */,
			0x01 /* output */,
			0x01 /* 1=Simple IO function */,
			SIO_GPIO_BLINK_GPIO10,
			IT8772F_GPIO_BLINK_FREQUENCY_1_HZ);
		break;
	case LED_BLINK:
		it8772f_gpio_led(IT8772F_GPIO_DEV,
			1 /* set */,
			0x01 /* select */,
			0x01 /* polarity */,
			0x01 /* 1=pullup */,
			0x01 /* output */,
			0x00, /* 0=Alternate function */
			SIO_GPIO_BLINK_GPIO10,
			IT8772F_GPIO_BLINK_FREQUENCY_1_HZ);
		break;
	}
}
