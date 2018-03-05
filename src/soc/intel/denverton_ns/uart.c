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
#include <soc/uart.h>
#include <fsp/api.h>

static void dnv_ns_uart_read_resources(struct device *dev)
{
	/* read resources to be visible in the log*/
	pci_dev_read_resources(dev);
	if (!IS_ENABLED(CONFIG_LEGACY_UART_MODE))
		return;
	struct resource *res = find_resource(dev, PCI_BASE_ADDRESS_0);
	if (res == NULL)
		return;
	res->size = 0x8;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
	/* Do not configure membar */
	res = find_resource(dev, PCI_BASE_ADDRESS_1);
	if (res != NULL)
		res->flags = 0;
	compact_resources(dev);

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

static void hide_hsuarts(void)
{
	int i;
	printk(BIOS_DEBUG, "HIDING HSUARTs.\n");
	/* There is a hardware requirement to hide functions starting from the
	   last one. */
	for (i = DENVERTON_UARTS_TO_INI - 1; i >= 0; i--) {
		struct device *uart_dev;
		uart_dev = dev_find_slot(0, PCI_DEVFN(HSUART_DEV, i));
		if (uart_dev == NULL)
			continue;
		pci_or_config32(uart_dev, PCI_FUNC_RDCFG_HIDE, 1);
	}
}

/* Hide HSUART PCI device very last when FSP no longer needs it */
void platform_fsp_notify_status(enum fsp_notify_phase phase)
{
	if (phase != END_OF_FIRMWARE)
		return;
	if (IS_ENABLED(CONFIG_LEGACY_UART_MODE))
		hide_hsuarts();
}
