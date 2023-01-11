/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <device/device.h>
#include <gpio.h>
#include <ec/google/chromeec/ec.h>
#include <types.h>
#include <vendorcode/google/chromeos/chromeos.h>

#include "gpio.h"
#include "ec.h"

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{-1, ACTIVE_HIGH, get_lid_switch(), "lid"},
		{-1, ACTIVE_HIGH, 0, "power"},
		{-1, ACTIVE_HIGH, gfx_get_init_done(), "oprom"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int get_lid_switch(void)
{
	if (CONFIG(EC_GOOGLE_CHROMEEC))
		/* Read lid switch state from the EC. */
		return !!(google_chromeec_get_switches() & EC_SWITCH_LID_OPEN);

	/* Lid always open */
	return 1;
}

int get_recovery_mode_switch(void)
{
	if (CONFIG(EC_GOOGLE_CHROMEEC)) {
		/* Check for dedicated recovery switch first. */
		if (google_chromeec_get_switches() &
			EC_SWITCH_DEDICATED_RECOVERY)
		return 1;

		/* Otherwise check if the EC has posted the keyboard recovery
		 * event. */
		return !!(google_chromeec_get_events_b() &
			  EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_RECOVERY));
	}

	return 0;
}

int get_write_protect_state(void)
{
	/* No write protect */
	return 0;
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AH(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE_NAME),
};
DECLARE_CROS_GPIOS(cros_gpios);

int get_ec_is_trusted(void)
{
	/* Do not have a Chrome EC involved in entering recovery mode;
	   Always return trusted. */
	return 1;
}
