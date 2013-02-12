/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "pch.h"
#include <usbdebug.h>
#include <arch/io.h>

static void usb_xhci_init(struct device *dev)
{
	u32 reg32;

	printk(BIOS_DEBUG, "XHCI: Setting up controller.. ");

	/* lock overcurrent map */
	reg32 = pci_read_config32(dev, 0x44);
	reg32 |= 1;
	pci_write_config32(dev, 0x44, reg32);

	/* Enable clock gating */
	reg32 = pci_read_config32(dev, 0x40);
	reg32 &= ~((1 << 20) | (1 << 21));
	reg32 |= (1 << 19) | (1 << 18) | (1 << 17);
	reg32 |= (1 << 10) | (1 << 9) | (1 << 8);
	reg32 |= (1 << 31); /* lock */
	pci_write_config32(dev, 0x40, reg32);

	printk(BIOS_DEBUG, "done.\n");
}

static void xhci_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				((device & 0xffff) << 16) | (vendor & 0xffff));
	}
}

static struct pci_operations xhci_pci_ops = {
	.set_subsystem		= xhci_set_subsystem,
};

static struct device_operations usb_xhci_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= usb_xhci_init,
	.scan_bus		= 0,
	.ops_pci		= &xhci_pci_ops,
};

static const unsigned short pci_device_ids[] = { 0x1e31, 0 };

static const struct pci_driver pch_usb_xhci __pci_driver = {
	.ops	 = &usb_xhci_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
