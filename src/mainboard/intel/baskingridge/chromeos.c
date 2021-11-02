/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <device/device.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <southbridge/intel/common/gpio.h>
#include <types.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include "onboard.h"

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		/* Recovery: GPIO69 - SV_DETECT - J8E3 (silkscreen: J8E2) */
		{GPIO_REC_MODE, ACTIVE_HIGH, get_recovery_mode_switch(), "presence"},

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
	/*
	 * Recovery: GPIO69, Connected to J8E3, however the silkscreen says
	 * J8E2. The jump is active high.
	 */
	return get_gpio(GPIO_REC_MODE);
}

int get_write_protect_state(void)
{
	/* Write protect is active low, so invert it here */
	return !get_gpio(GPIO_SPI_WP);
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AH(GPIO_REC_MODE, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AL(GPIO_SPI_WP, CROS_GPIO_DEVICE_NAME),
};

const struct cros_gpio *variant_cros_gpios(size_t *num)
{
	*num = ARRAY_SIZE(cros_gpios);
	return cros_gpios;
}
