/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>
#include <types.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include "onboard.h"

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
	return get_gpio(GPIO_SPI_WP);
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(GPIO_REC_MODE, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AH(GPIO_SPI_WP, CROS_GPIO_DEVICE_NAME),
};
DECLARE_CROS_GPIOS(cros_gpios);

int get_ec_is_trusted(void)
{
	/* EC is trusted if not in RW. */
	return !get_gpio(GPIO_EC_IN_RW);
}
