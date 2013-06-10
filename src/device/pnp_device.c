/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Linux Networx
 * (Written by Eric Biederman <ebiederman@lnxi.com> for Linux Networx)
 * Copyright (C) 2004 Li-Ta Lo <ollie@lanl.gov>
 * Copyright (C) 2005 Tyan
 * (Written by Yinghai Lu <yhlu@tyan.com> for Tyan)
 * Copyright (C) 2013 Nico Huber <nico.h@gmx.de>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/pnp.h>

/* PNP config mode wrappers */

void pnp_enter_conf_mode(device_t dev)
{
	if (dev->ops->ops_pnp_mode)
		dev->ops->ops_pnp_mode->enter_conf_mode(dev);
}

void pnp_exit_conf_mode(device_t dev)
{
	if (dev->ops->ops_pnp_mode)
		dev->ops->ops_pnp_mode->exit_conf_mode(dev);
}

/* PNP fundamental operations */

void pnp_write_config(device_t dev, u8 reg, u8 value)
{
	outb(reg, dev->path.pnp.port);
	outb(value, dev->path.pnp.port + 1);
}

u8 pnp_read_config(device_t dev, u8 reg)
{
	outb(reg, dev->path.pnp.port);
	return inb(dev->path.pnp.port + 1);
}

void pnp_set_logical_device(device_t dev)
{
	pnp_write_config(dev, 0x07, dev->path.pnp.device & 0xff);
}

void pnp_set_enable(device_t dev, int enable)
{
	u8 tmp, bitpos;

	tmp = pnp_read_config(dev, 0x30);

	/* Handle virtual devices, which share the same LDN register. */
	bitpos = (dev->path.pnp.device >> 8) & 0x7;

	if (enable)
		tmp |= (1 << bitpos);
	else
		tmp &= ~(1 << bitpos);

	pnp_write_config(dev, 0x30, tmp);
}

int pnp_read_enable(device_t dev)
{
	u8 tmp, bitpos;

	tmp = pnp_read_config(dev, 0x30);

	/* Handle virtual devices, which share the same LDN register. */
	bitpos = (dev->path.pnp.device >> 8) & 0x7;

	return !!(tmp & (1 << bitpos));
}

void pnp_set_iobase(device_t dev, u8 index, u16 iobase)
{
	/* Index == 0x60 or 0x62. */
	pnp_write_config(dev, index + 0, (iobase >> 8) & 0xff);
	pnp_write_config(dev, index + 1, iobase & 0xff);
}

void pnp_set_irq(device_t dev, u8 index, u8 irq)
{
	/* Index == 0x70 or 0x72. */
	pnp_write_config(dev, index, irq);
}

void pnp_set_drq(device_t dev, u8 index, u8 drq)
{
	/* Index == 0x74. */
	pnp_write_config(dev, index, drq & 0xff);
}

/* PNP device operations */

void pnp_read_resources(device_t dev)
{
	return;
}

static void pnp_set_resource(device_t dev, struct resource *resource)
{
	if (!(resource->flags & IORESOURCE_ASSIGNED)) {
		printk(BIOS_ERR, "ERROR: %s %02lx %s size: 0x%010llx "
		       "not assigned\n", dev_path(dev), resource->index,
		       resource_type(resource), resource->size);
		return;
	}

	/* Now store the resource. */
	if (resource->flags & IORESOURCE_IO) {
		pnp_set_iobase(dev, resource->index, resource->base);
	} else if (resource->flags & IORESOURCE_DRQ) {
		pnp_set_drq(dev, resource->index, resource->base);
	} else if (resource->flags & IORESOURCE_IRQ) {
		pnp_set_irq(dev, resource->index, resource->base);
	} else {
		printk(BIOS_ERR, "ERROR: %s %02lx unknown resource type\n",
		       dev_path(dev), resource->index);
		return;
	}
	resource->flags |= IORESOURCE_STORED;

	report_resource_stored(dev, resource, "");
}

void pnp_set_resources(device_t dev)
{
	struct resource *res;

	pnp_enter_conf_mode(dev);

	/* Select the logical device (LDN). */
	pnp_set_logical_device(dev);

	/* Paranoia says I should disable the device here... */
	for (res = dev->resource_list; res; res = res->next)
		pnp_set_resource(dev, res);

	pnp_exit_conf_mode(dev);
}

void pnp_enable_resources(device_t dev)
{
	pnp_enter_conf_mode(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 1);
	pnp_exit_conf_mode(dev);
}

void pnp_enable(device_t dev)
{
	if (!dev->enabled) {
		pnp_enter_conf_mode(dev);
		pnp_set_logical_device(dev);
		pnp_set_enable(dev, 0);
		pnp_exit_conf_mode(dev);
	}
}

void pnp_alt_enable(device_t dev)
{
	pnp_enter_conf_mode(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, !!dev->enabled);
	pnp_exit_conf_mode(dev);
}

struct device_operations pnp_ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_enable,
};

/* PNP chip operations */

static void pnp_get_ioresource(device_t dev, u8 index, struct io_info *info)
{
	struct resource *resource;
	unsigned moving, gran, step;

	if (!info->mask) {
		printk(BIOS_ERR, "ERROR: device %s index %d has no mask.\n",
				dev_path(dev), index);
		return;
	}

	resource = new_resource(dev, index);

	/* Initilize the resource. */
	resource->limit = 0xffff;
	resource->flags |= IORESOURCE_IO;

	/* Get the resource size... */

	moving = info->mask;
	gran = 15;
	step = 1 << gran;

	/* Find the first bit that moves. */
	while ((moving & step) == 0) {
		gran--;
		step >>= 1;
	}

	/* Now find the first bit that does not move. */
	while ((moving & step) != 0) {
		gran--;
		step >>= 1;
	}

	/*
	 * Of the moving bits the last bit in the first group,
	 * tells us the size of this resource.
	 */
	if ((moving & step) == 0) {
		gran++;
		step <<= 1;
	}

	/* Set the resource size and alignment. */
	resource->gran  = gran;
	resource->align = gran;
	resource->limit = info->mask | (step - 1);
	resource->size  = 1 << gran;
}

static void get_resources(device_t dev, struct pnp_info *info)
{
	struct resource *resource;

	if (info->flags & PNP_IO0)
		pnp_get_ioresource(dev, PNP_IDX_IO0, &info->io0);
	if (info->flags & PNP_IO1)
		pnp_get_ioresource(dev, PNP_IDX_IO1, &info->io1);
	if (info->flags & PNP_IO2)
		pnp_get_ioresource(dev, PNP_IDX_IO2, &info->io2);
	if (info->flags & PNP_IO3)
		pnp_get_ioresource(dev, PNP_IDX_IO3, &info->io3);

	if (info->flags & PNP_IRQ0) {
		resource = new_resource(dev, PNP_IDX_IRQ0);
		resource->size = 1;
		resource->flags |= IORESOURCE_IRQ;
	}
	if (info->flags & PNP_IRQ1) {
		resource = new_resource(dev, PNP_IDX_IRQ1);
		resource->size = 1;
		resource->flags |= IORESOURCE_IRQ;
	}

	if (info->flags & PNP_DRQ0) {
		resource = new_resource(dev, PNP_IDX_DRQ0);
		resource->size = 1;
		resource->flags |= IORESOURCE_DRQ;
	}
	if (info->flags & PNP_DRQ1) {
		resource = new_resource(dev, PNP_IDX_DRQ1);
		resource->size = 1;
		resource->flags |= IORESOURCE_DRQ;
	}

	/*
	 * These are not IRQs, but set the flag to have the
	 * resource allocator do the right thing.
	 */
	if (info->flags & PNP_EN) {
		resource = new_resource(dev, PNP_IDX_EN);
		resource->size = 1;
		resource->flags |= IORESOURCE_IRQ;
	}
	if (info->flags & PNP_MSC0) {
		resource = new_resource(dev, PNP_IDX_MSC0);
		resource->size = 1;
		resource->flags |= IORESOURCE_IRQ;
	}
	if (info->flags & PNP_MSC1) {
		resource = new_resource(dev, PNP_IDX_MSC1);
		resource->size = 1;
		resource->flags |= IORESOURCE_IRQ;
	}
}

void pnp_enable_devices(device_t base_dev, struct device_operations *ops,
			unsigned int functions, struct pnp_info *info)
{
	struct device_path path;
	device_t dev;
	int i;

	path.type = DEVICE_PATH_PNP;
	path.pnp.port = base_dev->path.pnp.port;

	/* Setup the ops and resources on the newly allocated devices. */
	for (i = 0; i < functions; i++) {
		/* Skip logical devices this Super I/O doesn't have. */
		if (info[i].function == -1)
			continue;

		path.pnp.device = info[i].function;
		dev = alloc_find_dev(base_dev->bus, &path);

		/* Don't initialize a device multiple times. */
		if (dev->ops)
			continue;

		if (info[i].ops == 0)
			dev->ops = ops;
		else
			dev->ops = info[i].ops;

		get_resources(dev, &info[i]);
	}
}
