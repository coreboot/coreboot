/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Rockchip Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <boot/coreboot_tables.h>
#include <gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>

#include "board.h"

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{GPIO_WP.raw, ACTIVE_LOW, gpio_get(GPIO_WP), "write protect"},
		{GPIO_BACKLIGHT.raw, ACTIVE_HIGH, -1, "backlight"},
		{GPIO_EC_IN_RW.raw, ACTIVE_HIGH, -1, "EC in RW"},
		{GPIO_EC_IRQ.raw, ACTIVE_LOW, -1, "EC interrupt"},
		{GPIO_RESET.raw, ACTIVE_HIGH, -1, "reset"},
	};

	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int get_developer_mode_switch(void)
{
	return 0;
}

int get_recovery_mode_switch(void)
{
	return 0;
}

int get_write_protect_state(void)
{
	return !gpio_get(GPIO_WP);
}

void setup_chromeos_gpios(void)
{
	gpio_input(GPIO_WP);
	gpio_input_pullup(GPIO_EC_IN_RW);
	gpio_input_pullup(GPIO_EC_IRQ);
}
