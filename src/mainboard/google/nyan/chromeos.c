/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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

#include <boot/coreboot_tables.h>
#include <bootmode.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <ec/google/chromeec/ec_commands.h>
#include <gpio.h>
#include <string.h>
#include <vendorcode/google/chromeos/chromeos.h>

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{GPIO(R1), ACTIVE_LOW, gpio_get(GPIO(R1)), "write protect"},
		{-1, ACTIVE_HIGH, get_recovery_mode_switch(), "recovery"},
		{GPIO(R4), ACTIVE_HIGH, -1, "lid"},
		{GPIO(Q0), ACTIVE_LOW, -1, "power"},
		{-1, ACTIVE_HIGH, get_developer_mode_switch(), "developer"},
		{GPIO(U4), ACTIVE_HIGH, -1, "EC in RW"},
		{GPIO(I5), ACTIVE_LOW, -1, "reset"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int get_developer_mode_switch(void)
{
	return 0;
}

int get_recovery_mode_switch(void)
{
	uint32_t ec_events;

	ec_events = google_chromeec_get_events_b();
	return !!(ec_events &
		  EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_RECOVERY));
}

int get_write_protect_state(void)
{
	return !gpio_get(GPIO(R1));
}
