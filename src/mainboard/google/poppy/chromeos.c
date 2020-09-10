/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <baseboard/variants.h>
#include <boot/coreboot_tables.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>

#include <variant/gpio.h>

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{-1, ACTIVE_HIGH, get_lid_switch(), "lid"},
		{-1, ACTIVE_HIGH, 0, "power"},
		{-1, ACTIVE_HIGH, gfx_get_init_done(), "oprom"},
		{GPIO_EC_IN_RW, ACTIVE_HIGH, gpio_get(GPIO_EC_IN_RW),
		 "EC in RW"},
#ifdef EC_SYNC_GPIO
		{EC_SYNC_GPIO, ACTIVE_LOW, gpio_get(EC_SYNC_GPIO), "EC sync gpio"},
#endif
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int get_write_protect_state(void)
{
	/* Read PCH_WP GPIO. */
	return gpio_get(GPIO_PCH_WP);
}

void mainboard_chromeos_acpi_generate(void)
{
	const struct cros_gpio *gpios;
	size_t num;

	gpios = variant_cros_gpios(&num);
	chromeos_acpi_gpio_generate(gpios, num);
}
