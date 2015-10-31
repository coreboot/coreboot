/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Inc.
 * Copyright (C) 2015 Intel Corporation
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
#include <stdlib.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>

static int pch_uart_is_debug(struct device *dev)
{
	return dev->path.pci.devfn == PCH_DEVFN_UART2;
}

static void pch_uart_read_resources(struct device *dev)
{
	pci_dev_read_resources(dev);

	/* Set the configured UART base address for the debug port */
	if (IS_ENABLED(CONFIG_UART_DEBUG) && pch_uart_is_debug(dev)) {
		struct resource *res = find_resource(dev, PCI_BASE_ADDRESS_0);
		/* Need to set the base and size for the resource allocator. */
		res->base = UART_DEBUG_BASE_ADDRESS;
		res->size = UART_DEBUG_BASE_SIZE;
		res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED |
			IORESOURCE_FIXED;
	}
}

static struct device_operations device_ops = {
	.read_resources		= &pch_uart_read_resources,
	.set_resources		= &pci_dev_set_resources,
	.enable_resources	= &pci_dev_enable_resources,
	.ops_pci		= &soc_pci_ops,
};

static const unsigned short pci_device_ids[] = {
	0x9d27, /* UART0 */
	0x9d28, /* UART1 */
	0x9d66, /* UART2 */
	0
};

static const struct pci_driver pch_uart __pci_driver = {
	.ops	 = &device_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
