/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <device/pnp.h>
#include <superio/conf_mode.h>
#include <pc80/keyboard.h>
#include <superio/ite/common/env_ctrl.h>
#include <option.h>

#include "chip.h"
#include "it8728f.h"

#define MAINBOARD_POWER_OFF	0
#define MAINBOARD_POWER_ON	1
#define MAINBOARD_POWER_KEEP	2

static void it8728f_init(struct device *dev)
{
	const struct superio_ite_it8728f_config *conf = dev->chip_info;
	const struct resource *res;
	uint8_t power_status;
	uint8_t byte_f2, byte_f4;

	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	/* TODO: Might potentially need code for FDC etc. */
	case IT8728F_EC:
		res = probe_resource(dev, PNP_IDX_IO0);
		if (!conf || !res)
			break;
		ite_ec_init(res->base, &conf->ec);

		/* Set power state after power fail */
		power_status = get_uint_option("power_on_after_fail",
				CONFIG_MAINBOARD_POWER_FAILURE_STATE);
		pnp_enter_conf_mode(dev);
		pnp_set_logical_device(dev);
		byte_f4 = pnp_read_config(dev, 0xf4);
		byte_f2 = pnp_read_config(dev, 0xf2);
		if (power_status == MAINBOARD_POWER_ON) {
			byte_f4 |= 0x20;
		} else if (power_status == MAINBOARD_POWER_KEEP) {
			byte_f4 &= ~0x20;
			byte_f2 |=  0x20;
		} else {
			byte_f4 &= ~0x20;
			byte_f2 &= ~0x20;
		}
		pnp_write_config(dev, 0xf4, byte_f4);
		pnp_write_config(dev, 0xf2, byte_f2);
		pnp_exit_conf_mode(dev);
		printk(BIOS_INFO, "set power %u after power fail\n", power_status);

		break;
	case IT8728F_KBCK:
		set_kbc_ps2_mode();
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = it8728f_init,
	.ops_pnp_mode     = &pnp_conf_mode_870155_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, IT8728F_FDC, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x0ff8, },
	{ NULL, IT8728F_SP1, PNP_IO0 | PNP_IRQ0, 0x0ff8, },
	{ NULL, IT8728F_SP2, PNP_IO0 | PNP_IRQ0, 0x0ff8, },
	{ NULL, IT8728F_PP, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x0ffc, },
	{ NULL, IT8728F_EC, PNP_IO0 | PNP_IO1 | PNP_IRQ0, 0x0ff8, 0x0ff8, },
	{ NULL, IT8728F_KBCK, PNP_IO0 | PNP_IO1 | PNP_IRQ0, 0x0fff, 0x0fff, },
	{ NULL, IT8728F_KBCM, PNP_IRQ0, },
	{ NULL, IT8728F_GPIO, PNP_IO0 | PNP_IO1 | PNP_IO2 | PNP_IRQ0,
		0x0fff, 0x0ff8, 0x0ff8, },
	{ NULL, IT8728F_IR, PNP_IO0 | PNP_IRQ0, 0x0ff8, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_ite_it8728f_ops = {
	.name = "ITE IT8728F Super I/O",
	.enable_dev = enable_dev
};
