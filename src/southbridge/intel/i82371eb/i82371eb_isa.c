/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
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

#include <stdint.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <pc80/isa-dma.h>
#include <pc80/mc146818rtc.h>
#include "i82371eb.h"

static void isa_init(struct device *dev)
{
	u16 reg16;
	u32 reg32;

	/* Initialize the real time clock (RTC). */
	rtc_init(0);

	/* Enable access to all BIOS regions. */
	reg16 = pci_read_config16(dev, XBCS);
	reg16 |= LOWER_BIOS_ENABLE;
	reg16 |= EXT_BIOS_ENABLE;
	reg16 |= EXT_BIOS_ENABLE_1MB;
	reg16 &= ~(WRITE_PROTECT_ENABLE);	/* Disable ROM write access. */
	pci_write_config16(dev, XBCS, reg16);

	/*
	 * The PIIX4 can support the full ISA bus, or the Extended I/O (EIO)
	 * bus, which is a subset of ISA. We select the full ISA bus here.
	 */
	reg32 = pci_read_config32(dev, GENCFG);
	reg32 |= ISA;	/* Select ISA, not EIO. */
	pci_write_config16(dev, GENCFG, reg32);

	/* Initialize ISA DMA. */
	isa_dma_init();
}

static const struct device_operations isa_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= isa_init,
	.scan_bus		= scan_static_bus,	/* TODO: Needed? */
	.enable			= 0,
	.ops_pci		= 0, /* No subsystem IDs on 82371EB! */
};

static const struct pci_driver isa_driver __pci_driver = {
	.ops	= &isa_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82371AB_ISA,
};
