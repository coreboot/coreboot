/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include "i82801gx.h"
#include "sata.h"

void i82801gx_enable(struct device *dev)
{
	u32 reg32;

	/* Enable SERR */
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_SERR;
	pci_write_config32(dev, PCI_COMMAND, reg32);

	if (dev->path.pci.devfn == PCI_DEVFN(31, 2)) {
		printk(BIOS_DEBUG, "Set SATA mode early\n");
		sata_enable(dev);
	}
}

struct chip_operations southbridge_intel_i82801gx_ops = {
	CHIP_NAME("Intel ICH7/ICH7-M (82801Gx) Series Southbridge")
	.enable_dev = i82801gx_enable,
};
