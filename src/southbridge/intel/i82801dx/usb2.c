/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Tyan
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <usbdebug.h>
#include "i82801dx.h"

static void usb2_init(struct device *dev)
{
	u32 cmd;
	printk(BIOS_DEBUG, "USB: Setting up controller.. ");
	cmd = pci_read_config32(dev, PCI_COMMAND);
	pci_write_config32(dev, PCI_COMMAND,
			   cmd | PCI_COMMAND_IO | PCI_COMMAND_MEMORY |
			   PCI_COMMAND_MASTER | PCI_COMMAND_INVALIDATE);
	printk(BIOS_DEBUG, "done.\n");
}

static struct device_operations usb2_ops = {
	.read_resources = pci_ehci_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = usb2_init,
	.scan_bus = 0,
	.enable = i82801dx_enable,
};

/* 82801DB/DBM USB 2.0 */
static const struct pci_driver usb2_driver __pci_driver = {
	.ops = &usb2_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_82801DB_EHCI,
};
