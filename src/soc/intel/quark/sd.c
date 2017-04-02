/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Intel Corporation
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
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/sdhci.h>
#include <device/storage.h>
#include <soc/storage_test.h>

static void init(struct device *dev)
{
	/* Run the SD test */
	if (IS_ENABLED(CONFIG_STORAGE_TEST)) {
		uint32_t bar;
		uint32_t previous_bar;
		uint16_t previous_command;

		bar = storage_test_init(dev, &previous_bar, &previous_command);
		storage_test(bar, 0);
		storage_test_complete(dev, previous_bar, previous_command);
	}
}

static const struct device_operations device_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= init,
};

static const struct pci_driver pmc __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x08A7,
};
