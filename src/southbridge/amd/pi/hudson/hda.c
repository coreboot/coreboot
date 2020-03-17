/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

#include "hudson.h"

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_AMD_SB900_HDA,
	PCI_DEVICE_ID_AMD_CZ_HDA,
	0
};

static void hda_init(struct device *dev)
{
}

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

static struct device_operations hda_audio_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = hda_init,
	.scan_bus = 0,
	.ops_pci = &lops_pci,
};

static const struct pci_driver hdaaudio_driver __pci_driver = {
	.ops = &hda_audio_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.devices = pci_device_ids,
};
