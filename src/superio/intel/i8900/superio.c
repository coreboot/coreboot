/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Arastra, Inc.
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

#include <stdlib.h>
#include <device/device.h>
#include <device/pnp.h>
#include <drivers/uart/uart8250reg.h>
#include "i8900.h"
#include <arch/io.h>

static void pnp_enter_ext_func_mode(struct device *dev)
{
	outb(0x80, dev->path.pnp.port);
	outb(0x86, dev->path.pnp.port);
}

static void pnp_exit_ext_func_mode(struct device *dev)
{
	outb(0x68, dev->path.pnp.port);
	outb(0x08, dev->path.pnp.port);
}

static void i8900_init(struct device *dev)
{
	if (!dev->enabled)
		return;
}

static void i8900_pnp_set_resources(struct device *dev)
{
	pnp_enter_ext_func_mode(dev);
	pnp_set_resources(dev);
	pnp_exit_ext_func_mode(dev);
}

static void i8900_pnp_enable_resources(struct device *dev)
{
	pnp_enter_ext_func_mode(dev);
	pnp_enable_resources(dev);
	pnp_exit_ext_func_mode(dev);
}

static void i8900_pnp_enable(struct device *dev)
{
	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, !!dev->enabled);
	pnp_exit_ext_func_mode(dev);
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = i8900_pnp_set_resources,
	.enable_resources = i8900_pnp_enable_resources,
	.enable           = i8900_pnp_enable,
	.init             = i8900_init,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, I8900_SP1, PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, I8900_SP2, PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, I8900_WDT, PNP_IO0 | PNP_IRQ0, 0x07f8, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_intel_i8900_ops = {
	CHIP_NAME("Intel 8900 Super I/O")
	.enable_dev = enable_dev,
};
