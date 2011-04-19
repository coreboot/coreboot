/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Nikolay Petukhov <nikolay.petukhov@gmail.com>
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
#include <device/device.h>
#include <device/pnp.h>
#include <console/console.h>
#include <string.h>
#include <bitops.h>
#include <uart8250.h>
#include <pc80/keyboard.h>
#include <stdlib.h>
#include "chip.h"
#include "w83977f.h"

static void w83977f_enter_ext_func_mode(device_t dev)
{
	outb(0x87, dev->path.pnp.port);
	outb(0x87, dev->path.pnp.port);
}

static void w83977f_exit_ext_func_mode(device_t dev)
{
	outb(0xaa, dev->path.pnp.port);
}

static void w83977f_init(device_t dev)
{
	struct superio_winbond_w83977f_config *conf = dev->chip_info;

	if (!dev->enabled)
		return;

	switch(dev->path.pnp.device) {
	case W83977F_KBC:
		pc_keyboard_init(&conf->keyboard);
		break;
	}
}

static void w83977f_set_resources(device_t dev)
{
	w83977f_enter_ext_func_mode(dev);
	pnp_set_resources(dev);
	w83977f_exit_ext_func_mode(dev);
}

static void w83977f_enable_resources(device_t dev)
{
	w83977f_enter_ext_func_mode(dev);
	pnp_enable_resources(dev);
	w83977f_exit_ext_func_mode(dev);
}

static void w83977f_enable(device_t dev)
{
	w83977f_enter_ext_func_mode(dev);
	pnp_enable(dev);
	w83977f_exit_ext_func_mode(dev);
}

static struct device_operations ops = {
	.read_resources		= pnp_read_resources,
	.set_resources		= w83977f_set_resources,
	.enable_resources	= w83977f_enable_resources,
	.enable			= w83977f_enable,
	.init			= w83977f_init,
};

static struct pnp_info pnp_dev_info[] = {
	{ &ops, W83977F_FDC,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, {0x07f8, 0}, },
	{ &ops, W83977F_PP,   PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, {0x07f8, 0}, },
	{ &ops, W83977F_SP1,  PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, W83977F_SP2,  PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, W83977F_RTC,  PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, W83977F_KBC,  PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1, {0x07f8, 0}, {0x07f8, 0}, },
	{ &ops, W83977F_IR, PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, W83977F_GPIO1, PNP_IO0, {0x07f8, 0}, },
	{ &ops, W83977F_GPIO2, PNP_IO0, {0x07f8, 0}, },
};

static void enable_dev(device_t dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_winbond_w83977f_ops = {
	CHIP_NAME("Winbond W83977F Super I/O")
	.enable_dev = enable_dev,
};
