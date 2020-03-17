/*
 * This file is part of the coreboot project.
 *
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
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

#include <soc/pci_devs.h>
#include <soc/ramstage.h>

static void usb_xhci_init(struct device *dev)
{
	/* USB XHCI configuration is handled by the FSP */

	printk(BIOS_NOTICE, "pch: %s\n", __func__);

	/* Set the value for PCI command register. */
	pci_write_config16(dev, PCI_COMMAND,
			   PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);
}

static struct device_operations usb_xhci_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = usb_xhci_init,
	.enable = pci_dev_enable_resources,
	.scan_bus = 0,
	.ops_pci = &soc_pci_ops,
};

static const struct pci_driver pch_usb_xhci __pci_driver = {
	.ops = &usb_xhci_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_DENVERTON_XHCI,
};
