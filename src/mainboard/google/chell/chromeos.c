/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Inc.
 * Copyright (C) 2015 Intel Corporation
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <rules.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <string.h>
#include <ec/google/chromeec/ec.h>
#include <vendorcode/google/chromeos/chromeos.h>

#include "gpio.h"
#include "ec.h"

#if ENV_RAMSTAGE
#include <boot/coreboot_tables.h>

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio *start_gpio = gpios->gpios;
	struct lb_gpio *gpio = start_gpio;

	fill_lb_gpio(gpio++, -1, ACTIVE_HIGH, "write protect",
		     get_write_protect_state());
	fill_lb_gpio(gpio++, -1, ACTIVE_HIGH, "recovery",
		     get_recovery_mode_switch());
	fill_lb_gpio(gpio++, -1, ACTIVE_HIGH, "developer",
		     get_developer_mode_switch());
	fill_lb_gpio(gpio++, -1, ACTIVE_HIGH, "lid",
		     get_lid_switch());
	fill_lb_gpio(gpio++, -1, ACTIVE_HIGH, "power", 0);
	fill_lb_gpio(gpio++, -1, ACTIVE_HIGH, "oprom", gfx_get_init_done());
	fill_lb_gpio(gpio++, GPIO_EC_IN_RW, ACTIVE_HIGH, "EC in RW",
		     gpio_get(GPIO_EC_IN_RW));

	gpios->count = gpio - start_gpio;
	gpios->size = sizeof(*gpios) + (gpios->count * sizeof(*gpio));
}
#endif /* ENV_RAMSTAGE */

int get_lid_switch(void)
{
	/* Read lid switch state from the EC. */
	return !!(google_chromeec_get_switches() & EC_SWITCH_LID_OPEN);
}

int get_developer_mode_switch(void)
{
	/* No physical developer mode switch. */
	return 0;
}

int get_recovery_mode_switch(void)
{
	/* Check for dedicated recovery switch first. */
	if (google_chromeec_get_switches() & EC_SWITCH_DEDICATED_RECOVERY)
		return 1;

	/* Otherwise check if the EC has posted the keyboard recovery event. */
	return !!(google_chromeec_get_events_b() &
		  EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_RECOVERY));
}

int clear_recovery_mode_switch(void)
{
	/* Clear keyboard recovery event. */
	return google_chromeec_clear_events_b(
		EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_RECOVERY));
}

int get_write_protect_state(void)
{
	/* Read PCH_WP GPIO. */
	return gpio_get(GPIO_PCH_WP);
}
