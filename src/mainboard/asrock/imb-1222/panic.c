/* SPDX-License-Identifier: GPL-2.0-only */

#include <delay.h>
#include <console/console.h>
#include <mainboard/beep.h>

static void panic(void)
{
	while (1) {
		mainboard_beep_and_blink_on_panic_once();
		mdelay(200);
		mainboard_beep_and_blink_on_panic_once();
		mdelay(200);
		mainboard_beep_and_blink_on_panic_once();
		delay(1);
	}
}

void die_notify(void)
{
	if (ENV_POSTCAR)
		return;

	panic();
}
