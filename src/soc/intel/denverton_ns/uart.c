/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * The sole purpose of this driver is to avoid BAR to be changed during
 * resource allocation. Since configuration space is just 32 bytes it
 * shouldn't cause any fragmentation.
 */

#include <console/uart.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <soc/pci_devs.h>
#include <console/console.h>
#include <soc/uart.h>
#include <fsp/api.h>

static void dnv_ns_uart_read_resources(struct device *dev)
{
	/* read resources to be visible in the log*/
	pci_dev_read_resources(dev);
	if (!CONFIG(LEGACY_UART_MODE))
		return;
	struct resource *res = probe_resource(dev, PCI_BASE_ADDRESS_0);
	if (res == NULL)
		return;
	res->size = 0x8;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
	/* Do not configure membar */
	res = probe_resource(dev, PCI_BASE_ADDRESS_1);
	if (res != NULL)
		res->flags = 0;
	compact_resources(dev);

}

static struct device_operations uart_ops = {
	.read_resources = dnv_ns_uart_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = pci_dev_init,
};

static const struct pci_driver uart_driver __pci_driver = {
	.ops = &uart_ops,
	.vendor = PCI_VID_INTEL,
	.device = PCI_DID_INTEL_DNV_HSUART
};

static void hide_hsuarts(void)
{
	int i;
	printk(BIOS_DEBUG, "HIDING HSUARTs.\n");
	/* There is a hardware requirement to hide functions starting from the
	   last one. */
	for (i = DENVERTON_UARTS_TO_INI - 1; i >= 0; i--) {
		struct device *uart_dev;
		uart_dev = pcidev_on_root(HSUART_DEV, i);
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
	if (CONFIG(LEGACY_UART_MODE))
		hide_hsuarts();
}
