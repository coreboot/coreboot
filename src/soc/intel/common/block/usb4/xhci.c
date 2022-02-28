/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/acpi.h>
#include <soc/pci_devs.h>

static const char *usb4_xhci_acpi_name(const struct device *dev)
{
	if (dev->path.type != DEVICE_PATH_PCI)
		return NULL;

	return "TXHC";
}

static struct device_operations usb4_xhci_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.ops_pci		= &pci_dev_ops_pci,
	.scan_bus		= scan_static_bus,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name		= usb4_xhci_acpi_name,
#endif
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_RPP_P_TCSS_XHCI,
	PCI_DID_INTEL_MTL_M_TCSS_XHCI,
	PCI_DID_INTEL_MTL_P_TCSS_XHCI,
	PCI_DID_INTEL_TGP_TCSS_XHCI,
	PCI_DID_INTEL_TGP_H_TCSS_XHCI,
	PCI_DID_INTEL_ADP_TCSS_XHCI,
	PCI_DID_INTEL_ADP_N_TCSS_XHCI,
	0
};

static const struct pci_driver usb4_xhci __pci_driver = {
	.ops	 = &usb4_xhci_ops,
	.vendor	 = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
