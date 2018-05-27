/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 - 2017 Intel Corporation.
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

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

#include <soc/pci_devs.h>
#include <soc/ramstage.h>

static void npk_init(struct device *dev)
{
	printk(BIOS_DEBUG, "pch: npk_init\n");

	/* TODO */
}

static void pci_npk_read_resources(struct device *dev)
{
	/* Skip NorthPeak enumeration. */
}

static struct device_operations pmc_ops = {
	.read_resources = pci_npk_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.scan_bus = 0,
	.init = npk_init,
	.ops_pci = &soc_pci_ops,
};

static const struct pci_driver pch_pmc __pci_driver = {
	.ops = &pmc_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = NPK_DEVID,
};
