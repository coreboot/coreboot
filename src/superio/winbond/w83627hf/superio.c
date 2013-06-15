/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000 AG Electronics Ltd.
 * Copyright (C) 2003-2004 Linux Networx
 * Copyright (C) 2004 Tyan
 * Copyright (C) 2010 Win Enterprises (anishp@win-ent.com)
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
#include <pc80/mc146818rtc.h>
#include <stdlib.h>
#include "chip.h"
#include "w83627hf.h"

static void pnp_write_index(u16 port, u8 reg, u8 value)
{
	outb(reg, port);
	outb(value, port + 1);
}

static u8 pnp_read_index(u16 port, u8 reg)
{
	outb(reg, port);
	return inb(port + 1);
}

static void enable_hwm_smbus(device_t dev)
{
	u8 reg8;

	/* Configure pins 91/92 as SDA/SCL (I2C bus). */
	reg8 = pnp_read_config(dev, 0x2b);
	reg8 &= 0x3f;
	pnp_write_config(dev, 0x2b, reg8);
}

static void init_acpi(device_t dev)
{
	u8 value = 0x20; /* FIXME: The 0x20 value here is never used? */
	int power_on = 1;

	get_option(&power_on, "power_on_after_fail");

	pnp_enter_conf_mode(dev);
	pnp_set_logical_device(dev);
	value = pnp_read_config(dev, 0xE4);
	value &= ~(3 << 5);
	if (power_on)
		value |= (1 << 5);
	pnp_write_config(dev, 0xE4, value);
	pnp_exit_conf_mode(dev);
}

static void init_hwm(u16 base)
{
	u8 reg, value;
	int i;

	u8 hwm_reg_values[] = {
	/*      reg   mask  data */
		0x40, 0xff, 0x81, /* Start HWM. */
		0x48, 0xaa, 0x2a, /* Set SMBus base to 0x2a (0x54 >> 1). */
		0x4a, 0x21, 0x21, /* Set T2 SMBus base to 0x92>>1 and T3 SMBus base to 0x94>>1. */
		0x4e, 0x80, 0x00,
		0x43, 0x00, 0xff,
		0x44, 0x00, 0x3f,
		0x4c, 0xbf, 0x18,
		0x4d, 0xff, 0x80, /* Turn off beep */
	};

	for (i = 0; i < ARRAY_SIZE(hwm_reg_values); i += 3) {
		reg = hwm_reg_values[i];
		value = pnp_read_index(base, reg);
		value &= 0xff & hwm_reg_values[i + 1];
		value |= 0xff & hwm_reg_values[i + 2];
		printk(BIOS_DEBUG, "base = 0x%04x, reg = 0x%02x, "
		       "value = 0x%02x\n", base, reg, value);
		pnp_write_index(base, reg, value);
	}
}

static void w83627hf_init(device_t dev)
{
	struct superio_winbond_w83627hf_config *conf = dev->chip_info;
	struct resource *res0;

	if (!dev->enabled)
		return;

	switch(dev->path.pnp.device) {
	case W83627HF_KBC:
		pc_keyboard_init(&conf->keyboard);
		break;
	case W83627HF_HWM:
		res0 = find_resource(dev, PNP_IDX_IO0);
#define HWM_INDEX_PORT 5
		init_hwm(res0->base + HWM_INDEX_PORT);
		break;
	case W83627HF_ACPI:
		init_acpi(dev);
		break;
	}
}

static void w83627hf_pnp_enable_resources(device_t dev)
{
	pnp_enable_resources(dev);

	pnp_enter_conf_mode(dev);
	switch(dev->path.pnp.device) {
	case W83627HF_HWM:
		printk(BIOS_DEBUG, "W83627HF HWM SMBus enabled\n");
		enable_hwm_smbus(dev);
		break;
	}
	pnp_exit_conf_mode(dev);
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = w83627hf_pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = w83627hf_init,
	.ops_pnp_mode     = &pnp_conf_mode_8787_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ &ops, W83627HF_FDC,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, {0x07f8, 0}, },
	{ &ops, W83627HF_PP,   PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, {0x07f8, 0}, },
	{ &ops, W83627HF_SP1,  PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, W83627HF_SP2,  PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, W83627HF_KBC,  PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1, {0x07ff, 0}, {0x07ff, 4}, },
	{ &ops, W83627HF_CIR, PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, W83627HF_GAME_MIDI_GPIO1, PNP_IO0 | PNP_IO1 | PNP_IRQ0, {0x07ff, 0}, {0x07fe, 4}, },
	{ &ops, W83627HF_GPIO2, },
	{ &ops, W83627HF_GPIO3, },
	{ &ops, W83627HF_ACPI, },
	{ &ops, W83627HF_HWM,  PNP_IO0 | PNP_IRQ0, {0x0ff8, 0}, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_winbond_w83627hf_ops = {
	CHIP_NAME("Winbond W83627HF Super I/O")
	.enable_dev = enable_dev,
};
