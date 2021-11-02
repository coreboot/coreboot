/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <device/mmio.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <types.h>
#include <vendorcode/google/chromeos/chromeos.h>

/* The WP status pin lives on MF_ISH_GPIO_4 */
#define WP_STATUS_PAD_CFG0	0x4838
#define WP_STATUS_PAD_CFG1	0x483C

#define WP_GPIO			GP_E_22

#define ACTIVE_LOW	0
#define ACTIVE_HIGH	1

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
	 * not been set up yet as that configuration is done in ramstage.
	 * Configuring this GPIO as input so that there isn't any ambiguity
	 * in the reading.
	 */
#if ENV_ROMSTAGE
	if (CONFIG(BOARD_GOOGLE_CYAN)) {
		write32((void *)(COMMUNITY_GPEAST_BASE + WP_STATUS_PAD_CFG0),
			(PAD_PULL_UP_20K | PAD_GPIO_ENABLE | PAD_CONFIG0_GPI_DEFAULT));
		write32((void *)(COMMUNITY_GPEAST_BASE + WP_STATUS_PAD_CFG1),
			PAD_CONFIG1_DEFAULT0);
	} else {
		gpio_input_pullup(WP_GPIO);
	}
#endif

	/* WP is enabled when the pin is reading high. */
	if (CONFIG(BOARD_GOOGLE_CYAN)) {
		return (read32((void *)(COMMUNITY_GPEAST_BASE + WP_STATUS_PAD_CFG0))
			& PAD_VAL_HIGH);
	} else {
		return !!gpio_get(WP_GPIO);
	}
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AH(0x10013, CROS_GPIO_DEVICE_NAME),
};
DECLARE_CROS_GPIOS(cros_gpios);

int get_ec_is_trusted(void)
{
	/* Do not have a Chrome EC involved in entering recovery mode;
	   Always return trusted. */
	return 1;
}
