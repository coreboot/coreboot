/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
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

#include <console/console.h>
#include <arch/io.h>
#include <arch/ioapic.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "cs5530.h"

static void cs5530_read_resources(device_t dev)
{
	struct resource* res;

	pci_dev_read_resources(dev);

	res = new_resource(dev, 1);
	res->base = 0x0UL;
	res->size = 0x1000UL;
	res->limit = 0xffffUL;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, 3); /* IOAPIC */
	res->base = IO_APIC_ADDR;
	res->size = 0x00001000;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static void isa_init(struct device *dev)
{
}

static struct device_operations isa_ops = {
	.read_resources		= cs5530_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= isa_init,
	.enable			= 0,
	.scan_bus		= scan_static_bus,
};

static const struct pci_driver isa_driver __pci_driver = {
	.ops	= &isa_ops,
	.vendor	= PCI_VENDOR_ID_CYRIX,
	.device	= PCI_DEVICE_ID_CYRIX_5530_LEGACY,
};
