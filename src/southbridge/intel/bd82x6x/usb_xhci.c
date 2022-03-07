/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "pch.h"
#include <device/pci_ehci.h>
#include <device/pci_ops.h>
#include "chip.h"

static void usb_xhci_init(struct device *dev)
{
	u32 reg32;
	struct southbridge_intel_bd82x6x_config *config = dev->chip_info;

	printk(BIOS_DEBUG, "XHCI: Setting up controller.. ");

	if (config->xhci_overcurrent_mapping)
		pci_write_config32(dev, XOCM, config->xhci_overcurrent_mapping);

	/* lock overcurrent map */
	pci_or_config32(dev, 0x44, 1);

	pci_write_config32(dev, XUSB2PRM, config->xhci_switchable_ports);
	pci_write_config32(dev, USB3PRM, config->superspeed_capable_ports);

	/* Enable clock gating */
	reg32 = pci_read_config32(dev, 0x40);
	reg32 &= ~((1 << 20) | (1 << 21));
	reg32 |= (1 << 19) | (1 << 18) | (1 << 17);
	reg32 |= (1 << 10) | (1 << 9) | (1 << 8);
	reg32 |= (1 << 31); /* lock */
	pci_write_config32(dev, 0x40, reg32);

	printk(BIOS_DEBUG, "done.\n");
}

static const char *xhci_acpi_name(const struct device *dev)
{
	return "XHC";
}

static struct device_operations usb_xhci_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= usb_xhci_init,
	.ops_pci		= &pci_dev_ops_pci,
	.acpi_name		= xhci_acpi_name,
};

static const unsigned short pci_device_ids[] = { 0x1e31, 0 };

static const struct pci_driver pch_usb_xhci __pci_driver = {
	.ops	 = &usb_xhci_ops,
	.vendor	 = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
