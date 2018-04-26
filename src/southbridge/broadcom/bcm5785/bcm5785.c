/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
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
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "bcm5785.h"

void bcm5785_enable(device_t dev)
{
	device_t sb_pci_main_dev;
	device_t bus_dev;
	// unsigned index;

	/* See if we are on the behind the pcix bridge */
	bus_dev = dev->bus->dev;
	if ((bus_dev->vendor == PCI_VENDOR_ID_SERVERWORKS) &&
		(bus_dev->device == 0x0036)) // device under PCI-X Bridge
	{
		unsigned devfn;
		devfn = bus_dev->path.pci.devfn + (1 << 3);
		sb_pci_main_dev = dev_find_slot(bus_dev->bus->secondary, devfn);
		// index = ((dev->path.pci.devfn & ~7) >> 3) + 8;
	} else if ((bus_dev->vendor == PCI_VENDOR_ID_SERVERWORKS) &&
		(bus_dev->device == 0x0104)) // device under PCI Bridge (under PCI-X)
	{
		unsigned devfn;
		devfn = bus_dev->bus->dev->path.pci.devfn + (1 << 3);
		sb_pci_main_dev = dev_find_slot(bus_dev->bus->dev->bus->secondary, devfn);
		// index = ((dev->path.pci.devfn & ~7) >> 3) + 8;
	}
	else { // same bus
		unsigned devfn;
		devfn = (dev->path.pci.devfn) & ~7;
		if (dev->vendor == PCI_VENDOR_ID_SERVERWORKS) {
			if (dev->device == 0x0036) //PCI-X Bridge
			{ devfn += (1<<3); }
			else if (dev->device == 0x0223) // USB
			{ devfn -= (1<<3); }
		}
		sb_pci_main_dev = dev_find_slot(dev->bus->secondary, devfn);
		// index = dev->path.pci.devfn & 7;
	}
	if (!sb_pci_main_dev) {
		return;
	}

	// get index now
#if 0
	unsigned reg_old, reg;
	if (index < 16) {
		reg = reg_old = pci_read_config16(sb_pci_main_dev, 0x48);
		reg &= ~(1 << index);
		if (dev->enabled) {
			reg |= (1 << index);
		}
		if (reg != reg_old) {
			pci_write_config16(sb_pci_main_dev, 0x48, reg);
		}
	}
	else if (index == 16) {
		reg = reg_old = pci_read_config8(sb_pci_main_dev, 0x47);
		reg &= ~(1 << 7);
		if (!dev->enabled) {
			reg |= (1 << 7);
		}
		if (reg != reg_old) {
			pci_write_config8(sb_pci_main_dev, 0x47, reg);
		}
	}
#endif
}

struct chip_operations southbridge_broadcom_bcm5785_ops = {
	CHIP_NAME("Serverworks BCM5785 Southbridge")
	.enable_dev = bcm5785_enable,
};
