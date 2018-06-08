/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
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

#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/ramstage.h>

static void pmc_read_resources(struct device *dev)
{
	unsigned int index = 0;
	struct resource *res;

	/* Get the normal PCI resources of this device. */
	pci_dev_read_resources(dev);

	/* GPE0 */
	res = new_resource(dev, index++);
	res->base = GPE0_BASE_ADDRESS;
	res->size = GPE0_SIZE;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	/* PM1BLK */
	res = new_resource(dev, index++);
	res->base = PM1BLK_BASE_ADDRESS;
	res->size = PM1BLK_SIZE;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	/* Legacy GPIO */
	res = new_resource(dev, index++);
	res->base = LEGACY_GPIO_BASE_ADDRESS;
	res->size = LEGACY_GPIO_SIZE;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static struct device_operations device_ops = {
	.read_resources		= &pmc_read_resources,
	.set_resources		= &pci_dev_set_resources,
	.enable_resources	= &pci_dev_enable_resources,
	.scan_bus		= &scan_lpc_bus,
};

static const struct pci_driver pmc __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= QUARK_V_LPC_DEVICE_ID_0,
};
