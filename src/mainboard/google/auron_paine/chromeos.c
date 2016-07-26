/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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

#include <string.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <console/console.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <ec/google/chromeec/ec.h>
#include <soc/gpio.h>
#include "ec.h"

/* SPI Write protect is GPIO 16 */
#define CROS_WP_GPIO	58

#ifndef __PRE_RAM__
#include <boot/coreboot_tables.h>

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{CROS_WP_GPIO, ACTIVE_HIGH, 0, "write protect"},
		{-1, ACTIVE_HIGH, get_recovery_mode_switch(), "recovery"},
		{-1, ACTIVE_HIGH, get_developer_mode_switch(), "developer"},
		{-1, ACTIVE_HIGH, get_lid_switch(), "lid"},
		{-1, ACTIVE_HIGH, 0, "power"},
		{-1, ACTIVE_HIGH, gfx_get_init_done(), "oprom"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
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

int get_write_protect_state(void)
{
	return get_gpio(CROS_WP_GPIO);
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AH(CROS_WP_GPIO, CROS_GPIO_DEVICE_NAME),
};

void mainboard_chromeos_acpi_generate(void)
{
	chromeos_acpi_gpio_generate(cros_gpios, ARRAY_SIZE(cros_gpios));
}
