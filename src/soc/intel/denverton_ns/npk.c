/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

#include <soc/pci_devs.h>
#include <soc/ramstage.h>

static void npk_init(struct device *dev)
{
	printk(BIOS_DEBUG, "pch: %s\n", __func__);

	/* TODO */
}

static void pci_npk_read_resources(struct device *dev)
{
	/* Skip NorthPeak enumeration. */
}

static struct device_operations pmc_ops = {
	.read_resources = pci_npk_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = npk_init,
	.ops_pci = &soc_pci_ops,
};

static const struct pci_driver pch_pmc __pci_driver = {
	.ops = &pmc_ops,
	.vendor = PCI_VID_INTEL,
	.device = PCI_DID_INTEL_DNV_TRACEHUB,
};
