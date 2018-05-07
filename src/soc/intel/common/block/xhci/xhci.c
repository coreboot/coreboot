/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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

#include <compiler.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <intelblocks/acpi.h>
#include <intelblocks/xhci.h>

__weak void soc_xhci_init(struct device *dev) { /* no-op */ }

static struct device_operations usb_xhci_ops = {
	.read_resources		= &pci_dev_read_resources,
	.set_resources		= &pci_dev_set_resources,
	.enable_resources	= &pci_dev_enable_resources,
	.init			= soc_xhci_init,
	.ops_pci		= &pci_dev_ops_pci,
	.scan_bus		= &scan_usb_bus,
	.acpi_name		= &soc_acpi_name,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_INTEL_APL_XHCI,
	PCI_DEVICE_ID_INTEL_CNL_LP_XHCI,
	PCI_DEVICE_ID_INTEL_GLK_XHCI,
	PCI_DEVICE_ID_INTEL_SPT_LP_XHCI,
	PCI_DEVICE_ID_INTEL_SPT_H_XHCI,
	PCI_DEVICE_ID_INTEL_KBP_H_XHCI,
	0
};

static const struct pci_driver pch_usb_xhci __pci_driver = {
	.ops	 = &usb_xhci_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices	 = pci_device_ids,
};
