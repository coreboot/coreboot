/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Arastra, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

/* This code is based on src/southbridge/intel/esb6300/esb6300.c */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "i3100.h"

void i3100_enable(device_t dev)
{
	device_t lpc_dev;
	u32 index = 0;
	u16 reg_old, reg;

	/* See if we are behind the i3100 PCI bridge */
	lpc_dev = dev_find_slot(dev->bus->secondary, PCI_DEVFN(0x1f, 0));
	if ((dev->path.u.pci.devfn & 0xf8) == 0xf8) {
		index = dev->path.u.pci.devfn & 7;
	}
	else if ((dev->path.u.pci.devfn & 0xf8) == 0xe8) {
		index = (dev->path.u.pci.devfn & 7) + 8;
	}
	if ((!lpc_dev) || (index >= 16) || ((1 << index) & 0x3091)) {
		return;
	}
	if ((lpc_dev->vendor != PCI_VENDOR_ID_INTEL) ||
		(lpc_dev->device != PCI_DEVICE_ID_INTEL_3100_LPC)) {
		u32 id;
		id = pci_read_config32(lpc_dev, PCI_VENDOR_ID);
		if (id != (PCI_VENDOR_ID_INTEL |
				(PCI_DEVICE_ID_INTEL_3100_LPC << 16))) {
			return;
		}
	}

	reg = reg_old = pci_read_config16(lpc_dev, 0xf2);
	reg &= ~(1 << index);
	if (!dev->enabled) {
		reg |= (1 << index);
	}
	if (reg != reg_old) {
		pci_write_config16(lpc_dev, 0xf2, reg);
	}
}

struct chip_operations southbridge_intel_i3100_ops = {
	CHIP_NAME("Intel 3100 Southbridge")
	.enable_dev = i3100_enable,
};
