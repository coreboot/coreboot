/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
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

#include <arch/acpi.h>
#include <boot/coreboot_tables.h>
#include <gpio.h>
#include <rules.h>
#include <soc/gpio.h>
#include <variant/gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{-1, ACTIVE_HIGH, get_write_protect_state(), "write protect"},
		{-1, ACTIVE_HIGH, get_recovery_mode_switch(), "recovery"},
		{-1, ACTIVE_HIGH, get_lid_switch(), "lid"},
		{-1, ACTIVE_HIGH, 0, "power"},
		{-1, ACTIVE_HIGH, gfx_get_init_done(), "oprom"},
		{-1, ACTIVE_HIGH, 0, "EC in RW"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

static int cros_get_gpio_value(int type)
{
	const struct cros_gpio *cros_gpios;
	size_t i, num_gpios = 0;

	cros_gpios = variant_cros_gpios(&num_gpios);

	for (i = 0; i < num_gpios; i++) {
		const struct cros_gpio *gpio = &cros_gpios[i];
		if (gpio->type == type) {
			int state = gpio_get(gpio->gpio_num);
			if (gpio->polarity == CROS_GPIO_ACTIVE_LOW)
				return !state;
			else
				return state;
		}
	}
	return 0;
}

void mainboard_chromeos_acpi_generate(void)
{
	const struct cros_gpio *cros_gpios;
	size_t num_gpios = 0;

	cros_gpios = variant_cros_gpios(&num_gpios);

	chromeos_acpi_gpio_generate(cros_gpios, num_gpios);
}

int get_write_protect_state(void)
{
	return cros_get_gpio_value(CROS_GPIO_WP);
}

int get_recovery_mode_switch(void)
{
	return cros_get_gpio_value(CROS_GPIO_REC);
}

int get_lid_switch(void)
{
	return 1;
}
