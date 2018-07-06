/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Corey Osgood <corey.osgood@gmail.com>
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

#include <arch/io.h>
#include <device/device.h>
#include <device/pnp.h>
#include <superio/conf_mode.h>
#include <console/console.h>
#include <stdlib.h>
#include <pc80/keyboard.h>
#include "f71872.h"

static void f71872_init(struct device *dev)
{

	if (!dev->enabled)
		return;

	switch(dev->path.pnp.device) {
	/* TODO: Might potentially need code for HWM or FDC etc. */
	case F71872_KBC:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = f71872_init,
	.ops_pnp_mode     = &pnp_conf_mode_8787_aa,
};

static struct pnp_info pnp_dev_info[] = {
	/* TODO: Some of the 0x07f8 etc. values may not be correct. */
	{ NULL, F71872_FDC,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x07f8, },
	{ NULL, F71872_SP1,  PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, F71872_SP2,  PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, F71872_PP,   PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x07f8, },
	{ NULL, F71872_HWM,  PNP_IO0 | PNP_IRQ0, 0x0ff8, },
	{ NULL, F71872_KBC,  PNP_IO0 | PNP_IRQ0 | PNP_IRQ1, 0x07ff, },
	{ NULL, F71872_GPIO, PNP_IRQ0, },
	{ NULL, F71872_VID,  PNP_IO0, 0x0ff8, },
	{ NULL, F71872_PM, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_fintek_f71872_ops = {
	CHIP_NAME("Fintek F71872 Super I/O")
	.enable_dev = enable_dev
};
