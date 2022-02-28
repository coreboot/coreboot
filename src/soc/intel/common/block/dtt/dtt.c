/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_RPL_DTT,
	PCI_DID_INTEL_MTL_DTT,
	PCI_DID_INTEL_CML_DTT,
	PCI_DID_INTEL_TGL_DTT,
	PCI_DID_INTEL_JSL_DTT,
	PCI_DID_INTEL_ADL_DTT,
	0
};

static struct device_operations dptf_dev_ops = {
	.read_resources			= pci_dev_read_resources,
	.set_resources			= pci_dev_set_resources,
	.enable_resources		= pci_dev_enable_resources,
	.scan_bus			= scan_generic_bus,
	.ops_pci			= &pci_dev_ops_pci,
};

static const struct pci_driver pch_dptf __pci_driver = {
	.ops				= &dptf_dev_ops,
	.vendor				= PCI_VID_INTEL,
	.devices			= pci_device_ids,
};
