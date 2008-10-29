/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Linux Networx
 * Copyright (C) 2005 Bitworks
 * Copyright (C) 2007 Ronald G. Minnich <rminnich@gmail.com>
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

#include <types.h>
#include <console.h>
#include <device/device.h>
#include <device/pci.h>
#include <mc146818rtc.h>
#include <string.h>
#include <statictree.h>

/* The plain PCI device uses the standard PCI operations. */

/* TODO: bring in the rest of the v2 code for controlling IDE enable.
 * This is just placeholder code for now
 */

static void i82371eb_isa_init(struct device *dev)
{
	rtc_init(0);
}

static void i82371eb_ide_init(struct device *dev)
{
	unsigned short c;

	/* These should be controlled in the dts. */
	printk(BIOS_DEBUG, "Enabling IDE channel 1\n");
	c = pci_read_config16(dev, 0x40);
	c |= 0x8000;
	pci_write_config16(dev, 0x40, c);

	printk(BIOS_DEBUG, "Enabling IDE channel 2\n");
	c = pci_read_config16(dev, 0x42);
	c |= 0x8000;
	pci_write_config16(dev, 0x42, c);

	printk(BIOS_INFO, "Enabling Legacy IDE\n");
	c = pci_read_config16(dev, PCI_COMMAND);
	c |= 1;
	pci_write_config16(dev, PCI_COMMAND, c);
}

static void i82371eb_acpi_init(struct device *dev)
{
	int smbus_io, pm_io;
	printk(BIOS_DEBUG, "Enabling SMBus.\n");

	smbus_io = 0xFFF0;

	/* iobase addr */
	pci_write_config32(dev, 0x90, smbus_io | 1);
	/* smbus enable */
	pci_write_config8(dev, 0xd2,  (0x4 << 1) | 1);
	/* iospace enable */
	pci_write_config16(dev, PCI_COMMAND, 1);

	printk(BIOS_DEBUG, "Enable Power Management Functions\n");
	pm_io = 0xFF80;
	/* iobase addr */
	pci_write_config32(dev, 0x40, pm_io | 1);

	/* enable pm io address */
	pci_write_config8(dev, 0x80, 1);
}

/*NOTE: We need our own read and set resources for this part! It has 
 * BARS that are not in the normal place (such as SMBUS)
 */
/* You can override or extend each operation as needed for the device. */
struct device_operations i82371eb_isa = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = 0x8086,.device = 0x7000}}},
	.constructor		 = default_device_constructor,
	.phase3_scan		 = 0,
	.phase4_read_resources	 = pci_dev_read_resources,
	.phase4_set_resources	 = pci_dev_set_resources,
	.phase5_enable_resources = pci_dev_enable_resources,
	.phase6_init		 = i82371eb_isa_init,
	.ops_pci		 = &pci_dev_ops_pci,
};

struct device_operations i82371eb_ide = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = 0x8086,.device = 0x7010}}},
	.constructor		 = default_device_constructor,
	.phase3_scan		 = 0,
	.phase4_read_resources	 = pci_dev_read_resources,
	.phase4_set_resources	 = pci_dev_set_resources,
	.phase5_enable_resources = pci_dev_enable_resources,
	.phase6_init		 = i82371eb_ide_init,
	.ops_pci		 = &pci_dev_ops_pci,
};

struct device_operations i82371eb_acpi = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = 0x8086,.device = 0x7113}}},
	.constructor		 = default_device_constructor,
	.phase3_scan		 = 0,
	.phase4_read_resources	 = pci_dev_read_resources,
	.phase4_set_resources	 = pci_dev_set_resources,
	.phase5_enable_resources = pci_dev_enable_resources,
	.phase6_init		 = i82371eb_acpi_init,
	.ops_pci		 = &pci_dev_ops_pci,
};
