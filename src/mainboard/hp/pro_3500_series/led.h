/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_LED_H
#define __MAINBOARD_LED_H

#ifndef __ACPI__
void set_power_led(int state);

enum {
	LED_WHITE =  1 << 2, // GPIO 22
	LED_YELLOW = 1 << 3, // GPIO 23
	LED_BOTH = LED_WHITE | LED_YELLOW,
	LED_OFF = LED_BOTH
};
#endif

#endif /* __MAINBOARD_LED_H */
