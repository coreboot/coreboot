/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <device/device.h>
#include <soc/chromeos.h>
#include <southbridge/intel/lynxpoint/lp_gpio.h>
#include <types.h>
#include <vendorcode/google/chromeos/chromeos.h>

/* Compile-time settings for recovery mode. */
#define REC_MODE_SETTING 0

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{-1, ACTIVE_HIGH, 1, "lid"}, // force open
		{-1, ACTIVE_HIGH, 0, "power"},
		{-1, ACTIVE_HIGH, gfx_get_init_done(), "oprom"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int get_recovery_mode_switch(void)
{
	return REC_MODE_SETTING;
}

int get_write_protect_state(void)
{
	return 0;
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AL(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE_NAME),
};

DECLARE_CROS_GPIOS(cros_gpios);
