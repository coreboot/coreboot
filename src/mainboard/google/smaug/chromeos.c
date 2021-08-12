/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <boot/coreboot_tables.h>

#include "gpio.h"

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{POWER_BUTTON, ACTIVE_LOW, -1, "power"},
		{EC_IN_RW, ACTIVE_HIGH, -1, "EC in RW"},
		{AP_SYS_RESET_L, ACTIVE_LOW, -1, "reset"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int get_write_protect_state(void)
{
	return !gpio_get(WRITE_PROTECT_L);
}

int get_ec_is_trusted(void)
{
	/* EC is trusted if not in RW. */
	return !gpio_get(EC_IN_RW);
}
