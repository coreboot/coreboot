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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "cs5530.h"

/**
 * Initialize the IDE controller.
 *
 * Depending on the configuration variables 'ide0_enable' and 'ide1_enable'
 * enable or disable the primary and secondary IDE interface, respectively.
 *
 * @param dev The device to use.
 */
static void ide_init(struct device *dev)
{
	uint8_t reg8;
	struct southbridge_amd_cs5530_config *conf = dev->chip_info;

	reg8 = pci_read_config8(dev, DECODE_CONTROL_REG2);

	/* Enable/disable the primary IDE interface. */
	if (conf->ide0_enable) {
		reg8 |= PRIMARY_IDE_ENABLE;
	} else {
		reg8 &= ~(PRIMARY_IDE_ENABLE);
	}

	/* Enable/disable the secondary IDE interface. */
	if (conf->ide1_enable) {
		reg8 |= SECONDARY_IDE_ENABLE;
	} else {
		reg8 &= ~(SECONDARY_IDE_ENABLE);
	}

	pci_write_config8(dev, DECODE_CONTROL_REG2, reg8);

	printk(BIOS_INFO, "%s IDE interface %s\n", "Primary",
		    conf->ide0_enable ? "enabled" : "disabled");
	printk(BIOS_INFO, "%s IDE interface %s\n", "Secondary",
		    conf->ide1_enable ? "enabled" : "disabled");
}

static struct device_operations ide_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= ide_init,
	.enable			= 0,
	.scan_bus		= scan_static_bus,
	.ops_pci		= 0,
};

static const struct pci_driver ide_driver __pci_driver = {
	.ops	= &ide_ops,
	.vendor = PCI_VENDOR_ID_CYRIX,
	.device = PCI_DEVICE_ID_CYRIX_5530_IDE,
};
