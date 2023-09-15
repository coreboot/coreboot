/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <gpio.h>
#include <types.h>

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{-1, ACTIVE_HIGH, get_lid_switch(), "lid"},
		{-1, ACTIVE_HIGH, 0, "power"},
		{-1, ACTIVE_HIGH, gfx_get_init_done(), "oprom"},
		{GPIO_EC_IN_RW, ACTIVE_HIGH, gpio_get(GPIO_EC_IN_RW), "EC in RW"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int get_write_protect_state(void)
{
	return gpio_get(GPIO_PCH_WP);
}

int get_ec_is_trusted(void)
{
	/* EC is trusted if not in RW. */
	return !gpio_get(GPIO_EC_IN_RW);
}
