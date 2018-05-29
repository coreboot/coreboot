/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2011 Google Inc.
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
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <watchdog.h>
#include "soc.h"

void watchdog_off(void)
{
	struct device *dev;
	u32 value, abase;

	/* Turn off the watchdog. */
	dev = dev_find_slot(0, PCI_DEVFN(0x1f, 0));

	/* Enable I/O space. */
	value = pci_read_config16(dev, 0x04);
	value |= 1;
	pci_write_config16(dev, 0x04, value);

	/* Get TCO base. */
	abase = (pci_read_config32(dev, ABASE) & ~0xf);

	/* Disable the watchdog timer. */
	value = inw(abase + 0x68);
	value |= 1 << 11;
	outw(value, abase + 0x68);

	/* Clear TCO timeout status. */
	outw(0x0008, abase + 0x64);
	outw(0x0002, abase + 0x66);

	printk(BIOS_DEBUG, "TCO Watchdog disabled\n");
}
