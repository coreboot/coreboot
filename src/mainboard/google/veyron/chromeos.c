/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <ec/google/chromeec/ec.h>
#include <ec/google/chromeec/ec_commands.h>
#include <gpio.h>

#include "board.h"

#define GPIO_WP		GPIO(7, A, 6)
#define GPIO_LID	GPIO(0, A, 6)
#define GPIO_POWER	GPIO(0, A, 5)
#define GPIO_RECOVERY	GPIO(0, B, 1)
#define GPIO_ECINRW	GPIO(0, A, 7)
#define GPIO_ECIRQ	GPIO(7, A, 7)

void setup_chromeos_gpios(void)
{
	gpio_input(GPIO_WP);
	gpio_input_pullup(GPIO_LID);
	gpio_input(GPIO_POWER);
	gpio_input_pullup(GPIO_RECOVERY);
	gpio_input(GPIO_ECIRQ);
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{GPIO_RECOVERY.raw, ACTIVE_LOW,
			!get_recovery_mode_switch(), "presence"},
		{GPIO_LID.raw, ACTIVE_HIGH, -1, "lid"},
		{GPIO_POWER.raw, ACTIVE_LOW, -1, "power"},
		{GPIO_ECINRW.raw, ACTIVE_HIGH, -1, "EC in RW"},
		{GPIO_ECIRQ.raw, ACTIVE_LOW, -1, "EC interrupt"},
		{GPIO_RESET.raw, ACTIVE_HIGH, -1, "reset"},
		{GPIO_BACKLIGHT.raw, ACTIVE_HIGH, -1, "backlight"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int get_recovery_mode_switch(void)
{
	uint64_t ec_events;

	/* The GPIO is active low. */
	if (!gpio_get(GPIO_RECOVERY))
		return 1;

	ec_events = google_chromeec_get_events_b();
	return !!(ec_events &
		  EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_RECOVERY));
}

int get_write_protect_state(void)
{
	return !gpio_get(GPIO_WP);
}

int get_ec_is_trusted(void)
{
	/* EC is trusted if not in RW. */
	return !gpio_get(GPIO_ECINRW);
}
