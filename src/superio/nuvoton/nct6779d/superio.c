/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <device/pnp.h>
#include <option.h>
#include <pc80/keyboard.h>
#include <superio/conf_mode.h>

#include "nct6779d.h"

/* C code shared by NCT6779D and NCT5535D */

#define MAINBOARD_POWER_OFF	0
#define MAINBOARD_POWER_ON	1
#define MAINBOARD_POWER_KEEP	2

static void nct6779d_init(struct device *dev)
{
	uint8_t byte, power_status;

	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	/* TODO: Might potentially need code for HWM or FDC etc. */
	case NCT6779D_KBC:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	case NCT6779D_ACPI:
		/* Set power state after power fail */
		power_status = get_uint_option("power_on_after_fail",
				CONFIG_MAINBOARD_POWER_FAILURE_STATE);
		pnp_enter_conf_mode(dev);
		pnp_set_logical_device(dev);
		byte = pnp_read_config(dev, 0xe4) & ~0x60;
		if (power_status == MAINBOARD_POWER_ON)
			byte |= (MAINBOARD_POWER_ON << 5);
		else if (power_status == MAINBOARD_POWER_KEEP)
			byte |= (MAINBOARD_POWER_KEEP << 5);
		pnp_write_config(dev, 0xe4, byte);
		pnp_exit_conf_mode(dev);
		printk(BIOS_INFO, "set power %s after power fail\n", power_status ? "on" : "off");
		break;
	}
}

struct device_operations _nuvoton_nct6779d_ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = nct6779d_init,
	.ops_pnp_mode     = &pnp_conf_mode_8787_aa,
};
