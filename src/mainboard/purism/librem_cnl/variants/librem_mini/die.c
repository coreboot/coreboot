/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <gpio.h>

void die_notify(void)
{
	if (ENV_POSTCAR)
		return;

	/* Make SATA LED blink */
	gpio_output(GPP_E8, 1);

	while (1) {
		gpio_set(GPP_E8, 0);
		mdelay(100);
		gpio_set(GPP_E8, 1);
		mdelay(100);
	}
}
