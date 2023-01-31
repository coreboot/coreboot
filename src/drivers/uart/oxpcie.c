/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <console/console.h>
#include <console/uart.h>
#include <device/mmio.h>
#include <device/pci_ops.h>

static void oxford_oxpcie_enable(struct device *dev)
{
	printk(BIOS_DEBUG, "Initializing Oxford OXPCIe952\n");

	struct resource *res = probe_resource(dev, PCI_BASE_ADDRESS_0);
	if (!res) {
		printk(BIOS_WARNING, "OXPCIe952: No UART resource found.\n");
		return;
	}
	void *bar0 = res2mmio(res, 0, 0);

	printk(BIOS_DEBUG, "OXPCIe952: Class=%x Revision ID=%x\n",
			(read32(bar0) >> 8), (read32(bar0) & 0xff));
	printk(BIOS_DEBUG, "OXPCIe952: %d UARTs detected.\n",
			(read32(bar0 + 4) & 3));
	printk(BIOS_DEBUG, "OXPCIe952: UART BAR: 0x%x\n", (u32)res->base);
}

static void oxford_oxpcie_set_resources(struct device *dev)
{
	pci_dev_set_resources(dev);

	/* Re-initialize OXPCIe base address after set_resources */
	u32 mmio_base = pci_read_config32(dev, PCI_BASE_ADDRESS_0);
	oxford_remap(mmio_base & ~0xf);
}

static struct device_operations oxford_oxpcie_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = oxford_oxpcie_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = oxford_oxpcie_enable,
};

static const struct pci_driver oxford_oxpcie_driver __pci_driver = {
	.ops    = &oxford_oxpcie_ops,
	.vendor = 0x1415,
	.device = 0xc158,
};

static const struct pci_driver oxford_oxpcie_driver_2 __pci_driver = {
	.ops    = &oxford_oxpcie_ops,
	.vendor = 0x1415,
	.device = 0xc11b,
};
