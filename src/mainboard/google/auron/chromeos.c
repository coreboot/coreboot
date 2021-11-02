/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <types.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <soc/chromeos.h>
#include <southbridge/intel/lynxpoint/lp_gpio.h>

#include "onboard.h"

/* EC_IN_RW is GPIO 25 in samus and 14 otherwise */
#if CONFIG(BOARD_GOOGLE_SAMUS)
#define EC_IN_RW_GPIO	25
#else
#define EC_IN_RW_GPIO	14
#endif

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{-1, ACTIVE_HIGH, get_lid_switch(), "lid"},
		{-1, ACTIVE_HIGH, 0, "power"},
		{-1, ACTIVE_HIGH, gfx_get_init_done(), "oprom"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int get_write_protect_state(void)
{
	return get_gpio(CROS_WP_GPIO);
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AH(CROS_WP_GPIO, CROS_GPIO_DEVICE_NAME),
};
DECLARE_CROS_GPIOS(cros_gpios);

int get_ec_is_trusted(void)
{
	/* EC is trusted if not in RW. */
	return !get_gpio(EC_IN_RW_GPIO);
}
