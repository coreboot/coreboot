/*
 * This file is part of the coreboot project.
 *
 *  * Copyright (C) 2016 Google Inc.
 *  * Copyright (C) 2017-2018 Intel Corporation.
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

static struct device_operations dsp_dev_ops = {
	.read_resources         = pci_dev_read_resources,
	.set_resources          = pci_dev_set_resources,
	.enable_resources       = pci_dev_enable_resources,
	.ops_pci                = &pci_dev_ops_pci,
	.scan_bus               = enable_static_devices,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_INTEL_APL_AUDIO,
	PCI_DEVICE_ID_INTEL_CNL_AUDIO,
	PCI_DEVICE_ID_INTEL_GLK_AUDIO,
	PCI_DEVICE_ID_INTEL_SKL_AUDIO,
	PCI_DEVICE_ID_INTEL_CNP_H_AUDIO,
	PCI_DEVICE_ID_INTEL_CMP_AUDIO,
	PCI_DEVICE_ID_INTEL_CMP_H_AUDIO,
	PCI_DEVICE_ID_INTEL_ICL_AUDIO,
	PCI_DEVICE_ID_INTEL_TGL_AUDIO,
	PCI_DEVICE_ID_INTEL_MCC_AUDIO,
	PCI_DEVICE_ID_INTEL_JSP_AUDIO,
	0,
};

static const struct pci_driver dsp_driver __pci_driver = {
	.ops    = &dsp_dev_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
