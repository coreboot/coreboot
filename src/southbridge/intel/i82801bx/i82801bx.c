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
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include "i82801bx.h"

void i82801bx_enable(struct device *dev)
{
	u16 reg16, index;
	struct device *lpc_dev;

	/* Search for the 82801BA/BAM LPC device (D31:F0) on PCI bus 0. */
	lpc_dev = dev_find_slot(0, PCI_DEVFN(0x1f, 0));
	if (!lpc_dev)
		return;

	index = PCI_FUNC(dev->path.pci.devfn);

	reg16 = pci_read_config16(lpc_dev, FUNC_DIS);
	reg16 &= ~(1 << index);         /* Enable device. */
	if (!dev->enabled)
		reg16 |= (1 << index);  /* Disable device, if desired. */
	pci_write_config16(lpc_dev, FUNC_DIS, reg16);
}

struct chip_operations southbridge_intel_i82801bx_ops = {
	CHIP_NAME("Intel ICH2 (82801Bx) Series Southbridge")
	.enable_dev = i82801bx_enable,
};
