/*
 * This file is part of the coreboot project.
 *
 * Copyright 2007 Corey Osgood <corey.osgood@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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

/* Ported from v2 driver */

#include <io.h>
#include <lib.h>
#include <device/device.h>
#include <device/pnp.h>
#include <console.h>
#include <string.h>
#include <uart8250.h>
#include <statictree.h>
#include "f71805f.h"

void f71805f_pnp_set_resources(struct device *dev);
void f71805f_pnp_set_resources(struct device *dev);
void f71805f_pnp_enable_resources(struct device *dev);
void f71805f_pnp_enable(struct device *dev);
static void f71805f_init(struct device *dev);

static void pnp_enter_conf_state(struct device *dev);
static void pnp_exit_conf_state(struct device *dev);

static void pnp_enter_conf_state(struct device *dev) 
{
	outb(0x87, dev->path.pnp.port);
}

static void pnp_exit_conf_state(struct device *dev) 
{
	outb(0xaa, dev->path.pnp.port);
}

void f71805f_pnp_set_resources(struct device *dev)
{
	pnp_enter_conf_state(dev);  
	pnp_set_resources(dev);
	pnp_exit_conf_state(dev);  
}       

void f71805f_pnp_enable_resources(struct device *dev)
{       
	pnp_enter_conf_state(dev);
	pnp_enable_resources(dev);
	pnp_exit_conf_state(dev);
}

void f71805f_pnp_enable(struct device *dev)
{
	pnp_enter_conf_state(dev);   
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, dev->enabled);
	pnp_exit_conf_state(dev);  
}

static void f71805f_init(struct device *dev)
{
	//struct superio_fintek_f71805f_config *conf = dev->device_configuration;
	struct resource *res0, *res1;

	if (!dev->enabled)
		return;
	
	switch (dev->path.pnp.device) {
	case F71805F_SP1: 
		res0 = find_resource(dev, PNP_IDX_IO0);
		//TODO: needed? fix or remove?
		//init_uart8250(res0->base, &conf->sp1);
		break;
		
	case F71805F_SP2:
		res1 = find_resource(dev, PNP_IDX_IO0);
		//init_uart8250(res0->base, &conf->sp2);
		break;
		
	/* No KBC on F71805f */
	}
}

static void phase2_setup_scan_bus(struct device *dev);

struct device_operations f71805f_ops = {
	.phase2_setup_scan_bus   = phase2_setup_scan_bus,
	.phase4_read_resources   = pnp_read_resources,
	.phase4_set_resources    = f71805f_pnp_set_resources,
	.phase4_enable_disable   = f71805f_pnp_enable_resources,
	.phase5_enable_resources = f71805f_pnp_enable,
	.phase6_init             = f71805f_init,
};

static struct pnp_info pnp_dev_info[] = {
	{ &f71805f_ops, F71805F_SP1,  PNP_IO0 | PNP_IRQ0, { 0x7f8, 0 }, },
	{ &f71805f_ops, F71805F_SP2,  PNP_IO0 | PNP_IRQ0, { 0x7f8, 0 }, },
	/* TODO: Everything else */
};

static void phase2_setup_scan_bus(struct device *dev)
{
	pnp_enable_devices(dev, &f71805f_ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}
