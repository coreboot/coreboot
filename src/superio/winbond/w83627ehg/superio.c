/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000 AG Electronics Ltd.
 * Copyright (C) 2003-2004 Linux Networx
 * Copyright (C) 2004 Tyan
 * Copyright (C) 2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
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
#include <console/console.h>
#include <string.h>
#include <bitops.h>
#include <uart8250.h>
#include <pc80/keyboard.h>
#include <pc80/mc146818rtc.h>
#include <stdlib.h>
#include "chip.h"
#include "w83627ehg.h"

static void pnp_enter_ext_func_mode(device_t dev)
{
	outb(0x87, dev->path.pnp.port);
	outb(0x87, dev->path.pnp.port);
}

static void pnp_exit_ext_func_mode(device_t dev)
{
	outb(0xaa, dev->path.pnp.port);
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

static void enable_hwm_smbus(device_t dev)
{
	/* Set the pin 91,92 as I2C bus. */
	uint8_t reg, value;
	reg = 0x2a;
	value = pnp_read_config(dev, reg);
	value |= (1 << 1);
	pnp_write_config(dev, reg, value);
}

static void init_acpi(device_t dev)
{
	uint8_t value = 0x20;
	int power_on = 1;

	get_option(&power_on, "power_on_after_fail");
	pnp_enter_ext_func_mode(dev);
	pnp_write_index(dev->path.pnp.port, 7, 0x0a);
	value = pnp_read_config(dev, 0xe4);
	value &= ~(3 << 5);
	if (power_on) {
		value |= (1 << 5);
	}
	pnp_write_config(dev, 0xe4, value);
	pnp_exit_ext_func_mode(dev);
}

static void init_hwm(unsigned long base)
{
	int i;
	uint8_t reg, value;

	/* reg mask data */
	unsigned hwm_reg_values[] = {
		0x40, 0xff, 0x81, /* Start HWM. */
		0x48, 0x7f, 0x2a, /* Set SMBus base to 0x54 >> 1. */
	};

	for(i = 0; i < ARRAY_SIZE(hwm_reg_values); i += 3) {
		reg = hwm_reg_values[i];
		value = pnp_read_index(base, reg);
		value &= 0xff & (~(hwm_reg_values[i + 1]));
		value |= 0xff & hwm_reg_values[i + 2];
		/* printk_debug("base = 0x%04x, reg = 0x%02x, value = 0x%02x\r\n", base, reg,value); */
		pnp_write_index(base, reg, value);
	}
}

static void w83627ehg_init(device_t dev)
{
	struct superio_winbond_w83627ehg_config *conf;
	struct resource *res0, *res1;
	if (!dev->enabled) {
		return;
	}
	conf = dev->chip_info;
	switch(dev->path.pnp.device) {
	case W83627EHG_SP1:
		res0 = find_resource(dev, PNP_IDX_IO0);
		init_uart8250(res0->base, &conf->com1);
		break;
	case W83627EHG_SP2:
		res0 = find_resource(dev, PNP_IDX_IO0);
		init_uart8250(res0->base, &conf->com2);
		break;
	case W83627EHG_KBC:
		res0 = find_resource(dev, PNP_IDX_IO0);
		res1 = find_resource(dev, PNP_IDX_IO1);
		pc_keyboard_init(&conf->keyboard);
		break;
	case W83627EHG_HWM:
		res0 = find_resource(dev, PNP_IDX_IO0);
#define HWM_INDEX_PORT 5
		init_hwm(res0->base + HWM_INDEX_PORT);
		break;
	case W83627EHG_ACPI:
		init_acpi(dev);
		break;
	}
}

static void w83627ehg_pnp_set_resources(device_t dev)
{
	pnp_enter_ext_func_mode(dev);
	pnp_set_resources(dev);
	pnp_exit_ext_func_mode(dev);
}

static void w83627ehg_pnp_enable_resources(device_t dev)
{
	pnp_enter_ext_func_mode(dev);
	pnp_enable_resources(dev);

	switch (dev->path.pnp.device) {
	case W83627EHG_HWM:
		printk_debug("w83627ehg hwm smbus enabled\n");
		enable_hwm_smbus(dev);
		break;
	}

	pnp_exit_ext_func_mode(dev);
}

static void w83627ehg_pnp_enable(device_t dev)
{
	if (!dev->enabled) {
		pnp_enter_ext_func_mode(dev);
		pnp_set_logical_device(dev);
		pnp_set_enable(dev, 0);
		pnp_exit_ext_func_mode(dev);
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = w83627ehg_pnp_set_resources,
	.enable_resources = w83627ehg_pnp_enable_resources,
	.enable           = w83627ehg_pnp_enable,
	.init             = w83627ehg_init,
};

static struct pnp_info pnp_dev_info[] = {
	{ &ops, W83627EHG_FDC, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, { 0x07f8, 0}, },
	{ &ops, W83627EHG_PP,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, { 0x07f8, 0}, },
	{ &ops, W83627EHG_SP1, PNP_IO0 | PNP_IRQ0, { 0x7f8, 0 }, },
	{ &ops, W83627EHG_SP2, PNP_IO0 | PNP_IRQ0, { 0x7f8, 0 }, },
	/* No 4 { 0,}, */
	{ &ops, W83627EHG_KBC, PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1, { 0x7ff, 0 }, { 0x7ff, 0x4}, },
	{ &ops, W83627EHG_SFI, PNP_IO0 | PNP_IRQ0, { 0x7f8, 0 }, },
	{ &ops, W83627EHG_WDTO_PLED, },
	{ &ops, W83627EHG_ACPI, },
	{ &ops, W83627EHG_HWM, PNP_IO0 | PNP_IRQ0, { 0xff8, 0 }, },
	{ &ops, W83627EHG_GAME, PNP_IO0, { 0x7ff, 0 }, },
	{ &ops, W83627EHG_MIDI, PNP_IO1 | PNP_IRQ0, { 0x7ff, 0 } , {0x7fe, 0x4}, },
	{ &ops, W83627EHG_GPIO1, },
	{ &ops, W83627EHG_GPIO2, },
	{ &ops, W83627EHG_GPIO3, },
	{ &ops, W83627EHG_GPIO4, },
	{ &ops, W83627EHG_GPIO5, },
	{ &ops, W83627EHG_GPIO6, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops,
		ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_winbond_w83627ehg_ops = {
	CHIP_NAME("Winbond W83627EHG Super I/O")
	.enable_dev = enable_dev,
};
