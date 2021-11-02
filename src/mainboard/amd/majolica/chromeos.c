/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <gpio.h>
#include <types.h>
#include <vendorcode/google/chromeos/chromeos.h>

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int get_write_protect_state(void)
{
	/* Majolica doesn't have a write protect pin */
	return 0;
}

const struct cros_gpio *variant_cros_gpios(size_t *num)
{
	/* No ChromeOS GPIOs */
	*num = 0;
	return NULL;
}
