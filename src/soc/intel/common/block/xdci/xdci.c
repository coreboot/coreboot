/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015-2017 Intel Corporation.
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

#include <arch/io.h>
#include <compiler.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/xdci.h>
#include <security/vboot/vboot_common.h>

__weak void soc_xdci_init(struct device *dev) { /* no-op */ }

/* Only allow xDCI controller in developer mode if VBOOT is enabled */
int xdci_can_enable(void)
{
	if (IS_ENABLED(CONFIG_VBOOT))
		return vboot_developer_mode_enabled() ?  1 : 0;
	else
		return 1;
}

static struct device_operations usb_xdci_ops = {
	.read_resources		= &pci_dev_read_resources,
	.set_resources		= &pci_dev_set_resources,
	.enable_resources	= &pci_dev_enable_resources,
	.init			= soc_xdci_init,
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_INTEL_APL_XDCI,
	PCI_DEVICE_ID_INTEL_CNL_LP_XDCI,
	PCI_DEVICE_ID_INTEL_GLK_XDCI,
	PCI_DEVICE_ID_INTEL_SPT_LP_XDCI,
	0
};

static const struct pci_driver pch_usb_xdci __pci_driver = {
	.ops	 = &usb_xdci_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices	 = pci_device_ids,
};
