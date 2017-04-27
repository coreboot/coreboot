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

#include <device/pci.h>
#include <intelblocks/uart.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>

static int pch_uart_is_debug(struct device *dev)
{
	return dev->path.pci.devfn == PCH_DEVFN_UART2;
}

void pch_uart_read_resources(struct device *dev)
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
