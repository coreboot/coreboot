/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/tsn_gbe.h>

static struct device_operations gbe_tsn_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
};

static const unsigned short gbe_tsn_device_ids[] = { 0x4b32, 0x4ba0, 0x4bb0, 0 };

static const struct pci_driver gbe_tsn_driver __pci_driver = {
	.ops     = &gbe_tsn_ops,
	.vendor  = PCI_VID_INTEL,
	.devices = gbe_tsn_device_ids,
};
