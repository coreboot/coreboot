/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Google Inc.
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

#include <bootmode.h>
#include <console/console.h>
#include <delay.h>
#include <ec/google/chromeec/ec.h>
#include <gpio.h>
#include <mainboard/google/fizz/gpio.h>
#include <soc/gpio.h>
#include <soc/ramstage.h>
#include <timer.h>

#include "gpio.h"

#define GPIO_HDMI_HPD		GPP_E13
#define GPIO_DP_HPD		GPP_E14

/* TODO: This can be moved to common directory */
static void wait_for_hpd(gpio_t gpio, long timeout)
{
	struct stopwatch sw;

	printk(BIOS_INFO, "Waiting for HPD\n");
	gpio_input(gpio);

	stopwatch_init_msecs_expire(&sw, timeout);
	while (!gpio_get(gpio)) {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_WARNING,
			       "HPD not ready after %ldms. Abort.\n", timeout);
			return;
		}
		mdelay(200);
	}
	printk(BIOS_INFO, "HPD ready after %lu ms\n",
	       stopwatch_duration_msecs(&sw));
}

void mainboard_silicon_init_params(FSP_SIL_UPD *params)
{
	static const long display_timeout_ms = 3000;

	/* This is reconfigured back to whatever FSP-S expects by
	   gpio_configure_pads. */
	gpio_input(GPIO_HDMI_HPD);
	if (display_init_required() && !gpio_get(GPIO_HDMI_HPD)) {
		/* This has to be done before FSP-S runs. */
		if (google_chromeec_wait_for_displayport(display_timeout_ms))
			wait_for_hpd(GPIO_DP_HPD, display_timeout_ms);
	}

	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}
