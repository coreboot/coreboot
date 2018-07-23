/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
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
#include <pc80/keyboard.h>
#include <stdlib.h>
#include "pc87309.h"

static void init(struct device *dev)
{

	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	case PC87309_KBCK:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_enable,
	.init             = init,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, PC87309_FDC,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x07fa, },
	{ NULL, PC87309_PP,   PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x04f8, },
	{ NULL, PC87309_SP2,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0 | PNP_DRQ1,
		0x07f8, },
	{ NULL, PC87309_SP1,  PNP_IO0 | PNP_IRQ0, 0x07f8, },
	/* TODO: PM. */
	{ NULL, PC87309_KBCM, PNP_IRQ0, },
	{ NULL, PC87309_KBCK, PNP_IO0 | PNP_IO1 | PNP_IRQ0, 0x07f8, 0x7f8, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_nsc_pc87309_ops = {
	CHIP_NAME("NSC PC87309 Super I/O")
	.enable_dev = enable_dev,
};
