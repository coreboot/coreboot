/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 secunet Security Networks AG
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
#include <superio/conf_mode.h>
#include <superio/ite/common/env_ctrl.h>

#include "it8783ef.h"
#include "chip.h"

static void it8783ef_init(struct device *const dev)
{
	const struct superio_ite_it8783ef_config *conf;
	const struct resource *res;

	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	case IT8783EF_EC:
		conf = dev->chip_info;
		res = find_resource(dev, PNP_IDX_IO0);
		if (!conf || !res)
			break;
		ite_ec_init(res->base, &conf->ec);
		break;
	case IT8783EF_KBCK:
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
	.enable           = pnp_alt_enable,
	.init             = it8783ef_init,
	.ops_pnp_mode     = &pnp_conf_mode_870155_aa,
};

static struct pnp_info pnp_dev_info[] = {
	/* Floppy Disk Controller */
	{ NULL, IT8783EF_FDC, PNP_IO0 | PNP_IRQ0 | PNP_MSC0 | PNP_MSC1,
	  0x0ff8, },
	/* Serial Port 1 */
	{ NULL, IT8783EF_SP1, PNP_IO0 | PNP_IRQ0 | PNP_MSC0, 0x0ff8, },
	/* Serial Port 2 */
	{ NULL, IT8783EF_SP2, PNP_IO0 | PNP_IRQ0 | PNP_MSC0, 0x0ff8, },
	/* Printer Port */
	{ NULL, IT8783EF_PP,  PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_DRQ0 |
				PNP_MSC0,
	  0x0ffc, 0x0ffc, },
	/* Environmental Controller */
	{ NULL, IT8783EF_EC,  PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_MSC0 |
				PNP_MSC1 | PNP_MSC2 | PNP_MSC3 | PNP_MSC4 |
				PNP_MSC5 | PNP_MSC6 | PNP_MSC7,
	  0x0ff8, 0x0ff8, },
	/* KBC Keyboard */
	{ NULL, IT8783EF_KBCK, PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_MSC0,
	  0x0fff, 0x0fff, },
	/* KBC Mouse */
	{ NULL, IT8783EF_KBCM, PNP_IRQ0 | PNP_MSC0, },
	/* GPIO */
	{ NULL, IT8783EF_GPIO, PNP_IO0 | PNP_IO1 | PNP_IO2 | PNP_IRQ0 |
				PNP_MSC0 | PNP_MSC1 | PNP_MSC2 | PNP_MSC3 |
				PNP_MSC4 | PNP_MSC5 | PNP_MSC6 | PNP_MSC7 |
				PNP_MSC8 | PNP_MSC9 | PNP_MSCA | PNP_MSCB,
	  0x0ffc, 0x0fff, 0x0ff8, },
	/* Serial Port 3 */
	{ NULL, IT8783EF_SP3, PNP_IO0 | PNP_IRQ0 | PNP_MSC0, 0x0ff8, },
	/* Serial Port 4 */
	{ NULL, IT8783EF_SP4, PNP_IO0 | PNP_IRQ0 | PNP_MSC0, 0x0ff8, },
	/* Serial Port 5 */
	{ NULL, IT8783EF_SP5, PNP_IO0 | PNP_IRQ0 | PNP_MSC0, 0x0ff8, },
	/* Serial Port 6 */
	{ NULL, IT8783EF_SP6, PNP_IO0 | PNP_IRQ0 | PNP_MSC0, 0x0ff8, },
	/* Consumer Infrared */
	{ NULL, IT8783EF_CIR, PNP_IO0 | PNP_IRQ0 | PNP_MSC0, 0x0ff8, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_ite_it8783ef_ops = {
	CHIP_NAME("ITE IT8783E/F Super I/O")
	.enable_dev = enable_dev,
};
