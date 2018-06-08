/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2011 Google Inc.
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
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <watchdog.h>

  //
  //  Disable PCH Watchdog timer at SB_RCBA+0x3410
  //
  //  Mmio32((MmPci32(0, 0, 0x1F, 0, 0xF0) & ~BIT0), 0x3410) |= 0x20;
  //
void watchdog_off(void)
{
	struct device *dev;
	unsigned long value, base;

	/* Turn off the ICH7 watchdog. */
	dev = dev_find_slot(0, PCI_DEVFN(0x1f, 0));

	/* Enable I/O space. */
	value = pci_read_config16(dev, 0x04);
	value |= (1 << 10);
	pci_write_config16(dev, 0x04, value);

	/* Get TCO base. */
	base = (pci_read_config32(dev, 0x40) & 0x0fffe) + 0x60;

	/* Disable the watchdog timer. */
	value = inw(base + 0x08);
	value |= 1 << 11;
	outw(value, base + 0x08);

	/* Clear TCO timeout status. */
	outw(0x0008, base + 0x04);
	outw(0x0002, base + 0x06);

	printk(BIOS_DEBUG, "PCH watchdog disabled\n");
}
