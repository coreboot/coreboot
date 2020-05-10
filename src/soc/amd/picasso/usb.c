/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/pci_ehci.h>
#include <soc/acpi.h>
#include <soc/pci_devs.h>
#include <soc/southbridge.h>
#include <amdblocks/acpimmio.h>

static void picasso_usb_init(struct device *dev)
{
	/* USB overcurrent configuration is programmed inside the FSP */

	printk(BIOS_DEBUG, "%s\n", __func__);
}

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

static struct device_operations usb_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = picasso_usb_init,
	.scan_bus = scan_static_bus,
	.acpi_name = soc_acpi_name,
	.ops_pci = &lops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_AMD_FAM17H_MODEL18H_XHCI0,
	PCI_DEVICE_ID_AMD_FAM17H_MODEL18H_XHCI1,
	PCI_DEVICE_ID_AMD_FAM17H_MODEL20H_XHCI0,
	0
};

static const struct pci_driver usb_0_driver __pci_driver = {
	.ops = &usb_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.devices = pci_device_ids,
};
