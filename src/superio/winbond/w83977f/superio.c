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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include <device/device.h>
#include <device/pnp.h>
#include <superio/conf_mode.h>
#include <console/console.h>
#include <string.h>
#include <uart8250.h>
#include <pc80/keyboard.h>
#include <stdlib.h>
#include "chip.h"
#include "w83977f.h"

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

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_enable,
	.init             = w83977f_init,
	.ops_pnp_mode     = &pnp_conf_mode_8787_aa,
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
