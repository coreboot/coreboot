/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000 AG Electronics Ltd.
 * Copyright (C) 2003-2004 Linux Networx
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
#include <console/console.h>
#include <string.h>
#include <stdlib.h>
#include "chip.h"
#include "pc87427.h"

static void init(device_t dev)
{
	struct superio_nsc_pc87427_config *conf = dev->chip_info;

	if (!dev->enabled)
		return;

	switch(dev->path.pnp.device) {
	case PC87427_KBCK:
		pc_keyboard_init(&conf->keyboard);
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
	{ &ops, PC87427_FDC,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, {0x07fa, 0}, },
	{ &ops, PC87427_SP2,  PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, PC87427_SP1,  PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, PC87427_SWC,  PNP_IO0 | PNP_IO1 | PNP_IO2 | PNP_IO3 | PNP_IRQ0, {0xfff0, 0}, {0xfffc, 0}, {0xfffc, 0}, {0xfff8, 0}, },
	{ &ops, PC87427_KBCM, PNP_IRQ0, },
	{ &ops, PC87427_KBCK, PNP_IO0 | PNP_IO1 | PNP_IRQ0, {0x07f8, 0}, {0x07f8, 4}, },
	{ &ops, PC87427_GPIO, PNP_IO0 | PNP_IRQ0, {0xffe0, 0}, },
	{ &ops, PC87427_WDT,  PNP_IO0 | PNP_IRQ0, {0xfff0, 0}, },
	{ &ops, PC87427_FMC,  PNP_IO0 | PNP_IRQ0, {0xffe0, 0}, },
	{ &ops, PC87427_XBUS, PNP_IO0 | PNP_IRQ0, {0xffe0, 0}, },
	{ &ops, PC87427_RTC,  PNP_IO0 | PNP_IO1 | PNP_IRQ0, {0xfffe, 0}, {0xfffe, 0}, },
	{ &ops, PC87427_MHC,  PNP_IO0 | PNP_IO1 | PNP_IRQ0, {0xffe0, 0}, {0xffe0, 0}, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_nsc_pc87427_ops = {
	CHIP_NAME("NSC PC87427 Super I/O")
	.enable_dev = enable_dev,
};
