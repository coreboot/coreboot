/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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

#include <arch/io.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <soc/pch.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>

u8 pch_revision(void)
{
	return pci_read_config8(PCH_DEV_LPC, PCI_REVISION_ID);
}

u16 pch_type(void)
{
	return pci_read_config16(PCH_DEV_LPC, PCI_DEVICE_ID);
}

#if ENV_RAMSTAGE
void pch_enable_dev(device_t dev)
{
	/* FSP should implement routines to disable PCH IPs */
	u32 reg32;

	/* These devices need special enable/disable handling */
	switch (PCI_SLOT(dev->path.pci.devfn)) {
	case PCH_DEV_SLOT_PCIE:
		return;
	}

	if (!dev->enabled) {
		printk(BIOS_DEBUG, "%s: Disabling device\n", dev_path(dev));

		/* Ensure memory, io, and bus master are all disabled */
		reg32 = pci_read_config32(dev, PCI_COMMAND);
		reg32 &= ~(PCI_COMMAND_MASTER |
			   PCI_COMMAND_MEMORY | PCI_COMMAND_IO);
		pci_write_config32(dev, PCI_COMMAND, reg32);
	} else {
		/* Enable SERR */
		reg32 = pci_read_config32(dev, PCI_COMMAND);
		reg32 |= PCI_COMMAND_SERR;
		pci_write_config32(dev, PCI_COMMAND, reg32);
	}
}

#endif
