/*
 * AMD 8111 "southbridge"
 * This file is part of the coreboot project.
 * Copyright (C) 2004-2005 Linux Networx
 * (Written by Eric Biederman <ebiederman@lnxi.com> and Jason Schildt for Linux Networx)
 * Copyright (C) 2005-7 YingHai Lu
 * Copyright (C) 2005 Ollie Lo
 * Copyright (C) 2005-2007 Stefan Reinauer <stepan@openbios.org>
 * Copyright (C) 2008 Ronald G. Minnich <rminnich@gmail.com>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */
#include <types.h>
#include <lib.h>
#include <console.h>
#include <device/pci.h>
#include <msr.h>
#include <legacy.h>
#include <device/pci_ids.h>
#include <statictree.h>
#include <config.h>
#include "amd8111.h"

void amd8111_enable(struct device * dev)
{
	struct device * lpc_dev;
	struct device * bus_dev;
	unsigned index;
	unsigned reg_old, reg;

	/* See if we are behind the amd8111 pci bridge */
	bus_dev = dev->bus->dev;
	if ((bus_dev->id.pci.vendor == PCI_VENDOR_ID_AMD) && 
	    (bus_dev->id.pci.device == PCI_DEVICE_ID_AMD_8111_PCI)) 
	{
		unsigned devfn;
		devfn = bus_dev->path.pci.devfn + (1 << 3);
		lpc_dev = dev_find_slot(bus_dev->bus->secondary, devfn);
		index = ((dev->path.pci.devfn & ~7) >> 3) + 8;
		if (dev->path.pci.devfn == 2) { /* EHCI */
			index = 16;
		}
	} else {
		unsigned devfn;
		devfn = (dev->path.pci.devfn) & ~7;
		lpc_dev = dev_find_slot(dev->bus->secondary, devfn);
		index = dev->path.pci.devfn & 7;
	}
	if ((!lpc_dev) || (index >= 17)) {
		return;
	}
	if ((lpc_dev->id.pci.vendor != PCI_VENDOR_ID_AMD) ||
	    (lpc_dev->id.pci.device != PCI_DEVICE_ID_AMD_8111_ISA)) 
	{
		u32 id;
		id = pci_read_config32(lpc_dev, PCI_VENDOR_ID);
		if (id != (PCI_VENDOR_ID_AMD | (PCI_DEVICE_ID_AMD_8111_ISA << 16))) {
			return;
		}
	}

	if (index < 16) {
		reg = reg_old = pci_read_config16(lpc_dev, 0x48);
		reg &= ~(1 << index);
		if (dev->enabled) {
			reg |= (1 << index);
		}
		if (reg != reg_old) {
			pci_write_config16(lpc_dev, 0x48, reg);
		}
	}
	else if (index == 16) {
		reg = reg_old = pci_read_config8(lpc_dev, 0x47);
		reg &= ~(1 << 7);
		if (!dev->enabled) {
			reg |= (1 << 7);
		}
		if (reg != reg_old) {
			pci_write_config8(lpc_dev, 0x47, reg);
		}
	}
}

