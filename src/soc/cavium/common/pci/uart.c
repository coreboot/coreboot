/* SPDX-License-Identifier: GPL-2.0-only */

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
	.device = PCI_DID_CAVIUM_THUNDERX_UART,
};

struct chip_operations soc_cavium_common_pci_ops = {
	CHIP_NAME("Cavium ThunderX UART")
};
