/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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
#include <device/pci_def.h>
#include <arch/io.h>
#include <delay.h>
#include <drivers/intel/gma/int15.h>


#include "ec_oem.c"

#include "mainboard.h"

#define MAX_LCD_BRIGHTNESS	0xd8

static void ec_enable(void)
{
	u16 keymap;
	/* Enable Hotkey SCI */

	/* Fn key map; F1 = [0] ... F12 = [11] */
	keymap = 0x5f1;
	send_ec_oem_command(0x45);
	send_ec_oem_data(0x09); // SCI
	// send_ec_oem_data(0x08); // SMI#
	send_ec_oem_data(keymap >> 8);
	send_ec_oem_data(keymap & 0xff);

	/* Enable Backlight */
	ec_write(0x17, MAX_LCD_BRIGHTNESS);

	/* Notify EC system is in ACPI mode */
	send_ec_oem_command(0x5e);
	send_ec_oem_data(0xea);
	send_ec_oem_data(0x0c);
	send_ec_oem_data(0x01);
}

static void pcie_limit_power(void)
{
#if 0
	// This piece of code needs further debugging as it crashes the
	// machine. It should set the slot numbers and enable power
	// limitation for the PCIe slots.

	struct device *dev;

	dev = dev_find_slot(0, PCI_DEVFN(28,0));
	if (dev) pci_write_config32(dev, 0x54, 0x0010a0e0);

	dev = dev_find_slot(0, PCI_DEVFN(28,1));
	if (dev) pci_write_config32(dev, 0x54, 0x0018a0e0);

	dev = dev_find_slot(0, PCI_DEVFN(28,2));
	if (dev) pci_write_config32(dev, 0x54, 0x0020a0e0);

	dev = dev_find_slot(0, PCI_DEVFN(28,3));
	if (dev) pci_write_config32(dev, 0x54, 0x0028a0e0);
#endif
}


static void mainboard_init(struct device *dev)
{
	ec_enable();
	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_INT_LVDS, GMA_INT15_PANEL_FIT_TXT_STRETCH, 0, 3);
}

// mainboard_enable is executed as first thing after
// enumerate_buses(). Is there no mainboard_init()?
static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
	dev->ops->write_acpi_tables = mainboard_write_acpi_tables;

	pcie_limit_power();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
