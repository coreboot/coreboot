/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
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
#include <delay.h>

// #define RTL8168_DEBUG 1

static void nic_init(struct device *dev)
{
	printk(BIOS_DEBUG, "Initializing RTL8168 Gigabit Ethernet\n");
	// Nothing to do yet, but this has to be here to keep 
	// coreboot from trying to execute an option ROM.

#ifdef RTL8168_DEBUG
	u8 reg8;

	printk(BIOS_DEBUG, "Resetting device... ");
	pci_write_config8(dev, 0x37, (1 << 4));
	do {
		reg8 = pci_read_config8(dev, 0x37);
		reg8 &= (1 << 4);
	} while (reg8);
	printk(BIOS_DEBUG, "ok\n");

	printk(BIOS_DEBUG, "Eeprom Auto-Load... ");
	reg8 = pci_read_config8(dev, 0x50);
	reg8 &= 0x3f;
	reg8 |= (1 << 6);
	pci_write_config8(dev, 0x50, reg8);
	mdelay(3);
	printk(BIOS_DEBUG, "ok\n");
#endif
}

static struct device_operations nic_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= nic_init,
	.scan_bus		= 0,
};

static const struct pci_driver rtl8168_nic __pci_driver = {
	.ops	= &nic_ops,
	.vendor	= 0x10ec,
	.device	= 0x8168,
};

#ifdef RTL8168_DEBUG
static const struct pci_driver rtl8129_nic __pci_driver = {
	.ops	= &nic_ops,
	.vendor	= 0x10ec,
	.device	= 0x8129,
};
#endif

