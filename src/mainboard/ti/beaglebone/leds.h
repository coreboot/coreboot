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

#ifndef __MAINBOARD_TI_BEAGLEBONE_LEDS_H__
#define __MAINBOARD_TI_BEAGLEBONE_LEDS_H__

enum beaglebone_led {
	BEAGLEBONE_LED_USR0,
	BEAGLEBONE_LED_USR1,
	BEAGLEBONE_LED_USR2,
	BEAGLEBONE_LED_USR3,
	BEAGLEBONE_LED_COUNT
};

void beaglebone_leds_init(void);
void beaglebone_leds_set(enum beaglebone_led led, int on);

#endif	/* __MAINBOARD_TI_BEAGLEBONE_LEDS_H__ */
