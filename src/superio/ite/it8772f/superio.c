/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <device/pnp.h>
#include <pc80/keyboard.h>
#include <superio/ite/common/env_ctrl.h>
#include <arch/io.h>
#include <delay.h>
#include <superio/conf_mode.h>
#include <superio/hwm5_conf.h>

#include "chip.h" /* FIXME */
#include "it8772f.h"

static void it8772f_init(struct device *dev)
{
	struct superio_ite_it8772f_config *conf = dev->chip_info;
	struct resource *res;

	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	case IT8772F_EC:
		res = probe_resource(dev, PNP_IDX_IO0);
		if (!conf || !res)
			break;
		ite_ec_init(res->base, &conf->ec);
		break;
	case IT8772F_GPIO:
		/* Set GPIO output levels */
		res = probe_resource(dev, PNP_IDX_IO1);
		if (res) {
			if (conf->gpio_set1)
				outb(conf->gpio_set1, res->base + 0);
			if (conf->gpio_set2)
				outb(conf->gpio_set2, res->base + 1);
			if (conf->gpio_set3)
				outb(conf->gpio_set3, res->base + 2);
			if (conf->gpio_set4)
				outb(conf->gpio_set4, res->base + 3);
			if (conf->gpio_set5)
				outb(conf->gpio_set5, res->base + 4);
			if (conf->gpio_set6)
				outb(conf->gpio_set6, res->base + 5);
		}
		break;
	case IT8772F_KBCK:
		if (!conf->skip_keyboard) {
			set_kbc_ps2_mode();
			pc_keyboard_init(NO_AUX_DEVICE);
		}
		break;
	case IT8772F_KBCM:
		break;
	case IT8772F_IR:
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = it8772f_init,
	.ops_pnp_mode     = &pnp_conf_mode_870155_aa,
};

static struct pnp_info pnp_dev_info[] = {
	/* Floppy Disk Controller */
	{ NULL, IT8772F_FDC, PNP_IO0 | PNP_IRQ0, 0x0ff8, },
	/* Serial Port 1 */
	{ NULL, IT8772F_SP1, PNP_IO0 | PNP_IRQ0, 0x0ff8, },
	/* Environmental Controller */
	{ NULL, IT8772F_EC, PNP_IO0 | PNP_IO1 | PNP_IRQ0 |
		PNP_MSC4 | PNP_MSCA,
	  0x0ff8, 0x0ffc, },
	/* KBC Keyboard */
	{ NULL, IT8772F_KBCK, PNP_IO0 | PNP_IO1 | PNP_IRQ0,
	  0x0fff, 0x0fff, },
	/* KBC Mouse */
	{ NULL, IT8772F_KBCM, PNP_IRQ0, },
	/* 27 GPIOs */
	{ NULL, IT8772F_GPIO, PNP_IO0 | PNP_IO1,
	  0x0fff, 0x0ff8, },
	/* Infrared */
	{ NULL, IT8772F_IR, PNP_IO0 | PNP_IRQ0, 0x0ff8, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_ite_it8772f_ops = {
	.name = "ITE IT8772F Super I/O",
	.enable_dev = enable_dev,
};
