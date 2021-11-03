/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <ec/google/chromeec/ec.h>
#include <gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <soc/gpio.h>
#include <variant/gpio.h>

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{-1, ACTIVE_HIGH, get_lid_switch(), "lid"},
		{-1, ACTIVE_HIGH, 0, "power"},
		{-1, ACTIVE_HIGH, gfx_get_init_done(), "oprom"},
		{-1, ACTIVE_HIGH, 0, "EC in RW"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int get_write_protect_state(void)
{
	return 0;
}

void mainboard_chromeos_acpi_generate(void)
{
	const struct cros_gpio *gpios;
	size_t num;

	gpios = variant_cros_gpios(&num);
	chromeos_acpi_gpio_generate(gpios, num);
}

int __weak get_lid_switch(void)
{
	return -1;
}
