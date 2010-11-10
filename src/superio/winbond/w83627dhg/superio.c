/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
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
#include <uart8250.h>
#include <pc80/keyboard.h>
#include <stdlib.h>
#include "chip.h"
#include "w83627dhg.h"

static void pnp_enter_ext_func_mode(device_t dev)
{
	outb(0x87, dev->path.pnp.port);
	outb(0x87, dev->path.pnp.port);
}

static void pnp_exit_ext_func_mode(device_t dev)
{
	outb(0xaa, dev->path.pnp.port);
}

static void w83627dhg_init(device_t dev)
{
	struct superio_winbond_w83627dhg_config *conf = dev->chip_info;
	struct resource *res0, *res1;

	if (!dev->enabled)
		return;

	switch(dev->path.pnp.device) {
	case W83627DHG_SP1:
		res0 = find_resource(dev, PNP_IDX_IO0);
		init_uart8250(res0->base, &conf->com1);
		break;
	case W83627DHG_SP2:
		res0 = find_resource(dev, PNP_IDX_IO0);
		init_uart8250(res0->base, &conf->com2);
		break;
	case W83627DHG_KBC:
		res0 = find_resource(dev, PNP_IDX_IO0);
		res1 = find_resource(dev, PNP_IDX_IO1);
		pc_keyboard_init(&conf->keyboard);
		break;
	}
}

static void w83627dhg_pnp_set_resources(device_t dev)
{
	pnp_enter_ext_func_mode(dev);
	pnp_set_resources(dev);
	pnp_exit_ext_func_mode(dev);
}

static void w83627dhg_pnp_enable_resources(device_t dev)
{
	pnp_enter_ext_func_mode(dev);
	pnp_enable_resources(dev);
	pnp_exit_ext_func_mode(dev);
}

static void w83627dhg_pnp_enable(device_t dev)
{
	if (!dev->enabled)
		return;

	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_exit_ext_func_mode(dev);
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = w83627dhg_pnp_set_resources,
	.enable_resources = w83627dhg_pnp_enable_resources,
	.enable           = w83627dhg_pnp_enable,
	.init             = w83627dhg_init,
};

static struct pnp_info pnp_dev_info[] = {
	{ &ops, W83627DHG_FDC, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, { 0x0ff8, 0}, },
	{ &ops, W83627DHG_PP,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, { 0x0ff8, 0}, },
	{ &ops, W83627DHG_SP1, PNP_IO0 | PNP_IRQ0, { 0xff8, 0 }, },
	{ &ops, W83627DHG_SP2, PNP_IO0 | PNP_IRQ0, { 0xff8, 0 }, },
	{ &ops, W83627DHG_KBC, PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1, { 0xfff, 0 }, { 0xfff, 0x4}, },
	/* the next line makes coreboot hang in pnp_enable_devices() */
	/* { &ops, W83627DHG_SPI, PNP_IO1, { 0xff8, 0 }, }, */
	{ &ops, W83627DHG_GPIO6, },
	{ &ops, W83627DHG_WDTO_PLED, },
	{ &ops, W83627DHG_GPIO2345, },
	{ &ops, W83627DHG_ACPI, },
	{ &ops, W83627DHG_HWM, PNP_IO0 | PNP_IRQ0, { 0xffe, 0 }, },
	{ &ops, W83627DHG_PECI_SST, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_winbond_w83627dhg_ops = {
	CHIP_NAME("Winbond W83627DHG Super I/O")
	.enable_dev = enable_dev,
};
