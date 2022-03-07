/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include "i82801gx.h"

static void usb_init(struct device *dev)
{
	/* USB Specification says the device must be Bus Master */
	printk(BIOS_DEBUG, "UHCI: Setting up controller.. ");

	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);

	// Erratum
	pci_write_config8(dev, 0xca, 0x00);

	// Yes. Another Erratum
	pci_or_config8(dev, 0xca, 1 << 0);

	printk(BIOS_DEBUG, "done.\n");
}

static struct device_operations usb_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= usb_init,
	.enable			= i82801gx_enable,
	.ops_pci		= &pci_dev_ops_pci,
};

/* 82801GB/GR/GDH/GBM/GHM/GU (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH/ICH7-U) */
static const struct pci_driver i82801gb_usb1 __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VID_INTEL,
	.device	= 0x27c8,
};

/* 82801GB/GR/GDH/GBM/GHM/GU (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH/ICH7-U) */
static const struct pci_driver i82801gb_usb2 __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VID_INTEL,
	.device	= 0x27c9,
};

/* 82801GB/GR/GDH/GBM/GHM/GU (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH/ICH7-U) */
static const struct pci_driver i82801gb_usb3 __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VID_INTEL,
	.device	= 0x27ca,
};

/* 82801GB/GR/GDH/GBM/GHM/GU (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH/ICH7-U) */
static const struct pci_driver i82801gb_usb4 __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VID_INTEL,
	.device	= 0x27cb,
};
