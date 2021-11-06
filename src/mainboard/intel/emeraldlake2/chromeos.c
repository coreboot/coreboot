/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <device/device.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>
#include <types.h>
#include <vendorcode/google/chromeos/chromeos.h>

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		/* Recovery: GPIO22 */
		{22, ACTIVE_LOW, !get_recovery_mode_switch(), "presence"},

		/* Hard code the lid switch GPIO to open. */
		{-1, ACTIVE_HIGH, 1, "lid"},

		/* Power Button */
		{-1, ACTIVE_HIGH, 0, "power"},

		/* Did we load the VGA option ROM? */
		{-1, ACTIVE_HIGH, gfx_get_init_done(), "oprom"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int get_recovery_mode_switch(void)
{
	/* Recovery: GPIO22, active low */
	return !get_gpio(22);
}

int get_write_protect_state(void)
{
	/* Write protect is active low, so invert it here */
	return !get_gpio(48);
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(22, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AL(48, CROS_GPIO_DEVICE_NAME),
};

void mainboard_chromeos_acpi_generate(void)
{
	chromeos_acpi_gpio_generate(cros_gpios, ARRAY_SIZE(cros_gpios));
}
