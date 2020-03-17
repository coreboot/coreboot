/*
 * This file is part of the coreboot project.
 *
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
#include <gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>

#include "board.h"

#define GPIO_WP		GPIO(7, A, 6)
#define GPIO_POWER	GPIO(0, A, 5)
#define GPIO_RECOVERY_SERVO	GPIO(0, B, 1)
#define GPIO_RECOVERY_PUSHKEY	GPIO(7, B, 1)


void setup_chromeos_gpios(void)
{
	gpio_input(GPIO_WP);
	gpio_input(GPIO_POWER);
	gpio_input_pullup(GPIO_RECOVERY_SERVO);
	gpio_input_pullup(GPIO_RECOVERY_PUSHKEY);
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		/* Note for early development, we want to support both servo
		 * and pushkey recovery buttons in firmware boot stages. */
		{GPIO_RECOVERY_PUSHKEY.raw, ACTIVE_LOW,
			!get_recovery_mode_switch(), "presence"},
		{GPIO_POWER.raw, ACTIVE_LOW, -1, "power"},
		{GPIO_RESET.raw, ACTIVE_HIGH, -1, "reset"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int get_recovery_mode_switch(void)
{
	// Both RECOVERY_SERVO and RECOVERY_PUSHKEY are low active.
	return !(gpio_get(GPIO_RECOVERY_SERVO) &&
		 gpio_get(GPIO_RECOVERY_PUSHKEY));
}

int get_write_protect_state(void)
{
	return !gpio_get(GPIO_WP);
}
