/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Edward O'Callaghan <eocallaghan@alterapraxis.com>
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

/* This code should work for all Fintek Super I/O HWM's. */
#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pnp.h>
#include <stdlib.h>
#include <hwm/superio_hwm.h>

/* Helper functions */
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
/* .. */

/* Initialize F71869AD hardware monitor registers, which are at 0x225. */
/* XXX: make configurable.. */
static void init_registers(u16 base)
{
	u8 reg, value;
	int i;

	/* XXX: work out correct values??? */
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
/* .. */

/* Main driver */
static void hwm_init(struct device *dev)
{
	struct resource *res0;

	if (!CONFIG_DRIVERS_FINTEK_HWM)
		return;

	printk(BIOS_DEBUG, "Initializing Fintek Hardware Monitor..\n");
	res0 = find_resource(dev, PNP_IDX_IO0);
	init_registers(res0->base);
}

/*
static void hwm_noop(device_t dummy)
{
}

static struct device_operations hwm_ops = {
	.read_resources = hwm_noop,
	.set_resources = hwm_noop,
	.enable_resources = hwm_noop,
	.init = hwm_init,
};

static void enable_dev(struct device *dev)
{
	dev->ops = &hwm_ops;
}

struct chip_operations hwm_fintek_ops = {
	CHIP_NAME("Fintek F71869AD Super I/O HWM")
	.enable_dev = enable_dev
};
*/
