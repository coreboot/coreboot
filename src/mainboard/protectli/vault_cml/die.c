/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <pc80/i8254.h>
#include <delay.h>
#include <gpio.h>

static void beep_and_blink(void)
{
	static uint8_t blink = 0;
	static uint8_t beep_count = 0;

	gpio_set(GPP_E8, blink);
	/* Beep 12 times at most, constant beeps may be annoying */
	if (beep_count < 12) {
		beep(800, 300);
		mdelay(200);
		beep_count++;
	} else {
		mdelay(500);
	}

	blink ^= 1;
}

void die_notify(void)
{
	if (ENV_POSTCAR)
		return;

	/* Make SATA LED blink and use PC SPKR */
	gpio_output(GPP_E8, 0);

	while (1) {
		beep_and_blink();
		beep_and_blink();
		beep_and_blink();
		beep_and_blink();
		delay(2);
	}
}
