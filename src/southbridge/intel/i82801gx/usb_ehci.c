/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "i82801gx.h"
#include <device/pci_ehci.h>
#include <device/mmio.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>

static void usb_ehci_init(struct device *dev)
{
	struct resource *res;
	u8 *base;
	u32 reg32;

	printk(BIOS_DEBUG, "EHCI: Setting up controller.. ");
	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER | PCI_COMMAND_SERR);

	pci_or_config32(dev, 0xdc, (1 << 31) | (1 << 27));

	pci_update_config32(dev, 0xfc, ~(3 << 2), (2 << 2) | (1 << 29) | (1 << 17));

	/* Clear any pending port changes */
	res = find_resource(dev, PCI_BASE_ADDRESS_0);
	base = res2mmio(res, 0, 0);
	reg32 = read32(base + 0x24) | (1 << 2);
	write32(base + 0x24, reg32);

	/* workaround */
	pci_or_config8(dev, 0x84, 1 << 4);

	printk(BIOS_DEBUG, "done.\n");
}

static void usb_ehci_set_subsystem(struct device *dev, unsigned int vendor, unsigned int device)
{
	u8 access_cntl;

	access_cntl = pci_read_config8(dev, 0x80);

	/* Enable writes to protected registers. */
	pci_write_config8(dev, 0x80, access_cntl | 1);

	pci_dev_set_subsystem(dev, vendor, device);

	/* Restore protection. */
	pci_write_config8(dev, 0x80, access_cntl);
}

static struct pci_operations lops_pci = {
	.set_subsystem	= &usb_ehci_set_subsystem,
};

static struct device_operations usb_ehci_ops = {
	.read_resources		= pci_ehci_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= usb_ehci_init,
	.enable			= i82801gx_enable,
	.ops_pci		= &lops_pci,
};

/* 82801GB/GR/GDH/GBM/GHM/GU (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH/ICH7-U) */
static const struct pci_driver i82801gx_usb_ehci __pci_driver = {
	.ops	= &usb_ehci_ops,
	.vendor	= PCI_VID_INTEL,
	.device	= 0x27cc,
};
