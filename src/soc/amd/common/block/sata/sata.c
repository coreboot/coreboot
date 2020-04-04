/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <amdblocks/sata.h>

void __weak soc_enable_sata_features(struct device *dev) { }

static struct device_operations sata_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = soc_enable_sata_features,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_AMD_CZ_SATA,
	PCI_DEVICE_ID_AMD_CZ_SATA_AHCI,
	0
};

static const struct pci_driver sata0_driver __pci_driver = {
	.ops = &sata_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.devices = pci_device_ids,
};
