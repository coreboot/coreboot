/*
 * This file is part of the coreboot project.
 *
 *  Copyright 2000  AG Electronics Ltd.
 * Copyright 2003-2004 Linux Networx
 * Copyright 2004 Tyan 
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

#include <io.h>
#include <lib.h>
#include <device/device.h>
#include <device/pnp.h>
#include <console.h>
#include <string.h>
//#include <bitops.h>
#include <uart8250.h>
#include <keyboard.h>
// #include <pc80/mc146818rtc.h>
#include <statictree.h>
#include "w83627hf.h"

static void w83627thg_enter_ext_func_mode(struct device * dev) 
{
        outb(0x87, dev->path.u.pnp.port);
        outb(0x87, dev->path.u.pnp.port);
}
static void w83627thg_exit_ext_func_mode(struct device * dev) 
{
        outb(0xaa, dev->path.u.pnp.port);
}

static void w83627thg_init(struct device * dev)
{
	struct superio_winbond_w83627thg_config *conf;
	struct resource *res0, *res1;
	/* Wishlist handle well known programming interfaces more
	 * generically.
	 */
	if (!dev->enabled) {
		return;
	}
	conf = dev->device_configuration;
	switch(dev->path.u.pnp.device) {
	case W83627THG_SP1: 
		res0 = find_resource(dev, PNP_IDX_IO0);
		init_uart8250(res0->base, &conf->com1);
		break;
	case W83627THG_SP2:
		res0 = find_resource(dev, PNP_IDX_IO0);
		init_uart8250(res0->base, &conf->com2);
		break;
	case W83627THG_KBC:
		res0 = find_resource(dev, PNP_IDX_IO0);
		res1 = find_resource(dev, PNP_IDX_IO1);
		init_pc_keyboard(res0->base, res1->base, &conf->keyboard);
		break;
	}
}

static void w83627thg_set_resources(struct device * dev)
{
	w83627thg_enter_ext_func_mode(dev);
	pnp_set_resources(dev);
	w83627thg_exit_ext_func_mode(dev);
}

static void w83627thg_enable_resources(struct device * dev)
{
	w83627thg_enter_ext_func_mode(dev);
	pnp_enable_resources(dev);
	w83627thg_exit_ext_func_mode(dev);
}

static void w83627thg_enable(struct device * dev)
{
	w83627thg_enter_ext_func_mode(dev);   
	pnp_enable(dev);
	w83627thg_exit_ext_func_mode(dev);  
}

static void phase3_chip_setup_dev(struct device *dev);
static struct device_operations w83627thg_ops = {
	.phase3_chip_setup_dev   = phase3_chip_setup_dev,
	.phase3_enable           = w83627thg_enable,
	.phase4_read_resources   = pnp_read_resources,
	.phase4_set_resources    = w83627thg_set_resources,
	.phase5_enable_resources = w83627thg_enable_resources,
	.enable           = ,
	.init             = w83627thg_init,
};

static struct pnp_info pnp_dev_info[] = {
        { &ops, W83627THG_FDC,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, { 0x07f8, 0}, },
        { &ops, W83627THG_PP,   PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, { 0x07f8, 0}, },
        { &ops, W83627THG_SP1,  PNP_IO0 | PNP_IRQ0, { 0x7f8, 0 }, },
        { &ops, W83627THG_SP2,  PNP_IO0 | PNP_IRQ0, { 0x7f8, 0 }, },
        // No 4 { 0,},
        { &ops, W83627THG_KBC,  PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1, { 0x7ff, 0 }, { 0x7ff, 0x4}, },
        { &ops, W83627THG_GAME_MIDI_GPIO1, PNP_IO0 | PNP_IO1 | PNP_IRQ0, { 0x7ff, 0 }, {0x7fe, 4} },
        { &ops, W83627THG_GPIO2,},
        { &ops, W83627THG_GPIO3,},
        { &ops, W83627THG_ACPI, PNP_IRQ0,  },
        { &ops, W83627THG_HWM,  PNP_IO0 | PNP_IRQ0, { 0xff8, 0 } },
};

static void phase3_chip_setup_dev(struct device *dev)
{
	pnp_enable_devices(dev, &w83627thg_ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}
