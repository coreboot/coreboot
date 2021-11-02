/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <soc/gpio.h>
#include <types.h>
#include <vendorcode/google/chromeos/chromeos.h>

/* The WP status pin lives on GPIO_SSUS_6 which is pad 36 in the SUS well. */
#define WP_STATUS_PAD	36

/* The EC_IN_RW lives on SCGPIO59 */
#define EC_IN_RW_PAD	59

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
	/*
	 * The vboot loader queries this function in romstage. The GPIOs have
	 * not been set up yet as that configuration is done in ramstage. The
	 * hardware defaults to an input but there is a 20K pulldown. Externally
	 * there is a 10K pullup. Disable the internal pull in romstage so that
	 * there isn't any ambiguity in the reading.
	 */
	if (ENV_ROMSTAGE)
		ssus_disable_internal_pull(WP_STATUS_PAD);

	/* WP is enabled when the pin is reading high. */
	return ssus_get_gpio(WP_STATUS_PAD);
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AH(0x2006, CROS_GPIO_DEVICE_NAME),
};
DECLARE_CROS_GPIOS(cros_gpios);

int get_ec_is_trusted(void)
{
	/* EC is trusted if not in RW. */
	return !score_get_gpio(EC_IN_RW_PAD);
}
