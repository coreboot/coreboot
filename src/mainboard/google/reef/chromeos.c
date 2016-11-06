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

#include <baseboard/variants.h>
#include <boot/coreboot_tables.h>
#include <ec/google/chromeec/ec.h>
#include <gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <soc/gpio.h>
#include <variant/gpio.h>

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{-1, ACTIVE_HIGH, get_write_protect_state(), "write protect"},
		{-1, ACTIVE_HIGH, get_recovery_mode_switch(), "recovery"},
		{-1, ACTIVE_HIGH, get_developer_mode_switch(), "developer"},
		{-1, ACTIVE_HIGH, get_lid_switch(), "lid"},
		{-1, ACTIVE_HIGH, 0, "power"},
		{-1, ACTIVE_HIGH, gfx_get_init_done(), "oprom"},
		{GPIO_EC_IN_RW, ACTIVE_HIGH,
			gpio_get(GPIO_EC_IN_RW), "EC in RW"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int get_lid_switch(void)
{
	/* Read lid switch state from the EC. */
	return !!(google_chromeec_get_switches() & EC_SWITCH_LID_OPEN);
}

int get_developer_mode_switch(void)
{
	/* No physical developer mode switch. It's virtual. */
	return 0;
}

int get_recovery_mode_switch(void)
{
	/* Check if the EC has posted the keyboard recovery event. */
	return !!(google_chromeec_get_events_b() &
		  EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_RECOVERY));
}

int get_recovery_mode_retrain_switch(void)
{
	/*
	 * Check if the EC has posted the keyboard recovery event with memory
	 * retrain.
	 */
	return !!(google_chromeec_get_events_b() &
		EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_RECOVERY_HW_REINIT));
}

int clear_recovery_mode_switch(void)
{
	/* Clear all host event bits requesting recovery mode. */
	return google_chromeec_clear_events_b(
		EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_RECOVERY) |
		EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_RECOVERY_HW_REINIT));
}

int get_write_protect_state(void)
{
	/* Read PCH_WP GPIO. */
	return gpio_get(GPIO_PCH_WP);
}

void mainboard_chromeos_acpi_generate(void)
{
	const struct cros_gpio *gpios;
	size_t num;

	gpios = variant_cros_gpios(&num);
	chromeos_acpi_gpio_generate(gpios, num);
}
