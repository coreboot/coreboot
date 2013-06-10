/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <arch/io.h>
#include <device/pnp.h>
#include <stdlib.h>
#include "nct5104d.h"

static void pnp_enter_exteded_mode(device_t dev)
{
	outb(0x87,dev->path.pnp.port);
	outb(0x87,dev->path.pnp.port);
}

static void pnp_exit_extended_mode(device_t dev)
{
	outb(0xaa,dev->path.pnp.port);
}

static void nct5104d_init(device_t dev)
{
}

static void nct5104d_pnp_set_resources(device_t dev)
{
	pnp_enter_exteded_mode(dev);
	pnp_set_resources(dev);
	pnp_exit_extended_mode(dev);
}

static void nct5104d_pnp_enable_resources(device_t dev)
{
	pnp_enter_exteded_mode(dev);
	pnp_enable_resources(dev);
	pnp_exit_extended_mode(dev);
}

static void nct5104d_pnp_enable(device_t dev)
{
	pnp_enter_exteded_mode(dev);
	pnp_alt_enable(dev);
	pnp_exit_extended_mode(dev);
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = nct5104d_pnp_set_resources,
	.enable_resources = nct5104d_pnp_enable_resources,
	.enable           = nct5104d_pnp_enable,
	.init             = nct5104d_init,
};

static struct pnp_info pnp_dev_info[] = {
	{ &ops, NCT5104D_SP2,  PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, NCT5104D_SP1,  PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, NCT5104D_GPIO_WDT},
	{ &ops, NCT5104D_GPIO_PP_OD},
	{ &ops, NCT5104D_GPIO0},
	{ &ops, NCT5104D_GPIO1},
	{ &ops, NCT5104D_GPIO6},
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_nuvoton_nct5104d_ops = {
	CHIP_NAME("NUVOTON NCT5104D Super I/O")
	.enable_dev = enable_dev,
};
