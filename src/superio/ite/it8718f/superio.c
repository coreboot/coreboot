/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/device.h>
#include <device/pnp.h>
#include <pc80/keyboard.h>
#include <stdlib.h>
#include <superio/ite/common/env_ctrl.h>
#include <superio/conf_mode.h>

#include "it8718f.h"
#include "chip.h"

static void init(struct device *dev)
{
	const struct superio_ite_it8718f_config *conf;
	const struct resource *res;

	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	case IT8718F_FDC: /* TODO. */
		break;
	case IT8718F_PP: /* TODO. */
		break;
	case IT8718F_EC:
		conf = dev->chip_info;
		res = find_resource(dev, PNP_IDX_IO0);
		if (!conf || !res)
			break;
		ite_ec_init(res->base, &conf->ec);
		break;
	case IT8718F_KBCK:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	case IT8718F_KBCM: /* TODO. */
		break;
	case IT8718F_IR: /* TODO. */
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_enable,
	.init             = init,
	.ops_pnp_mode     = &pnp_conf_mode_870155_aa,
};

/* TODO: IR. */
static struct pnp_info pnp_dev_info[] = {
	{ NULL, IT8718F_FDC,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0
			| PNP_MSC0 | PNP_MSC1, 0x0ff8, },
	{ NULL, IT8718F_SP1,  PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, IT8718F_SP2,  PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, IT8718F_EC,   PNP_IO0 | PNP_IO1 | PNP_IRQ0
			| PNP_MSC0 | PNP_MSC1 | PNP_MSC2 | PNP_MSC3
			| PNP_MSC4 | PNP_MSC5 | PNP_MSC6,
			0x0ff8, 0x0ff8, },
	{ NULL, IT8718F_KBCK, PNP_IO0 | PNP_IO1 | PNP_IRQ0
			| PNP_MSC0, 0x07f8, 0x07f8, },
	{ NULL, IT8718F_KBCM, PNP_IRQ0 | PNP_MSC0, },
	{ NULL, IT8718F_PP,   PNP_IO0 | PNP_IO1 | PNP_IRQ0
			| PNP_DRQ0 | PNP_MSC0, 0x0ff8, 0x0ff8, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_ite_it8718f_ops = {
	CHIP_NAME("ITE IT8718F Super I/O")
	.enable_dev = enable_dev,
};
