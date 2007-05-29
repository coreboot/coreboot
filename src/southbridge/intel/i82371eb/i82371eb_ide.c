/*
 * This file is part of the LinuxBIOS project.
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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "i82371eb.h"

/**
 * Initialize the IDE controller.
 *
 * Depending on the configuration variables 'ide0_enable' and 'ide1_enable'
 * we enable or disable the primary and secondary IDE interface, respectively.
 *
 * @param dev The device to use.
 */
static void ide_init(struct device *dev)
{
	uint16_t reg;
	struct southbridge_intel_i82371eb_config *conf;

	conf = dev->chip_info;

	/* Enable/disable the primary IDE interface. */
	reg = pci_read_config16(dev, IDETIM_PRI);
	if (conf->ide0_enable) {
		reg |= IDE_DECODE_ENABLE;
		print_info("Primary IDE interface enabled\n");
	} else {
		reg &= ~(IDE_DECODE_ENABLE);
		print_info("Primary IDE interface disabled\n");
	}
	pci_write_config16(dev, IDETIM_PRI, reg);

	/* Enable/disable the secondary IDE interface. */
	reg = pci_read_config16(dev, IDETIM_SEC);
	if (conf->ide1_enable) {
		reg |= IDE_DECODE_ENABLE;
		print_info("Secondary IDE interface enabled\n");
	} else {
		reg &= ~(IDE_DECODE_ENABLE);
		print_info("Secondary IDE interface disabled\n");
	}
	pci_write_config16(dev, IDETIM_SEC, reg);
}

/* There are no subsystem IDs on the Intel 82371EB. */
static struct pci_operations lops_pci = {
	// .set_subsystem = 0,
};

static struct device_operations ide_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= ide_init,
	.scan_bus		= 0,
	.ops_pci		= &lops_pci,
};

static struct pci_driver ide_driver __pci_driver = {
	.ops	= &ide_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82371AB_IDE,
};
