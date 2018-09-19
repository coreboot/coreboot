/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Eric Biederman
 * Copyright (C) 2006-2010 coresystems GmbH
 * Copyright (C) 2015-2016 Intel Corporation.
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

#include <console/uart.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <soc/pci_devs.h>

static void uart_read_resources(struct device *dev)
{
	struct resource *res;

	/* Read the resources */
	pci_dev_read_resources(dev);

	/* Set the debug port configuration */
	res = find_resource(dev, PCI_BASE_ADDRESS_0);
	res->base = uart_platform_base(CONFIG_UART_FOR_CONSOLE);
	res->size = 0x100;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static struct device_operations device_ops = {
	.read_resources		= &uart_read_resources,
	.set_resources		= &pci_dev_set_resources,
	.enable_resources	= &pci_dev_enable_resources,
};

static const struct pci_driver uart_driver __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= HSUART_DEVID,
};
