/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 secunet Security Networks AG
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
#include <device/device.h>
#include <device/pci.h>

static void disable_aspm(struct device *const dev)
{
	printk(BIOS_INFO, "Disabling ASPM for %s [%04x/%04x]\n",
	       dev_path(dev), dev->vendor, dev->device);
	dev->disable_pcie_aspm = 1;
}

static struct device_operations asmedia_noaspm_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.enable			= disable_aspm,
};

static const unsigned short pci_device_ids[] = {
	0x0611, /* ASM1061 SATA IDE Controller */
	0
};

static const struct pci_driver asmedia_noaspm __pci_driver = {
	.ops	 = &asmedia_noaspm_ops,
	.vendor	 = 0x1b21,
	.devices = pci_device_ids,
};
