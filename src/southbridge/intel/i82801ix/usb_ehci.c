/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
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
#include "i82801ix.h"
#include <usbdebug.h>

static void usb_ehci_init(struct device *dev)
{
	u32 reg32;

	printk(BIOS_DEBUG, "EHCI: Setting up controller.. ");
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_MASTER;
	pci_write_config32(dev, PCI_COMMAND, reg32);

	printk(BIOS_DEBUG, "done.\n");
}

static void usb_ehci_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	u8 access_cntl;

	access_cntl = pci_read_config8(dev, 0x80);

	/* Enable writes to protected registers. */
	pci_write_config8(dev, 0x80, access_cntl | 1);

	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				((device & 0xffff) << 16) | (vendor & 0xffff));
	}

	/* Restore protection. */
	pci_write_config8(dev, 0x80, access_cntl);
}

static void usb_ehci_set_resources(struct device *dev)
{
#if CONFIG_USBDEBUG
	struct resource *res;
	u32 base;
	u32 usb_debug;

	usb_debug = get_ehci_debug();
	set_ehci_debug(0);
#endif
	pci_dev_set_resources(dev);

#if CONFIG_USBDEBUG
	res = find_resource(dev, 0x10);
	set_ehci_debug(usb_debug);
	if (!res) return;
	base = res->base;
	set_ehci_base(base);
	report_resource_stored(dev, res, "");
#endif
}


static const unsigned short pci_device_ids[] = {
	0x293a,
	0x293c,
	0
};

static struct pci_operations lops_pci = {
	.set_subsystem	= &usb_ehci_set_subsystem,
};

static struct device_operations usb_ehci_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= usb_ehci_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= usb_ehci_init,
	.scan_bus		= 0,
	.ops_pci		= &lops_pci,
};

static const struct pci_driver pch_usb_ehci1 __pci_driver = {
	.ops	= &usb_ehci_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
