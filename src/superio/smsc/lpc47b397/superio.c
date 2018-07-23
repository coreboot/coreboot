/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000 AG Electronics Ltd.
 * Copyright (C) 2003-2004 Linux Networx
 * Copyright (C) 2004 Tyan
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
 */

#include <arch/io.h>
#include <device/device.h>
#include <device/pnp.h>
#include <superio/conf_mode.h>
#include <console/console.h>
#include <device/smbus.h>
#include <string.h>
#include <pc80/keyboard.h>
#include <stdlib.h>
#include "lpc47b397.h"

static void enable_hwm_smbus(struct device *dev)
{
	/* Enable SensorBus register access. */
	u8 reg8;

	reg8 = pnp_read_config(dev, 0xf0);
	reg8 |= (1 << 1);
	pnp_write_config(dev, 0xf0, reg8);
}

static void lpc47b397_init(struct device *dev)
{

	if (!dev->enabled)
		return;

	switch(dev->path.pnp.device) {
	case LPC47B397_KBC:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	}
}

static void lpc47b397_pnp_enable_resources(struct device *dev)
{
	pnp_enable_resources(dev);

	pnp_enter_conf_mode(dev);
	switch(dev->path.pnp.device) {
	case LPC47B397_HWM:
		printk(BIOS_DEBUG, "LPC47B397 SensorBus register access enabled\n");
		pnp_set_logical_device(dev);
		enable_hwm_smbus(dev);
		break;
	}
	/* dump_pnp_device(dev); */
	pnp_exit_conf_mode(dev);
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = lpc47b397_pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = lpc47b397_init,
	.ops_pnp_mode     = &pnp_conf_mode_55_aa,
};

#define HWM_INDEX 0
#define HWM_DATA  1
#define SB_INDEX  0x0b
#define SB_DATA0  0x0c
#define SB_DATA1  0x0d
#define SB_DATA2  0x0e
#define SB_DATA3  0x0f

static int lsmbus_read_byte(struct device *dev, u8 address)
{
	unsigned int device;
	struct resource *res;
	int result;

	device = dev->path.i2c.device;

	res = find_resource(get_pbus_smbus(dev)->dev, PNP_IDX_IO0);

	pnp_write_index(res->base + HWM_INDEX, 0, device); /* Why 0? */

	/* We only read it one byte one time. */
	result = pnp_read_index(res->base + SB_INDEX, address);

	return result;
}

static int lsmbus_write_byte(struct device *dev, u8 address, u8 val)
{
	unsigned int device;
	struct resource *res;

	device = dev->path.i2c.device;
	res = find_resource(get_pbus_smbus(dev)->dev, PNP_IDX_IO0);

	pnp_write_index(res->base+HWM_INDEX, 0, device); /* Why 0? */

	/* We only write it one byte one time. */
	pnp_write_index(res->base+SB_INDEX, address, val);

	return 0;
}

static struct smbus_bus_operations lops_smbus_bus = {
	/* .recv_byte  = lsmbus_recv_byte, */
	/* .send_byte  = lsmbus_send_byte, */
	.read_byte  = lsmbus_read_byte,
	.write_byte = lsmbus_write_byte,
};

static struct device_operations ops_hwm = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = lpc47b397_pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = lpc47b397_init,
	.ops_smbus_bus    = &lops_smbus_bus,
	.ops_pnp_mode     = &pnp_conf_mode_55_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, LPC47B397_FDC, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x07f8, },
	{ NULL, LPC47B397_PP,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x07f8, },
	{ NULL, LPC47B397_SP1, PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, LPC47B397_SP2, PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, LPC47B397_KBC, PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1,
		0x07ff, 0x07ff, },
	{ NULL_hwm, LPC47B397_HWM,  PNP_IO0, 0x07f0, },
	{ NULL, LPC47B397_RT,  PNP_IO0, 0x0780, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_smsc_lpc47b397_ops = {
	CHIP_NAME("SMSC LPC47B397 Super I/O")
	.enable_dev = enable_dev,
};
