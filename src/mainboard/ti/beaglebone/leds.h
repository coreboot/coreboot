/* SPDX-License-Identifier: GPL-2.0-or-later */

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
