/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Linux Networx
 * (Written by Eric Biederman <ebiederman@lnxi.com> for Linux Networx)
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console.h>
#include <device/device.h>
#include <pciconf.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <types.h>

/**
 * Walk up the tree from the current dev, in an attempt to find a
 * bus that has ops_pci_bus set. The assumption here being that if it
 * has ops_pci_bus set, then it can do bus operations.
 */
static struct bus *get_pbus(struct device *dev)
{
	struct bus *pbus = dev->bus;
	while (pbus && pbus->dev && !ops_pci_bus(pbus)) {
		if (pbus->dev == dev) {
			printk(BIOS_EMERG, "Loop: dev->dtsname dev->bus->dev\n");
			printk(BIOS_EMERG, "To fix this, set ops_pci_bus in dts\n");
			die("loop due to insufficient dts");
		}

		pbus = pbus->dev->bus;
	}
	if (!pbus || !pbus->dev || !pbus->dev->ops
	    || !pbus->dev->ops->ops_pci_bus) {
		printk(BIOS_ALERT, "%s: %s(%s) Cannot find PCI bus operations",
		       __func__, dev->dtsname, dev_path(dev));
		die("");
	}
	return pbus;
}

u8 pci_read_config8(struct device *dev, unsigned int where)
{
        struct bus *pbus = get_pbus(dev);
	return ops_pci_bus(pbus)->read8(PCI_BDEVFN(dev->bus->secondary,
						     dev->path.pci.devfn), 
					where);
}

u16 pci_read_config16(struct device *dev, unsigned int where)
{
	struct bus *pbus = get_pbus(dev);
	return ops_pci_bus(pbus)->read16(PCI_BDEVFN(dev->bus->secondary,
						      dev->path.pci.devfn), 
					 where);
}

u32 pci_read_config32(struct device *dev, unsigned int where)
{
	struct bus *pbus = get_pbus(dev);
	return ops_pci_bus(pbus)->read32(PCI_BDEVFN(dev->bus->secondary,
						      dev->path.pci.devfn), 
					 where);
}

void pci_write_config8(struct device *dev, unsigned int where, u8 val)
{
	struct bus *pbus = get_pbus(dev);
	ops_pci_bus(pbus)->write8(PCI_BDEVFN(dev->bus->secondary,
					       dev->path.pci.devfn), 
				  where, val);
}

void pci_write_config16(struct device *dev, unsigned int where, u16 val)
{
	struct bus *pbus = get_pbus(dev);
	ops_pci_bus(pbus)->write16(PCI_BDEVFN(dev->bus->secondary,
						dev->path.pci.devfn), 
				   where, val);
}

void pci_write_config32(struct device *dev, unsigned int where, u32 val)
{
	struct bus *pbus = get_pbus(dev);
	ops_pci_bus(pbus)->write32(PCI_BDEVFN(dev->bus->secondary,
						dev->path.pci.devfn), 
				   where, val);
}
