/*
 * This file is part of the LinuxBIOS project.
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

#include <console/console.h>
#include <arch/pciconf.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

static struct bus *get_pbus(device_t dev)
{
	struct bus *pbus = dev->bus;
	while(pbus && pbus->dev && !ops_pci_bus(pbus)) {
		pbus = pbus->dev->bus;
	}
	if (!pbus || !pbus->dev || !pbus->dev->ops || !pbus->dev->ops->ops_pci_bus) {
		printk_alert("%s Cannot find pci bus operations", dev_path(dev));
		die("");
		for(;;);
	}
	return pbus;
}

uint8_t pci_read_config8(device_t dev, unsigned where)
{
	struct bus *pbus = get_pbus(dev);
	return ops_pci_bus(pbus)->read8(pbus, dev->bus->secondary, dev->path.u.pci.devfn, where);
}

uint16_t pci_read_config16(device_t dev, unsigned where)
{
	struct bus *pbus = get_pbus(dev);
	return ops_pci_bus(pbus)->read16(pbus, dev->bus->secondary, dev->path.u.pci.devfn, where);
}

uint32_t pci_read_config32(device_t dev, unsigned where)
{
	struct bus *pbus = get_pbus(dev);
	return ops_pci_bus(pbus)->read32(pbus, dev->bus->secondary, dev->path.u.pci.devfn, where);
}

void pci_write_config8(device_t dev, unsigned where, uint8_t val)
{
	struct bus *pbus = get_pbus(dev);
	ops_pci_bus(pbus)->write8(pbus, dev->bus->secondary, dev->path.u.pci.devfn, where, val);
}

void pci_write_config16(device_t dev, unsigned where, uint16_t val)
{
	struct bus *pbus = get_pbus(dev);
	ops_pci_bus(pbus)->write16(pbus, dev->bus->secondary, dev->path.u.pci.devfn, where, val);
}

void pci_write_config32(device_t dev, unsigned where, uint32_t val)
{
	struct bus *pbus = get_pbus(dev);
	ops_pci_bus(pbus)->write32(pbus, dev->bus->secondary, dev->path.u.pci.devfn, where, val);
}
