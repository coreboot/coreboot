/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Linux Networx
 * (Written by Eric Biederman <ebiederman@lnxi.com> for Linux Networx)
 * Copyright (C) 2009 coresystems GmbH
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

#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

/* Overwrite with architecture specific method */
const struct pci_bus_operations __attribute__((weak))
*pci_bus_default_ops(struct device *dev)
{
	return NULL;
}

static const struct pci_bus_operations *pci_bus_ops(struct bus *bus, struct device *dev)
{
	const struct pci_bus_operations *bops;
	bops = NULL;
	if (bus && bus->dev && bus->dev->ops && bus->dev->ops->ops_pci_bus) {
		bops = bus->dev->ops->ops_pci_bus(dev);
	}
	if (!bops)
		bops = pci_bus_default_ops(dev);
	return bops;
}

/*
 * The only consumer of the return value of get_pbus() is pci_bus_ops().
 * pci_bus_ops() can handle being passed NULL and auto-picks working ops.
 */
static struct bus *get_pbus(struct device *dev)
{
	struct bus *pbus = NULL;

	if (!dev)
		die("get_pbus: dev is NULL!\n");
	else
		pbus = dev->bus;

	while (pbus && pbus->dev && !pci_bus_ops(pbus, dev)) {
		if (pbus == pbus->dev->bus) {
			printk(BIOS_ALERT, "%s in endless loop looking for a "
			       "parent bus with pci_bus_ops for %s, breaking "
			       "out.\n", __func__, dev_path(dev));
			break;
		}
		pbus = pbus->dev->bus;
	}

	if (!pbus || !pbus->dev || !pbus->dev->ops
	    || !pbus->dev->ops->ops_pci_bus) {
		/* This can happen before the device tree is fully set up. */

		// printk(BIOS_EMERG, "%s: Cannot find PCI bus operations.\n",
		// dev_path(dev));

		pbus = NULL;
	}

	return pbus;
}

u8 pci_read_config8(struct device *dev, unsigned int where)
{
	struct bus *pbus = get_pbus(dev);
	return pci_bus_ops(pbus, dev)->read8(pbus, dev->bus->secondary,
					dev->path.pci.devfn, where);
}

u16 pci_read_config16(struct device *dev, unsigned int where)
{
	struct bus *pbus = get_pbus(dev);
	return pci_bus_ops(pbus, dev)->read16(pbus, dev->bus->secondary,
					 dev->path.pci.devfn, where);
}

u32 pci_read_config32(struct device *dev, unsigned int where)
{
	struct bus *pbus = get_pbus(dev);
	return pci_bus_ops(pbus, dev)->read32(pbus, dev->bus->secondary,
					 dev->path.pci.devfn, where);
}

void pci_write_config8(struct device *dev, unsigned int where, u8 val)
{
	struct bus *pbus = get_pbus(dev);
	pci_bus_ops(pbus, dev)->write8(pbus, dev->bus->secondary,
				  dev->path.pci.devfn, where, val);
}

void pci_write_config16(struct device *dev, unsigned int where, u16 val)
{
	struct bus *pbus = get_pbus(dev);
	pci_bus_ops(pbus, dev)->write16(pbus, dev->bus->secondary,
				   dev->path.pci.devfn, where, val);
}

void pci_write_config32(struct device *dev, unsigned int where, u32 val)
{
	struct bus *pbus = get_pbus(dev);
	pci_bus_ops(pbus, dev)->write32(pbus, dev->bus->secondary,
				   dev->path.pci.devfn, where, val);
}
