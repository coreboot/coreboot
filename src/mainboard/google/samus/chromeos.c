/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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

#include <string.h>
#include <bootmode.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <console/console.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <ec/google/chromeec/ec.h>
#include <ec/google/chromeec/ec_commands.h>
#include <soc/gpio.h>


/* SPI Write protect is GPIO 16 */
#define CROS_WP_GPIO	16

#ifndef __PRE_RAM__
#include <boot/coreboot_tables.h>

#define GPIO_COUNT	6

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio *gpio;

	gpios->size = sizeof(*gpios) + (GPIO_COUNT * sizeof(struct lb_gpio));
	gpios->count = GPIO_COUNT;

	gpio = gpios->gpios;
	fill_lb_gpio(gpio++, CROS_WP_GPIO, ACTIVE_HIGH, "write protect", 0);
	fill_lb_gpio(gpio++, -1, ACTIVE_HIGH, "recovery",
		     get_recovery_mode_switch());
	fill_lb_gpio(gpio++, -1, ACTIVE_HIGH, "developer",
		     get_developer_mode_switch());
	fill_lb_gpio(gpio++, -1, ACTIVE_HIGH, "lid",
		     get_lid_switch());
	fill_lb_gpio(gpio++, -1, ACTIVE_HIGH, "power", 0);
	fill_lb_gpio(gpio++, -1, ACTIVE_HIGH, "oprom", gfx_get_init_done());
}
#endif

int get_lid_switch(void)
{
	u8 ec_switches = inb(EC_LPC_ADDR_MEMMAP + EC_MEMMAP_SWITCHES);

	return !!(ec_switches & EC_SWITCH_LID_OPEN);
}

/* The dev-switch is virtual */
int get_developer_mode_switch(void)
{
	return 0;
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

int clear_recovery_mode_switch(void)
{
	const uint32_t kb_rec_mask =
		EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_RECOVERY);

	/* Unconditionally clear the EC recovery request. */
	return google_chromeec_clear_events_b(kb_rec_mask);
}

int get_write_protect_state(void)
{
	return get_gpio(CROS_WP_GPIO);
}
