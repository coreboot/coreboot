/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2006 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * Copyright (C) 2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
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

#include <device/device.h>
#include <device/pnp.h>
#include <console/console.h>
#include <uart8250.h>
#include <pc80/keyboard.h>
#include "chip.h"
#include "it8716f.h"
#include <arch/io.h>

/* Base address 0x2e: 0x87 0x01 0x55 0x55. */
/* Base address 0x4e: 0x87 0x01 0x55 0xaa. */
static void pnp_enter_ext_func_mode(device_t dev)
{
	outb(0x87, dev->path.u.pnp.port);
	outb(0x01, dev->path.u.pnp.port);
	outb(0x55, dev->path.u.pnp.port);

	if (dev->path.u.pnp.port == 0x4e) {
		outb(0xaa, dev->path.u.pnp.port);
	} else {
		outb(0x55, dev->path.u.pnp.port);
	}
}

static void pnp_exit_ext_func_mode(device_t dev)
{
	pnp_write_config(dev, 0x02, 0x02);
}

static void pnp_write_index(unsigned long port_base, uint8_t reg, uint8_t value)
{
	outb(reg, port_base);
	outb(value, port_base + 1);
}

static uint8_t pnp_read_index(unsigned long port_base, uint8_t reg)
{
	outb(reg, port_base);
	return inb(port_base + 1);
}

static void init_ec(unsigned long base)
{
	uint8_t value;

	// Read out current value of FAN_CTL Control register (0x14)
	value = pnp_read_index(base, 0x14);
	printk_debug("FAN_CTL: reg = 0x%04x, read value = 0x%02x\r\n",base + 0x14, value);
	
	// Set FAN_CTL Control register (0x14) polarity to High, and activate fans 1, 2 and 3
	pnp_write_index(base, 0x14, value | 0x87);
	printk_debug("FAN_CTL: reg = 0x%04x, writing value = 0x%02x\r\n",base + 0x14, value | 0x87);
}

static void it8716f_init(device_t dev)
{
	struct superio_ite_it8716f_config *conf;
	struct resource *res0, *res1;

	if (!dev->enabled) {
		return;
	}

	conf = dev->chip_info;

	/* TODO: FDC, PP, KBCM, MIDI, GAME, IR. */
	switch (dev->path.u.pnp.device) {
	case IT8716F_SP1:
		res0 = find_resource(dev, PNP_IDX_IO0);
		init_uart8250(res0->base, &conf->com1);
		break;
	case IT8716F_SP2:
		res0 = find_resource(dev, PNP_IDX_IO0);
		init_uart8250(res0->base, &conf->com2);
		break;
	case IT8716F_EC:
		res0 = find_resource(dev, PNP_IDX_IO0);
#define EC_INDEX_PORT 5
		init_ec(res0->base + EC_INDEX_PORT);
		break;
	case IT8716F_KBCK:
		res0 = find_resource(dev, PNP_IDX_IO0);
		res1 = find_resource(dev, PNP_IDX_IO1);
		init_pc_keyboard(res0->base, res1->base, &conf->keyboard);
		break;
	}
}

static void it8716f_pnp_set_resources(device_t dev)
{
	pnp_enter_ext_func_mode(dev);
	pnp_set_resources(dev);
	pnp_exit_ext_func_mode(dev);
}

static void it8716f_pnp_enable_resources(device_t dev)
{
	pnp_enter_ext_func_mode(dev);
	pnp_enable_resources(dev);
	pnp_exit_ext_func_mode(dev);
}

static void it8716f_pnp_enable(device_t dev)
{
	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, dev->enabled);
	pnp_exit_ext_func_mode(dev);
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = it8716f_pnp_set_resources,
	.enable_resources = it8716f_pnp_enable_resources,
	.enable           = it8716f_pnp_enable,
	.init             = it8716f_init,
};

static struct pnp_info pnp_dev_info[] = {
	{ &ops, IT8716F_FDC, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, { 0x07f8, 0}, },
	{ &ops, IT8716F_SP1, PNP_IO0 | PNP_IRQ0, { 0x7f8, 0 }, },
	{ &ops, IT8716F_SP2, PNP_IO0 | PNP_IRQ0, { 0x7f8, 0 }, },
	{ &ops, IT8716F_PP, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, { 0x07f8, 0}, },
	{ &ops, IT8716F_EC, PNP_IO0 | PNP_IO1 | PNP_IRQ0, { 0x7f8, 0 }, { 0x7f8, 0x4}, },
	{ &ops, IT8716F_KBCK, PNP_IO0 | PNP_IO1 | PNP_IRQ0, { 0x7ff, 0 }, { 0x7ff, 0x4}, },
	{ &ops, IT8716F_KBCM, PNP_IRQ0, },
	// No 7 { 0,},
	{ &ops, IT8716F_MIDI, PNP_IO0 | PNP_IRQ0, {0x7fe, 0x4}, },
	{ &ops, IT8716F_GAME, PNP_IO0, { 0x7ff, 0 }, },
	{ &ops, IT8716F_IR, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops,
		sizeof(pnp_dev_info)/sizeof(pnp_dev_info[0]), pnp_dev_info);
}

struct chip_operations superio_ite_it8716f_ops = {
	CHIP_NAME("ITE IT8716F Super I/O")
	.enable_dev = enable_dev,
};

