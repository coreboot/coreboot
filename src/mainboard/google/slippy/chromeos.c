/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <string.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <southbridge/intel/lynxpoint/pch.h>

#if CONFIG_EC_GOOGLE_CHROMEEC
#include "ec.h"
#include <ec/google/chromeec/ec.h>
#endif

#ifndef __PRE_RAM__
#include <boot/coreboot_tables.h>

#define GPIO_COUNT	6
#define ACTIVE_LOW	0
#define ACTIVE_HIGH	1

static int get_lid_switch(void)
{
#if CONFIG_EC_GOOGLE_CHROMEEC
	u8 ec_switches = inb(EC_LPC_ADDR_MEMMAP + EC_MEMMAP_SWITCHES);

	return !!(ec_switches & EC_SWITCH_LID_OPEN);
#else
	return 0;
#endif
}

static void fill_lb_gpio(struct lb_gpio *gpio, int num,
			 int polarity, const char *name, int force)
{
	memset(gpio, 0, sizeof(*gpio));
	gpio->port = num;
	gpio->polarity = polarity;
	if (force >= 0)
		gpio->value = force;
	else if (num >= 0)
		gpio->value = get_gpio(num);
	strncpy((char *)gpio->name, name, GPIO_MAX_NAME_LENGTH);
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio *gpio;

	gpios->size = sizeof(*gpios) + (GPIO_COUNT * sizeof(struct lb_gpio));
	gpios->count = GPIO_COUNT;

	gpio = gpios->gpios;
	fill_lb_gpio(gpio++, 58, ACTIVE_HIGH, "write protect", 0);
	fill_lb_gpio(gpio++, -1, ACTIVE_HIGH, "recovery",
		     get_recovery_mode_switch());
	fill_lb_gpio(gpio++, -1, ACTIVE_HIGH, "developer",
		     get_developer_mode_switch());
	fill_lb_gpio(gpio++, -1, ACTIVE_HIGH, "lid",
		     get_lid_switch());
	fill_lb_gpio(gpio++, -1, ACTIVE_HIGH, "power", 0);
	fill_lb_gpio(gpio++, -1, ACTIVE_HIGH, "oprom", oprom_is_loaded);
}
#endif

/* The dev-switch is virtual */
int get_developer_mode_switch(void)
{
	return 1;
}

/* There are actually two recovery switches. One is the magic keyboard chord,
 * the other is driven by Servo. */
int get_recovery_mode_switch(void)
{
#if CONFIG_EC_GOOGLE_CHROMEEC
	u8 ec_switches = inb(EC_LPC_ADDR_MEMMAP + EC_MEMMAP_SWITCHES);
	u32 ec_events;

	/* If a switch is set, we don't need to look at events. */
	if (ec_switches & (EC_SWITCH_DEDICATED_RECOVERY))
		return 1;

	/* Else check if the EC has posted the keyboard recovery event. */
	ec_events = google_chromeec_get_events_b();

	return !!(ec_events &
		  EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_RECOVERY));
#else
	return 0;
#endif
}

int get_write_protect_state(void)
{
	return get_gpio(58);
}
