/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "pch.h"
#include <device/mmio.h>

static void thermal_init(struct device *dev)
{
	struct resource *res;
	u8 *base;
	printk(BIOS_DEBUG, "Thermal init start.\n");

	res = probe_resource(dev, PCI_BASE_ADDRESS_0);
	if (!res)
		return;

	base = res2mmio(res, 0, 0);
	write32(base + 4, 0x3a2b);
	write8(base + 0xe, 0x40);
	write16(base + 0x56, 0xffff);
	write16(base + 0x64, 0xffff);
	write16(base + 0x66, 0xffff);
	write16(base + 0x68, 0xfa);

	write8(base + 1, 0xb8);

	printk(BIOS_DEBUG, "Thermal init done.\n");
}

static struct device_operations thermal_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = thermal_init,
	.ops_pci = &pci_dev_ops_pci,
};

static const struct pci_driver pch_thermal __pci_driver = {
	.ops = &thermal_ops,
	.vendor = PCI_VID_INTEL,
	.device = PCI_DID_INTEL_IBEXPEAK_THERMAL,
};
