/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/mmio.h>
#include <device/pci_ehci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/pci.h>

#include "pch.h"

static void usb_ehci_init(struct device *dev)
{
	u32 reg32;

	/* Disable Wake on Disconnect in RMH */
	reg32 = RCBA32(RMHWKCTL);
	reg32 |= 0x22;
	RCBA32(RMHWKCTL) = reg32;

	printk(BIOS_DEBUG, "EHCI: Setting up controller.. ");

	/* For others, done in MRC.  */
#if CONFIG(USE_NATIVE_RAMINIT)
	pci_write_config32(dev, 0x84, 0x930c8811);
	pci_write_config32(dev, 0x88, 0x24000d30);
	pci_write_config32(dev, 0xf4, 0x80408588);
	pci_write_config32(dev, 0xf4, 0x80808588);
	pci_write_config32(dev, 0xf4, 0x00808588);
	pci_write_config32(dev, 0xfc, 0x205b1708);
#endif

	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);
	//pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_SERR);

	/* For others, done in MRC.  */
#if CONFIG(USE_NATIVE_RAMINIT)
	struct resource *res;
	u8 access_cntl;

	access_cntl = pci_read_config8(dev, 0x80);

	/* Enable writes to protected registers. */
	pci_write_config8(dev, 0x80, access_cntl | 1);

	res = probe_resource(dev, PCI_BASE_ADDRESS_0);
	if (res) {
		/* Number of ports and companion controllers.  */
		reg32 = read32p((uintptr_t)(res->base + 4));
		write32p((uintptr_t)(res->base + 4),
			(reg32 & 0xfff00000) | 3);
	}

	/* Restore protection. */
	pci_write_config8(dev, 0x80, access_cntl);
#endif

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

static const char *usb_ehci_acpi_name(const struct device *dev)
{
	switch (dev->path.pci.devfn) {
	case PCI_DEVFN(0x1a, 0):
		return "EHC2";
	case PCI_DEVFN(0x1d, 0):
		return "EHC1";
	}
	return NULL;
}

static struct pci_operations lops_pci = {
	.set_subsystem	= &usb_ehci_set_subsystem,
};

static struct device_operations usb_ehci_ops = {
	.read_resources		= pci_ehci_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= usb_ehci_init,
	.ops_pci		= &lops_pci,
	.acpi_name		= usb_ehci_acpi_name,
};

static const unsigned short pci_device_ids[] = { 0x1c26, 0x1c2d, 0x1e26, 0x1e2d,
						 0 };

static const struct pci_driver pch_usb_ehci __pci_driver = {
	.ops	 = &usb_ehci_ops,
	.vendor	 = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
