/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
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

#include <ec/google/chromeec/ec.h>
#include <fsp/car.h>
#include <soc/gpio.h>
#include "gpio.h"

static void early_config_gpio(void)
{
	/* This is a hack for FSP because it does things in MemoryInit()
	 * which it shouldn't be. We have to prepare certain gpios here
	 * because of the brokenness in FSP. */
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}

void car_mainboard_post_console_init(void)
{
	/* Ensure the EC and PD are in the right mode for recovery */
	google_chromeec_early_init();

	early_config_gpio();
}
