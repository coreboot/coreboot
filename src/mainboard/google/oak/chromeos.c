/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
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

#include <boardid.h>
#include <boot/coreboot_tables.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <ec/google/chromeec/ec_commands.h>
#include <gpio.h>
#include <string.h>
#include <vendorcode/google/chromeos/chromeos.h>

#include "gpio.h"

void setup_chromeos_gpios(void)
{
	gpio_input(WRITE_PROTECT);
	gpio_input_pullup(EC_IN_RW);
	gpio_input_pullup(EC_IRQ);
	gpio_input_pullup(LID);
	gpio_input_pullup(POWER_BUTTON);
	if (board_id() + CONFIG_BOARD_ID_ADJUSTMENT < 5)
		gpio_output(EC_SUSPEND_L, 1);
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{WRITE_PROTECT, ACTIVE_LOW,
			gpio_get(WRITE_PROTECT), "write protect"},
		{-1, ACTIVE_HIGH, get_recovery_mode_switch(), "recovery"},
		{LID, ACTIVE_HIGH, -1, "lid"},
		{POWER_BUTTON, ACTIVE_HIGH, -1, "power"},
		{-1, ACTIVE_HIGH, get_developer_mode_switch(), "developer"},
		{EC_IN_RW, ACTIVE_HIGH, -1, "EC in RW"},
		{EC_IRQ, ACTIVE_LOW, -1, "EC interrupt"},
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
	return !gpio_get(WRITE_PROTECT);
}
