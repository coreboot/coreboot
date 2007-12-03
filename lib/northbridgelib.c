/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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

#include <types.h>
#include <console.h>
#include <device/device.h>
#include <device/pci.h>

/**
 * Set resources for the PCI domain.
 *
 * A PCI domain contains the I/O and memory resource address space below it.
 * Set up basic global ranges for I/O and memory. Allocation of sub-resources
 * draws on these top-level resources in the usual hierarchical manner.
 *
 * @param dev The northbridge device.
 */
void pci_domain_read_resources(struct device *dev)
{
	struct resource *res;

	/* Initialize the system-wide I/O space constraints. */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	res->limit = 0xffffUL;
	res->flags =
	    IORESOURCE_IO | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;

	/* Initialize the system-wide memory resources constraints. */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	res->limit = 0xffffffffULL;
	res->flags =
	    IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;
}

/**
 * Create a RAM resource, by taking the passed-in size and creating
 * a resource record.
 *
 * @param dev The device.
 * @param index A resource index.
 * @param basek Base memory address in KB.
 * @param sizek Size of memory in KB.
 */
void ram_resource(struct device *dev, unsigned long index,
		  unsigned long basek, unsigned long sizek)
{
	struct resource *res;

	if (!sizek)
		return;

	res = new_resource(dev, index);
	res->base = ((resource_t) basek) << 10;	/* Convert to bytes. */
	res->size = ((resource_t) sizek) << 10; /* Convert to bytes. */
	res->flags = IORESOURCE_MEM | IORESOURCE_CACHEABLE |
	    IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED;

	printk(BIOS_SPEW, "Adding RAM resource (%lld bytes)\n", res->size);
}

/**
 * Support for scan bus from the "tippy top" -- i.e. the PCI domain,
 * not the 0:0.0 device.
 *
 * This function works for almost all chipsets (AMD K8 is the exception).
 *
 * @param dev The PCI domain device.
 * @param max Maximum number of devices to scan.
 * @return TODO
 */
unsigned int pci_domain_scan_bus(struct device *dev, unsigned int max)
{
	/* There is only one link on this device, and it is always link 0. */
	return pci_scan_bus(&dev->link[0], PCI_DEVFN(0, 0), 0xff, max);
}
