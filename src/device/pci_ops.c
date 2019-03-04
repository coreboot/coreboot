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

#include <stdint.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>

static __always_inline const struct pci_bus_operations *pci_bus_ops(void)
{
	return pci_bus_default_ops();
}

static void pcidev_assert(const struct device *dev)
{
	if (dev)
		return;
	die("PCI: dev is NULL!\n");
}

u8 pci_read_config8(struct device *dev, unsigned int where)
{
	pcidev_assert(dev);
	return pci_bus_ops()->read8(dev, where);
}

u16 pci_read_config16(struct device *dev, unsigned int where)
{
	pcidev_assert(dev);
	return pci_bus_ops()->read16(dev, where);
}

u32 pci_read_config32(struct device *dev, unsigned int where)
{
	pcidev_assert(dev);
	return pci_bus_ops()->read32(dev, where);
}

void pci_write_config8(struct device *dev, unsigned int where, u8 val)
{
	pcidev_assert(dev);
	pci_bus_ops()->write8(dev, where, val);
}

void pci_write_config16(struct device *dev, unsigned int where, u16 val)
{
	pcidev_assert(dev);
	pci_bus_ops()->write16(dev, where, val);
}

void pci_write_config32(struct device *dev, unsigned int where, u32 val)
{
	pcidev_assert(dev);
	pci_bus_ops()->write32(dev, where, val);
}
