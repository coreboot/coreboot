/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <gpio.h>

#include "board.h"

#define GPIO_WP		GPIO(7, A, 6)
#define GPIO_RECOVERY	GPIO(0, B, 1)

void setup_chromeos_gpios(void)
{
	gpio_input(GPIO_WP);
	gpio_input_pullup(GPIO_RECOVERY);
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{GPIO_RECOVERY.raw, ACTIVE_LOW,
			!get_recovery_mode_switch(), "presence"},
		{GPIO_RESET.raw, ACTIVE_HIGH, -1, "reset"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int get_recovery_mode_switch(void)
{
	return !gpio_get(GPIO_RECOVERY);
}

int get_write_protect_state(void)
{
	return !gpio_get(GPIO_WP);
}
