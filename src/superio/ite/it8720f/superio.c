/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <device/pnp.h>
#include <pc80/keyboard.h>
#include <superio/ite/common/env_ctrl.h>
#include <superio/conf_mode.h>
#include <types.h>

#include "chip.h"
#include "it8720f.h"

static void it8720f_init(struct device *dev)
{
	const struct superio_ite_it8720f_config *conf;
	const struct resource *res;

	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	case IT8720F_EC:
		conf = dev->chip_info;
		res = probe_resource(dev, PNP_IDX_IO0);
		if (!conf || !res)
			break;
		ite_ec_init(res->base, &conf->ec);
		ite_ec_set_power_state(dev);
		break;
	case IT8720F_KBCK:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	default:
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_enable,
	.init             = it8720f_init,
	.ops_pnp_mode     = &pnp_conf_mode_870155_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, IT8720F_FDC,
		PNP_IO0 | PNP_IRQ0 | PNP_DRQ0 | PNP_MSC0 | PNP_MSC1,
		0x0ff8, },
	{ NULL, IT8720F_SP1,
		PNP_IO0 | PNP_IRQ0 | PNP_MSC0 | PNP_MSC1,
		0x0ff8, },
	{ NULL, IT8720F_SP2,
		PNP_IO0 | PNP_IRQ0 | PNP_MSC0 | PNP_MSC1,
		0x0ff8, },
	{ NULL, IT8720F_PP,
		PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_DRQ0 | PNP_MSC0,
		0x0ff8, 0x0ffc, },
	{ NULL, IT8720F_EC,
		PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_MSC0 | PNP_MSC1 | PNP_MSC2 |
		PNP_MSC3 | PNP_MSC4 | PNP_MSC5 | PNP_MSC6,
		0x0ff8, 0x0ffc, },
	{ NULL, IT8720F_KBCK,
		PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_MSC0,
		0x0fff, 0x0fff, },
	{ NULL, IT8720F_KBCM,
		PNP_IRQ0 | PNP_MSC0, },
	{ NULL, IT8720F_GPIO,
		PNP_IO0 | PNP_IO1 | PNP_IO2 | PNP_MSC0 | PNP_MSC1 | PNP_MSC2 |
		PNP_MSC3 | PNP_MSC4 | PNP_MSC5 | PNP_MSC6 | PNP_MSC7 |
		PNP_MSC8 | PNP_MSC9 | PNP_MSCA | PNP_MSCB | PNP_MSCD | PNP_MSCE,
		0x0ff8, 0x0ff8, 0x0ff8, },
	{ NULL, IT8720F_CIR,
		PNP_IO0 | PNP_IRQ0 | PNP_MSC0,
		0x0ff8, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_ite_it8720f_ops = {
	.name = "ITE IT8720F Super I/O",
	.enable_dev = enable_dev,
};
