/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2012 The Chromium OS Authors.  All rights reserved.
 * Copyright (C) 2013 Sage Electronic Engineering, LLC.
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
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include "soc.h"

static int soc_revision_id = -1;
static int soc_type = -1;

int soc_silicon_revision(void)
{
	if (soc_revision_id < 0)
		soc_revision_id = pci_read_config8(
			dev_find_slot(0, PCI_DEVFN(0x1f, 0)),
			PCI_REVISION_ID);
	return soc_revision_id;
}

int soc_silicon_type(void)
{
	if (soc_type < 0)
		soc_type = pci_read_config8(
			dev_find_slot(0, PCI_DEVFN(0x1f, 0)),
			PCI_DEVICE_ID + 1);
	return soc_type;
}

int soc_silicon_supported(int type, int rev)
{
	int cur_type = soc_silicon_type();
	int cur_rev = soc_silicon_revision();

	switch (type) {
	case SOC_TYPE_RANGELEY:
		if (cur_type == SOC_TYPE_RANGELEY && cur_rev >= rev)
			return 1;
	}

	return 0;
}

/* Set bit in Function Disable register to hide this device */
static void soc_hide_devfn(unsigned devfn)
{
/* TODO Function Disable. */
}




void soc_enable(struct device *dev)
{
	u32 reg32;

	if (!dev->enabled) {
		printk(BIOS_DEBUG, "%s: Disabling device\n",  dev_path(dev));

		/* Ensure memory, IO, and bus master are all disabled */
		reg32 = pci_read_config32(dev, PCI_COMMAND);
		reg32 &= ~(PCI_COMMAND_MASTER |
			   PCI_COMMAND_MEMORY | PCI_COMMAND_IO);
		pci_write_config32(dev, PCI_COMMAND, reg32);

		/* Hide this device if possible */
		soc_hide_devfn(dev->path.pci.devfn);
	} else {
		/* Enable SERR */
		reg32 = pci_read_config32(dev, PCI_COMMAND);
		reg32 |= PCI_COMMAND_SERR;
		pci_write_config32(dev, PCI_COMMAND, reg32);
	}
}

struct chip_operations southbridge_intel_fsp_rangeley_ops = {
	CHIP_NAME("Intel Rangeley Southbridge")
	.enable_dev = soc_enable,
};
