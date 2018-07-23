/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 Digital Design Corporation
 * Copyright (C) 2008-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* RAM-based driver for SMSC LPC47N227 Super I/O chip. */

#include <arch/io.h>
#include <device/device.h>
#include <device/pnp.h>
#include <console/console.h>
#include <device/smbus.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <pc80/keyboard.h>
#include <superio/conf_mode.h>

#include "lpc47n227.h"

/* Forward declarations. */
static void enable_dev(struct device *dev);
void lpc47n227_pnp_set_resources(struct device *dev);
void lpc47n227_pnp_enable_resources(struct device *dev);
void lpc47n227_pnp_enable(struct device *dev);
static void lpc47n227_init(struct device *dev);
static void lpc47n227_pnp_set_resource(struct device *dev, struct resource *resource);
void lpc47n227_pnp_set_iobase(struct device *dev, u16 iobase);
void lpc47n227_pnp_set_drq(struct device *dev, u8 drq);
void lpc47n227_pnp_set_irq(struct device *dev, u8 irq);
void lpc47n227_pnp_set_enable(struct device *dev, int enable);

struct chip_operations superio_smsc_lpc47n227_ops = {
	CHIP_NAME("SMSC LPC47N227 Super I/O")
	.enable_dev = enable_dev,
};

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = lpc47n227_pnp_set_resources,
	.enable_resources = lpc47n227_pnp_enable_resources,
	.enable           = lpc47n227_pnp_enable,
	.init             = lpc47n227_init,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, LPC47N227_PP, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x07f8, },
	{ NULL, LPC47N227_SP1, PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, LPC47N227_SP2, PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, LPC47N227_KBDC, PNP_IO0 | PNP_IO1 | PNP_IRQ0, 0x07f8, 0x07f8, },
};

/**
 * Create device structures and allocate resources to devices specified in the
 * pnp_dev_info array (above).
 *
 * @param dev Pointer to structure describing a Super I/O device.
 */
static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

/**
 * Configure the specified Super I/O device with the resources (I/O space,
 * etc.) that have been allocate for it.
 *
 * NOTE: Cannot use pnp_set_resources() here because it assumes chip
 * support for logical devices, which the LPC47N227 doesn't have.
 *
 * @param dev Pointer to structure describing a Super I/O device.
 */
void lpc47n227_pnp_set_resources(struct device *dev)
{
	struct resource *res;

	pnp_enter_conf_mode_55(dev);
	for (res = dev->resource_list; res; res = res->next)
		lpc47n227_pnp_set_resource(dev, res);
	pnp_exit_conf_mode_aa(dev);
}

/*
 * NOTE: Cannot use pnp_enable_resources() here because it assumes chip
 * support for logical devices, which the LPC47N227 doesn't have.
 */
void lpc47n227_pnp_enable_resources(struct device *dev)
{
	pnp_enter_conf_mode_55(dev);
	lpc47n227_pnp_set_enable(dev, 1);
	pnp_exit_conf_mode_aa(dev);
}

/*
 * NOTE: Cannot use pnp_set_enable() here because it assumes chip
 * support for logical devices, which the LPC47N227 doesn't have.
 */
void lpc47n227_pnp_enable(struct device *dev)
{
	pnp_enter_conf_mode_55(dev);
	lpc47n227_pnp_set_enable(dev, !!dev->enabled);
	pnp_exit_conf_mode_aa(dev);
}

/**
 * Initialize the specified Super I/O device.
 *
 * Devices other than COM ports and keyboard controller are ignored.
 * For COM ports, we configure the baud rate.
 *
 * @param dev Pointer to structure describing a Super I/O device.
 */
static void lpc47n227_init(struct device *dev)
{

	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	case LPC47N227_KBDC:
		printk(BIOS_DEBUG, "LPC47N227: Initializing keyboard.\n");
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	}
}

static void lpc47n227_pnp_set_resource(struct device *dev, struct resource *resource)
{
	if (!(resource->flags & IORESOURCE_ASSIGNED)) {
		printk(BIOS_ERR, "ERROR: %s %02lx not allocated\n",
		       dev_path(dev), resource->index);
		return;
	}

	/* Now store the resource. */
	/*
	 * NOTE: Cannot use pnp_set_XXX() here because they assume chip
	 * support for logical devices, which the LPC47N227 doesn't have.
	 */
	if (resource->flags & IORESOURCE_IO) {
		lpc47n227_pnp_set_iobase(dev, resource->base);
	} else if (resource->flags & IORESOURCE_DRQ) {
		lpc47n227_pnp_set_drq(dev, resource->base);
	} else if (resource->flags & IORESOURCE_IRQ) {
		lpc47n227_pnp_set_irq(dev, resource->base);
	} else {
		printk(BIOS_ERR, "ERROR: %s %02lx unknown resource type\n",
		       dev_path(dev), resource->index);
		return;
	}
	resource->flags |= IORESOURCE_STORED;

	report_resource_stored(dev, resource, "");
}

void lpc47n227_pnp_set_iobase(struct device *dev, u16 iobase)
{
	ASSERT(!(iobase & 0x3));

	switch (dev->path.pnp.device) {
	case LPC47N227_PP:
		pnp_write_config(dev, 0x23, (iobase >> 2) & 0xff);
		break;
	case LPC47N227_SP1:
		pnp_write_config(dev, 0x24, (iobase >> 2) & 0xff);
		break;
	case LPC47N227_SP2:
		pnp_write_config(dev, 0x25, (iobase >> 2) & 0xff);
		break;
	case LPC47N227_KBDC:
		break;
	default:
		BUG();
		break;
	}
}

void lpc47n227_pnp_set_drq(struct device *dev, u8 drq)
{
	const u8 PP_DMA_MASK = 0x0F;
	const u8 PP_DMA_SELECTION_REGISTER = 0x26;
	u8 current_config, new_config;

	if (dev->path.pnp.device == LPC47N227_PP) {
		current_config = pnp_read_config(dev,
						 PP_DMA_SELECTION_REGISTER);
		ASSERT(!(drq & ~PP_DMA_MASK));	// DRQ out of range??
		new_config = (current_config & ~PP_DMA_MASK) | drq;
		pnp_write_config(dev, PP_DMA_SELECTION_REGISTER, new_config);
	} else {
		BUG();
	}
}

void lpc47n227_pnp_set_irq(struct device *dev, u8 irq)
{
	u8 irq_config_register = 0, irq_config_mask = 0;
	u8 current_config, new_config;

	switch (dev->path.pnp.device) {
	case LPC47N227_PP:
		irq_config_register = 0x27;
		irq_config_mask = 0x0F;
		break;
	case LPC47N227_SP1:
		irq_config_register = 0x28;
		irq_config_mask = 0xF0;
		irq <<= 4;
		break;
	case LPC47N227_SP2:
		irq_config_register = 0x28;
		irq_config_mask = 0x0F;
		break;
	case LPC47N227_KBDC:
		break;
	default:
		BUG();
		return;
	}

	current_config = pnp_read_config(dev, irq_config_register);
	new_config = (current_config & ~irq_config_mask) | irq;
	pnp_write_config(dev, irq_config_register, new_config);
}

void lpc47n227_pnp_set_enable(struct device *dev, int enable)
{
	u8 power_register = 0, power_mask = 0, current_power, new_power;

	switch (dev->path.pnp.device) {
	case LPC47N227_PP:
		power_register = 0x01;
		power_mask = 0x04;
		break;
	case LPC47N227_SP1:
		power_register = 0x02;
		power_mask = 0x08;
		break;
	case LPC47N227_SP2:
		power_register = 0x02;
		power_mask = 0x80;
		break;
	case LPC47N227_KBDC:
		break;
	default:
		BUG();
		return;
	}

	current_power = pnp_read_config(dev, power_register);
	new_power = current_power & ~power_mask; /* Disable by default. */
	if (enable) {
		struct resource *ioport_resource;
		ioport_resource = find_resource(dev, PNP_IDX_IO0);
		lpc47n227_pnp_set_iobase(dev, ioport_resource->base);
		new_power |= power_mask; /* Enable. */
	} else {
		lpc47n227_pnp_set_iobase(dev, 0);
	}
	pnp_write_config(dev, power_register, new_power);
}
