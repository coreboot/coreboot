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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdlib.h>
#include <device/device.h>
#include <device/pnp.h>
#include <uart8250.h>
#include "i3100.h"
#include <arch/io.h>

static void pnp_enter_ext_func_mode(device_t dev)
{
	outb(0x80, dev->path.pnp.port);
	outb(0x86, dev->path.pnp.port);
}

static void pnp_exit_ext_func_mode(device_t dev)
{
	outb(0x68, dev->path.pnp.port);
	outb(0x08, dev->path.pnp.port);
}

static void i3100_init(device_t dev)
{
	if (!dev->enabled)
		return;
}

static const struct pnp_mode_ops pnp_conf_mode_ops = {
	.enter_conf_mode  = pnp_enter_ext_func_mode,
	.exit_conf_mode   = pnp_exit_ext_func_mode,
};

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = i3100_init,
	.ops_pnp_mode     = &pnp_conf_mode_ops,
};

static struct pnp_info pnp_dev_info[] = {
	{ &ops, I3100_SP1, PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, I3100_SP2, PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_intel_i3100_ops = {
	CHIP_NAME("Intel 3100 Super I/O")
	.enable_dev = enable_dev,
};
