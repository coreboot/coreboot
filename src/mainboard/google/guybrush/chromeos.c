/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <boot/coreboot_tables.h>
#include <gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{-1, ACTIVE_HIGH, get_lid_switch(), "lid"},
		{-1, ACTIVE_HIGH, 0, "power"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}
