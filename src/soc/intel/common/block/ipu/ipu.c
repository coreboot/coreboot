/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/pci.h>
#include <device/pci_ids.h>

struct device_operations ipu_pci_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.scan_bus		= scan_generic_bus,
	.ops_pci		= &pci_dev_ops_pci,
};

static const uint16_t pci_device_ids[] = {
	PCI_DID_INTEL_TGL_IPU,
	PCI_DID_INTEL_TGL_H_IPU,
	PCI_DID_INTEL_JSL_IPU,
	PCI_DID_INTEL_ADL_IPU,
	PCI_DID_INTEL_ADL_N_IPU,
	0
};

static const struct pci_driver intel_ipu __pci_driver = {
	.ops		= &ipu_pci_ops,
	.vendor		= PCI_VID_INTEL,
	.devices	= pci_device_ids,
};
