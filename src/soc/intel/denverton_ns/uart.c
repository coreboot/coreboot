/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 - 2017 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * The sole purpose of this driver is to avoid BAR to be changed during
 * resource allocation. Since configuration space is just 32 bytes it
 * shouldn't cause any fragmentation.
 */

#include <console/uart.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/pci_devs.h>
#include <console/console.h>

static void dnv_ns_uart_read_resources(struct device *dev)
{
	/* read resources to be visible in the log*/
	pci_dev_read_resources(dev);
}

static struct device_operations uart_ops = {
	.read_resources = dnv_ns_uart_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = pci_dev_init,
	.enable = DEVICE_NOOP
};

static const unsigned short uart_ids[] = {
	HSUART_DEVID, /* HSUART 0/1/2 */
	0
};

static const struct pci_driver uart_driver __pci_driver = {
	.ops = &uart_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.devices = uart_ids
};
