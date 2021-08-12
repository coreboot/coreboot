/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>

#define GPIO_EC_IN_RW 21

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		/* Lid: the "switch" comes from the EC */
		{-1, ACTIVE_HIGH, get_lid_switch(), "lid"},

		/* Power Button: hard-coded as not pressed; we'll detect later
		 * presses via SMI. */
		{-1, ACTIVE_HIGH, 0, "power"},

		/* Did we load the VGA Option ROM? */
		/* -1 indicates that this is a pseudo GPIO */
		{-1, ACTIVE_HIGH, gfx_get_init_done(), "oprom"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int get_write_protect_state(void)
{
	return get_gpio(57);
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(9, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AH(57, CROS_GPIO_DEVICE_NAME),
};

void mainboard_chromeos_acpi_generate(void)
{
	chromeos_acpi_gpio_generate(cros_gpios, ARRAY_SIZE(cros_gpios));
}

int get_ec_is_trusted(void)
{
	/* EC is trusted if not in RW. */
	return !get_gpio(GPIO_EC_IN_RW);
}
