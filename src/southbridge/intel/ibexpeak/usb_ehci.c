/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "pch.h"
#include <device/pci_ehci.h>
#include <device/mmio.h>
#include <device/pci_ops.h>

static void usb_ehci_init(struct device *dev)
{
	u32 reg32;
	struct resource *res;
	u8 access_cntl;

	/* Disable Wake on Disconnect in RMH */
	reg32 = RCBA32(0x35b0);
	reg32 |= 0x22;
	RCBA32(0x35b0) = reg32;

	printk(BIOS_DEBUG, "EHCI: Setting up controller.. ");

	pci_write_config32(dev, 0x84, 0x130c8911);
	pci_write_config32(dev, 0x88, 0xa0);
	pci_write_config32(dev, 0xf4, 0x80808588);
	pci_write_config32(dev, 0xf4, 0x00808588);
	pci_write_config32(dev, 0xf4, 0x00808588);
	pci_write_config32(dev, 0xfc, 0x301b1728);

	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);

	access_cntl = pci_read_config8(dev, 0x80);

	/* Enable writes to protected registers. */
	pci_write_config8(dev, 0x80, access_cntl | 1);

	res = probe_resource(dev, PCI_BASE_ADDRESS_0);
	if (res) {
		/* Number of ports and companion controllers.  */
		reg32 = read32((u32 *)(uintptr_t)(res->base + 4));
		write32((u32 *)(uintptr_t)(res->base + 4),
			(reg32 & 0xfff00000) | 2);
	}

	/* Restore protection. */
	pci_write_config8(dev, 0x80, access_cntl);

	printk(BIOS_DEBUG, "done.\n");
}

static void usb_ehci_set_subsystem(struct device *dev, unsigned int vendor,
				   unsigned int device)
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
	.set_subsystem = &usb_ehci_set_subsystem,
};

static struct device_operations usb_ehci_ops = {
	.read_resources = pci_ehci_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init = usb_ehci_init,
	.ops_pci = &lops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_IBEXPEAK_EHCI_1,
	PCI_DID_INTEL_IBEXPEAK_EHCI_2,
	0
};

static const struct pci_driver pch_usb_ehci __pci_driver = {
	.ops = &usb_ehci_ops,
	.vendor = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
