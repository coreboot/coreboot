/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "hudson.h"

static void pcie_init(struct device *dev)
{
}

static struct pci_operations lops_pci = {
	.set_subsystem = 0,
};

static struct device_operations pci_ops = {
	.read_resources = pci_bus_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init = pcie_init,
	.scan_bus = pci_scan_bridge,
	.reset_bus = pci_bus_reset,
	.ops_pci = &lops_pci,
};

static const struct pci_driver pciea_driver __pci_driver = {
	.ops = &pci_ops,
	.vendor = PCI_VID_AMD,
	.device = PCI_DID_AMD_SB900_PCIEA,
};

static const struct pci_driver pcieb_driver __pci_driver = {
	.ops = &pci_ops,
	.vendor = PCI_VID_AMD,
	.device = PCI_DID_AMD_SB900_PCIEB,
};
static const struct pci_driver pciec_driver __pci_driver = {
	.ops = &pci_ops,
	.vendor = PCI_VID_AMD,
	.device = PCI_DID_AMD_SB900_PCIEC,
};
static const struct pci_driver pcied_driver __pci_driver = {
	.ops = &pci_ops,
	.vendor = PCI_VID_AMD,
	.device = PCI_DID_AMD_SB900_PCIED,
};
