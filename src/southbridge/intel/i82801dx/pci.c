/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "i82801dx.h"

static void pci_init(struct device *dev)
{
	/* Enable pci error detecting */
	uint32_t dword;
	/* System error enable */
	dword = pci_read_config32(dev, 0x04);
	dword |= (1 << 8);	/* SERR# Enable */
	dword |= (1 << 6);	/* Parity Error Response */
	pci_write_config32(dev, 0x04, dword);
}

static struct device_operations pci_ops = {
	.read_resources = pci_bus_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init = pci_init,
	.scan_bus = pci_scan_bridge,
};

/* 82801DB */
static const struct pci_driver pci_driver_db __pci_driver = {
	.ops = &pci_ops,
	.vendor = PCI_VID_INTEL,
	.device = PCI_DID_INTEL_82801DB_PCI,
};

/* 82801DBM/DBL */
static const struct pci_driver pci_driver_dbm __pci_driver = {
	.ops = &pci_ops,
	.vendor = PCI_VID_INTEL,
	.device = PCI_DID_INTEL_82801DBM_PCI,
};
