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

#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <soc/ramstage.h>
#include <soc/xhci.h>
#include <soc/cpu.h>

static struct device_operations usb_xhci_ops = {
	.read_resources		= &pci_dev_read_resources,
	.set_resources		= &pci_dev_set_resources,
	.enable_resources	= &pci_dev_enable_resources,
	.ops_pci		= &soc_pci_ops,
};

static const unsigned short pci_device_ids[] = {
	0x9d2f, /* SunRisePoint LP */
	0
};

static const struct pci_driver pch_usb_xhci __pci_driver = {
	.ops	 = &usb_xhci_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
