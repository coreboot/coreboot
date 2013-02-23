/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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

static void usb_init(struct device *dev)
{
	u32 reg32;
	u8 reg8;

	/* USB Specification says the device must be Bus Master */
	printk(BIOS_DEBUG, "UHCI: Setting up controller.. ");

	reg32 = pci_read_config32(dev, PCI_COMMAND);
	pci_write_config32(dev, PCI_COMMAND, reg32 | PCI_COMMAND_MASTER);

	reg8 = pci_read_config8(dev, 0xca);
	reg8 |= (1 << 0);
	pci_write_config8(dev, 0xca, reg8);

	printk(BIOS_DEBUG, "done.\n");
}

static struct device_operations usb_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = usb_init,
	.enable           = 0,
	.ops_pci          = 0,
};

static const struct pci_driver via_usb_driver __pci_driver = {
	.ops    = &usb_ops,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = 0x3038,
};
