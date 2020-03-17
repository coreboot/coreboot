/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <boot/coreboot_tables.h>
#include <bootmode.h>
#include "board.h"

int get_write_protect_state(void)
{
	return !gpio_get(GPIO_WP_STATE);
}

void setup_chromeos_gpios(void)
{
	gpio_input_pullup(GPIO_EC_IN_RW);
	gpio_input_pullup(GPIO_AP_EC_INT);
	gpio_output(GPIO_AP_SUSPEND, 1);
	gpio_input(GPIO_WP_STATE);
	gpio_input_pullup(GPIO_H1_AP_INT);
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{GPIO_EC_IN_RW.addr, ACTIVE_LOW, gpio_get(GPIO_EC_IN_RW),
			"EC in RW"},
		{GPIO_AP_EC_INT.addr, ACTIVE_LOW, gpio_get(GPIO_AP_EC_INT),
			"EC interrupt"},
		{GPIO_H1_AP_INT.addr, ACTIVE_LOW, gpio_get(GPIO_H1_AP_INT),
			"TPM interrupt"},
	};

	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}
