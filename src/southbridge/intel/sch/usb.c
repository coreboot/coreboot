/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2010 coresystems GmbH
 * Copyright (C) 2009-2010 iWave Systems
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
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

	/* USB Specification says the device must be Bus Master. */
	printk(BIOS_DEBUG, "UHCI: Setting up controller.. ");

	reg32 = pci_read_config32(dev, PCI_COMMAND);
	pci_write_config32(dev, PCI_COMMAND, reg32 | PCI_COMMAND_MASTER);
	/* Disable clock gating. */
	reg32 = pci_read_config32(dev, 0xFC);
	reg32 |= (1 << 2);
	pci_write_config32(dev, 0xFC, reg32);
	pci_write_config8(dev,0xF8,0x86);
	pci_write_config8(dev,0xF9,0x0F);
	pci_write_config8(dev,0xFA,0x06);
	reg32 = pci_read_config32(dev, 0x4);
	printk(BIOS_DEBUG, "PCI_COMMAND %x.\n",reg32);
	reg32 = pci_read_config32(dev, 0x20);
	printk(BIOS_DEBUG, "PCI_BASE %x.\n",reg32);
	reg32 = pci_read_config32(dev, 0xFC);
	printk(BIOS_DEBUG, "PCI_FD %x.\n",reg32);
	printk(BIOS_DEBUG, "done.\n");
}

static void usb_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				((device & 0xffff) << 16) | (vendor & 0xffff));
	}
}

static struct pci_operations usb_pci_ops = {
	.set_subsystem = usb_set_subsystem,
};

static struct device_operations usb_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= usb_init,
	.scan_bus		= 0,
	.ops_pci		= &usb_pci_ops,
};

static const struct pci_driver sch_usb0 __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x8114,
};

static const struct pci_driver sch_usb1 __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x8115,
};

static const struct pci_driver sch_usb2 __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x8116,
};
