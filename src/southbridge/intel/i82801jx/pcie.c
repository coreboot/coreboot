/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <device/pciexp.h>
#include <device/pci_ids.h>
#include <southbridge/intel/common/pciehp.h>
#include "chip.h"

static void pci_init(struct device *dev)
{
	struct southbridge_intel_i82801jx_config *config = dev->chip_info;

	printk(BIOS_DEBUG, "Initializing ICH10 PCIe root port.\n");

	/* Enable Bus Master */
	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);

	/* Set Cache Line Size to 0x10 */
	// This has no effect but the OS might expect it
	pci_write_config8(dev, 0x0c, 0x10);

	pci_and_config16(dev, PCI_BRIDGE_CONTROL, ~PCI_BRIDGE_CTL_PARITY);

	/* Enable IO xAPIC on this PCIe port */
	pci_or_config32(dev, 0xd8, 1 << 7);

	/* Enable Backbone Clock Gating */
	pci_or_config32(dev, 0xe1, (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0));

	/* Set VC0 transaction class */
	pci_update_config32(dev, 0x114, ~0x000000ff, 1);

	/* Mask completion timeouts */
	pci_or_config32(dev, 0x148, 1 << 14);

	/* Lock R/WO Correctable Error Mask. */
	pci_update_config32(dev, 0x154, ~0, 0);

	/* Clear errors in status registers */
	pci_update_config16(dev, 0x06, ~0, 0);
	pci_update_config16(dev, 0x1e, ~0, 0);

	/* Get configured ASPM state */
	const enum aspm_type apmc = pci_read_config32(dev, 0x50) & 3;

	/* If both L0s and L1 enabled then set root port 0xE8[1]=1 */
	if (apmc == PCIE_ASPM_BOTH)
		pci_or_config32(dev, 0xe8, 1 << 1);

	/* Enable expresscard hotplug events.  */
	if (config->pcie_hotplug_map[PCI_FUNC(dev->path.pci.devfn)]) {

		pci_or_config32(dev, 0xd8, 1 << 30);
		pci_write_config16(dev, 0x42, 0x142);
	}
}

static void pch_pciexp_scan_bridge(struct device *dev)
{
	struct southbridge_intel_i82801jx_config *config = dev->chip_info;

	if (CONFIG(PCIEXP_HOTPLUG) && config->pcie_hotplug_map[PCI_FUNC(dev->path.pci.devfn)]) {
		pciexp_hotplug_scan_bridge(dev);
	} else {
		/* Normal PCIe Scan */
		pciexp_scan_bridge(dev);
	}
}

static struct device_operations device_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.init			= pci_init,
	.scan_bus		= pch_pciexp_scan_bridge,
	.ops_pci		= &pci_dev_ops_pci,
};

/* 82801lJx, ICH10  */
static const unsigned short pci_device_ids[] = {
	0x3a40, /* Port 1 */
	0x3a42, /* Port 2 */
	0x3a44, /* Port 3 */
	0x3a46, /* Port 4 */
	0x3a48, /* Port 5 */
	0x3a4a, /* Port 6 */

	0x3a70, /* Port 1 */
	0x3a72, /* Port 2 */
	0x3a74, /* Port 3 */
	0x3a76, /* Port 4 */
	0x3a78, /* Port 5 */
	0x3a7a, /* Port 6 */
	0
};

static const struct pci_driver ich10_pcie __pci_driver = {
	.ops		= &device_ops,
	.vendor		= PCI_VID_INTEL,
	.devices	= pci_device_ids,
};
