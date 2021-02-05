/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

static struct device_operations internal_pcie_gpp_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.scan_bus		= pci_scan_bridge,
	.reset_bus		= pci_bus_reset,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_AMD_FAM17H_MODEL60H_PCIE_GPP_BUSABC,
	0
};

static const struct pci_driver internal_pcie_gpp_driver __pci_driver = {
	.ops			= &internal_pcie_gpp_ops,
	.vendor			= PCI_VENDOR_ID_AMD,
	.devices		= pci_device_ids,
};
