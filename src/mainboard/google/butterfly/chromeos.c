/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <device/device.h>

#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>
#include <ec/quanta/ene_kb3940q/ec.h>
#include <types.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include "ec.h"
#include "onboard.h"

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		/* lid switch value from EC */
		{-1, ACTIVE_HIGH, get_lid_switch(), "lid"},

		/* Power Button - Hardcode Low as power button may still be
		 * pressed when read here.*/
		{-1, ACTIVE_HIGH, 0, "power"},

		/* Was VGA Option ROM loaded? */
		/* -1 indicates that this is a pseudo GPIO */
		{-1, ACTIVE_HIGH, gfx_get_init_done(), "oprom"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int get_write_protect_state(void)
{
	return !get_gpio(WP_GPIO);
}

int get_lid_switch(void)
{
	return (ec_mem_read(EC_HW_GPI_STATUS) >> EC_GPI_LID_STAT_BIT) & 1;
}

/* FIXME: VBOOT reads this in ENV_ROMSTAGE. */
int get_recovery_mode_switch(void)
{
	if (ENV_RAMSTAGE)
		return (ec_mem_read(EC_CODE_STATE) == EC_COS_EC_RO);

	return 0;
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AH(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AL(WP_GPIO, CROS_GPIO_DEVICE_NAME),
};

const struct cros_gpio *variant_cros_gpios(size_t *num)
{
	*num = ARRAY_SIZE(cros_gpios);
	return cros_gpios;
}

int get_ec_is_trusted(void)
{
	/* Do not have a Chrome EC involved in entering recovery mode;
	   Always return trusted. */
	return 1;
}
