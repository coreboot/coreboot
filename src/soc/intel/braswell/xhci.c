/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <arch/acpi.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdint.h>
#include <reg_script.h>

#include <soc/iomap.h>
#include <soc/iosf.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/ramstage.h>
#include <soc/xhci.h>

#include "chip.h"

static void xhci_init(struct device *dev)
{
	struct soc_intel_braswell_config *config = dev->chip_info;

	if (config && config->usb_comp_bg) {
		struct reg_script ops[] = {
			REG_IOSF_WRITE(IOSF_PORT_USBPHY, USBPHY_COMPBG,
				config->usb_comp_bg),
			REG_SCRIPT_END
		};
		printk(BIOS_INFO, "Override USB2_COMPBG to: 0x%X\n",
			config->usb_comp_bg);
		reg_script_run(ops);
	}
}

static struct device_operations xhci_device_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= xhci_init,
	.ops_pci		= &soc_pci_ops,
};

static const struct pci_driver soc_xhci __pci_driver = {
	.ops	= &xhci_device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= XHCI_DEVID
};
