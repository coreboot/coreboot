/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <device/pci_ops.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

#include "i82801ix.h"

static void thermal_init(struct device *dev)
{
	if (LPC_IS_MOBILE(pcidev_on_root(0x1f, 0)))
		return;

	u8 reg8;

	pci_write_config32(dev, 0x10, (uintptr_t)DEFAULT_TBAR);
	pci_or_config32(dev, 0x04, 1 << 1);

	write32(DEFAULT_TBAR + 0x04, 0); /* Clear thermal trip points. */
	write32(DEFAULT_TBAR + 0x44, 0);

	write8(DEFAULT_TBAR + 0x01, 0xba); /* Enable sensor 0 + 1. */
	write8(DEFAULT_TBAR + 0x41, 0xba);

	reg8 = read8(DEFAULT_TBAR + 0x08); /* Lock thermal registers. */
	write8(DEFAULT_TBAR + 0x08, reg8 | (1 << 7));
	reg8 = read8(DEFAULT_TBAR + 0x48);
	write8(DEFAULT_TBAR + 0x48, reg8 | (1 << 7));

	pci_and_config32(dev, 0x04, ~(1 << 1));
	pci_write_config32(dev, 0x10, 0);
}

static struct device_operations device_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= thermal_init,
	.ops_pci		= &pci_dev_ops_pci,
};

static const struct pci_driver ich9_thermal __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VID_INTEL,
	.device	= PCI_DID_INTEL_82801IB_THERMAL,
};
