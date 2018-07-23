/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
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
#include <console/console.h>
#include <string.h>
#include <pc80/keyboard.h>
#include <stdlib.h>
#include "pc87382.h"

static void init(struct device *dev)
{
	if (!dev->enabled)
		return;

	switch(dev->path.pnp.device) {
	case PC87382_DOCK:
		break;

	case PC87382_GPIO:
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
	{ NULL, PC87382_IR,   PNP_IO0 | PNP_IRQ0 | PNP_DRQ0 | PNP_DRQ1,
		0x07f8 },
	{ NULL, PC87382_SP1,  PNP_IO0 | PNP_IRQ0, 0x07f8 },
	{ NULL, PC87382_GPIO, PNP_IO0 | PNP_IRQ0, 0xfff0 },
	{ NULL, PC87382_DOCK, PNP_IO0 | PNP_IRQ0, 0xfffe },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_nsc_pc87382_ops = {
	CHIP_NAME("NSC PC87382 Docking LPC Switch")
	.enable_dev = enable_dev,
};
