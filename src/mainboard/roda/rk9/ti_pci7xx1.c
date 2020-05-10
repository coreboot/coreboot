/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/cardbus.h>

static void pci7xx1_enable(struct device *const dev)
{
	printk(BIOS_DEBUG, "%s: TI PCI7xx1 media controller\n", __func__);
	if (PCI_FUNC(dev->path.pci.devfn) == 0) {
		const unsigned int slot = PCI_SLOT(dev->path.pci.devfn);

		int fn;

		/* Hide functions based on devicetree info. */
		u16 gcr = pci_read_config16(dev, 0x86);
		for (fn = 5; fn > 0; --fn) {
			const struct device *const d =
				pcidev_path_behind(dev->bus, PCI_DEVFN(slot, fn));
			if (!d || d->enabled) continue;
			printk(BIOS_DEBUG,
				"%s: Hiding function #%d.\n", __func__, fn);
			switch (fn) {
				case 1: gcr |= 1 << 4; break; /* CardBus B */
				case 2: gcr |= 1 << 3; break; /* OHCI 1394 */
				case 3: gcr |= 1 << 5; break; /* Flash media */
				case 4: gcr |= 1 << 6; break; /* SD card */
				case 5: gcr |= 1 << 7; break; /* Smart Card */
			}
		}
		pci_write_config16(dev, 0x86, gcr);
	}
}

static struct device_operations device_ops = {
	.read_resources		= cardbus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= cardbus_enable_resources,
	.init			= 0,
	.scan_bus		= pci_scan_bridge,
	.enable			= pci7xx1_enable,
	.reset_bus		= pci_bus_reset,
};

static const struct pci_driver ti_pci7xx1 __pci_driver = {
	.ops	= &device_ops,
	.vendor	= 0x104c,
	.device	= 0x8031,
};
