/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 Digital Design Corporation
 * (Written by Steven J. Magnani <steve@digidescorp.com> for Digital Design)
 * Copyright (C) 2007 Corey Osgood <corey.osgood@gmail.com>
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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include "i82801ax.h"

void i82801ax_enable(device_t dev)
{
	unsigned int index = 0;
	uint16_t cur_disable_mask, new_disable_mask;

	/* All 82801xx devices should be on bus 0. */
	unsigned int devfn = PCI_DEVFN(0x1f, 0);	// LPC
	device_t lpc_dev = dev_find_slot(0, devfn);	// 0
	if (!lpc_dev)
		return;

	/* We're going to assume, perhaps incorrectly, that if a function
	 * exists it can be disabled. Workarounds for ICH variants that don't
	 * follow this should be done by checking the device ID.
	 */
	if (PCI_SLOT(dev->path.pci.devfn) == 31) {
		index = PCI_FUNC(dev->path.pci.devfn);
	} else if (PCI_SLOT(dev->path.pci.devfn) == 29) {
		index = 8 + PCI_FUNC(dev->path.pci.devfn);
	}

	/* Function 0 is a bit of an exception. */
	if (index == 0) {
		index = 14;
	}

	cur_disable_mask = pci_read_config16(lpc_dev, FUNC_DIS);
	new_disable_mask = cur_disable_mask & ~(1 << index); /* Enable it. */
	if (!dev->enabled) {
		new_disable_mask |= (1 << index); /* Disable it, if desired. */
	}
	if (new_disable_mask != cur_disable_mask) {
		pci_write_config16(lpc_dev, FUNC_DIS, new_disable_mask);
	}
}

struct chip_operations southbridge_intel_i82801ax_ops = {
	CHIP_NAME("Intel ICH/ICH0 (82801AA/AB) Series Southbridge")
	.enable_dev = i82801ax_enable,
};
