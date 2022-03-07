/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

#include "hudson.h"

static const unsigned short pci_device_ids[] = {
	PCI_DID_AMD_SB900_HDA,
	PCI_DID_AMD_CZ_HDA,
	0
};

static void hda_init(struct device *dev)
{
}

static struct device_operations hda_audio_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = hda_init,
	.ops_pci = &pci_dev_ops_pci,
};

static const struct pci_driver hdaaudio_driver __pci_driver = {
	.ops = &hda_audio_ops,
	.vendor = PCI_VID_AMD,
	.devices = pci_device_ids,
};
