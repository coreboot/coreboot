/*
 * This file is part of the coreboot project.
 *
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

#include <device/device.h>
#include <soc/uart.h>
#include <device/pci.h>
#include <device/pci_ids.h>

static void cavium_uart_init(struct device *dev)
{
	const u8 fn = PCI_FUNC(dev->path.pci.devfn);

	/* Calling uart_setup with no baudrate will do minimal HW init
	 * enough for the kernel to not panic */
	if (!uart_is_enabled(fn))
		uart_setup(fn, 0);
}

static struct device_operations device_ops = {
	.init = cavium_uart_init,
};

static const struct pci_driver soc_cavium_uart __pci_driver = {
	.ops    = &device_ops,
	.vendor = PCI_VENDOR_CAVIUM,
	.device = PCI_DEVICE_ID_CAVIUM_THUNDERX_UART,
};

struct chip_operations soc_cavium_common_pci_ops = {
	CHIP_NAME("Cavium ThunderX UART")
};
