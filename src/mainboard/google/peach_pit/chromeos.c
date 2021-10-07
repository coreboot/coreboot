/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <bootmode.h>
#include <ec/google/chromeec/ec.h>
#include <ec/google/chromeec/ec_commands.h>
#include <soc/cpu.h>
#include <soc/gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		/* Lid: active high (LID_GPIO) */
		{EXYNOS5_GPX3, ACTIVE_HIGH, gpio_get_value(GPIO_X34), "lid"},

		/* Power: virtual GPIO active low (POWER_GPIO) */
		{EXYNOS5_GPX1, ACTIVE_LOW, gpio_get_value(GPIO_X12), "power"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int get_recovery_mode_switch(void)
{
	uint64_t ec_events;

	/* The GPIO is active low. */
	if (!gpio_get_value(GPIO_X07)) // RECMODE_GPIO
		return 1;

	ec_events = google_chromeec_get_events_b();
	return !!(ec_events &
		  EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_RECOVERY));
}

int get_write_protect_state(void)
{
	return !gpio_get_value(GPIO_X30);
}
