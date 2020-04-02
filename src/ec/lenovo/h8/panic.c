/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */
#include <console/console.h>
#include <ec/acpi/ec.h>

#include "h8.h"

static void h8_panic(void)
{
	if (CONFIG(H8_FLASH_LEDS_ON_DEATH)) {
		static const u8 leds[] = {
			H8_LED_CONTROL_POWER_LED,
			H8_LED_CONTROL_BAT0_LED,
			H8_LED_CONTROL_BAT1_LED,
			H8_LED_CONTROL_UBAY_LED,
			H8_LED_CONTROL_SUSPEND_LED,
			H8_LED_CONTROL_DOCK_LED1,
			H8_LED_CONTROL_DOCK_LED2,
			H8_LED_CONTROL_ACDC_LED,
			H8_LED_CONTROL_MUTE_LED
		};

		/* Flash all LEDs */
		for (size_t i = 0; i < ARRAY_SIZE(leds); i++)
			ec_write(H8_LED_CONTROL,
				 H8_LED_CONTROL_BLINK | leds[i]);
	}
	if (CONFIG(H8_BEEP_ON_DEATH)) {
		/* Beep 4 Sec. 1250 Hz */
		ec_write(H8_SOUND_ENABLE1, 4);
		ec_write(H8_SOUND_REPEAT, 1);
		ec_write(H8_SOUND_REG, 10);
	}
}

void die_notify(void)
{
	h8_panic();
}
