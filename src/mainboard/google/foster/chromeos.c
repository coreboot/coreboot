/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <gpio.h>

void fill_lb_gpios(struct lb_gpios *gpios)
{
	/* TBD(twarren@nvidia.com): Any analogs for these on Foster-FFD? */
	struct lb_gpio chromeos_gpios[] = {
		/* TODO: Power: active low / high depending on board id */
		{GPIO(X5), ACTIVE_LOW, -1, "power"},

		/* TODO: Reset: active low (output) */
		{GPIO(I5), ACTIVE_LOW, -1, "reset"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int get_recovery_mode_switch(void)
{
	return 0;
}

int get_write_protect_state(void)
{
	return 0;
}
