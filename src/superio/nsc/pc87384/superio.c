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
#include "pc87384.h"

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_enable,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, PC87384_PP,   PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x04f8 },
	{ NULL, PC87384_SP1,  PNP_IO0 | PNP_IRQ0, 0x07f8 },
	{ NULL, PC87384_SP2,  PNP_IO0 | PNP_IRQ0, 0x07f8 },
	{ NULL, PC87384_GPIO, PNP_IO0 | PNP_IRQ0, 0xfff0 },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_nsc_pc87384_ops = {
	CHIP_NAME("NSC PC87384 Super I/O")
	.enable_dev = enable_dev,
};
