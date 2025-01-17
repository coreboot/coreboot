/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <boot/coreboot_tables.h>
#include <gpio.h>

#include "gpio.h"

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{GPIO_XHCI_INIT_DONE.id, ACTIVE_HIGH, -1, "XHCI init done"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}
