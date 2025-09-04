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
	if (CONFIG(USE_NATIVE_RAMINIT)) {
		pci_update_config32(dev, 0x84, 0x600, 0x10040010);
		pci_or_config32(dev, 0x88, 0x20080d20);

		pci_or_config32(dev, 0xf4, BIT(31));
		pci_update_config32(dev, 0xf4, (u32)~BIT(22), BIT(23));
		pci_and_config32(dev, 0xf4, (u32)~BIT(31));

		pci_or_config32(dev, 0xfc, BIT(17));
	}

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
	if (dev->path.type == DEVICE_PATH_USB) {
		switch (dev->path.usb.port_type) {
		case 0:
			if (dev->path.usb.port_id == 0)
				/* Root Hub */
				return "HUB7";
			else if (dev->path.usb.port_id == 1)
				/* Rate Matching Hub on Port 1 */
				return "URMH";
			break;
		case 2:
			/* USB2 ports */
			switch (dev->path.usb.port_id) {
			case 0: return "PRT0";
			case 1: return "PRT1";
			case 2: return "PRT2";
			case 3: return "PRT3";
			case 4: return "PRT4";
			case 5: return "PRT5";
			case 6: return "PRT6";
			case 7: return "PRT7";
			}
			break;
		}
		return NULL;
	} else if (dev->path.type == DEVICE_PATH_PCI) {
		switch (dev->path.pci.devfn) {
		case PCI_DEVFN(0x1a, 0):
			return "EHC2";
		case PCI_DEVFN(0x1d, 0):
			return "EHC1";
		}
	}

	return NULL;
}

static struct pci_operations lops_pci = {
	.set_subsystem	= &usb_ehci_set_subsystem,
};

struct device_operations bd82x6x_usb_ehci_ops = {
	.read_resources		= pci_ehci_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.scan_bus		= scan_static_bus,
	.init			= usb_ehci_init,
	.ops_pci		= &lops_pci,
	.acpi_name		= usb_ehci_acpi_name,
};
