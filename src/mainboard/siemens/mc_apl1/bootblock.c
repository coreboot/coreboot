/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <bootblock_common.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <intelblocks/gpio.h>
#include <types.h>

static void pcie_rp_early_enable(void)
{
	const pci_devfn_t rp_dev = PCI_DEV(0, CONFIG_EARLY_PCI_BRIDGE_DEVICE,
		CONFIG_EARLY_PCI_BRIDGE_FUNCTION);

	if (pci_read_config16(rp_dev, PCI_VENDOR_ID) == 0xffff)
		return;

	/*
	 * Needs to be done "immediately after PERST# de-assertion"
	 * as per IAFW BIOS spec volume 2 (doc 559811)
	 */
	pci_and_config32(rp_dev, 0x338, ~(1 << 26));	/* BLKDQDA */
	pci_and_config32(rp_dev,  0xf4, ~(1 <<  2));	/* BLKPLLEN */
}

void bootblock_mainboard_early_init(void)
{
	const struct pad_config *pads;
	size_t num;

	pads = variant_early_gpio_table(&num);
	gpio_configure_pads(pads, num);

	/* Enable the PCIe root port when used before FSP-M MemoryInit() */
	if (CONFIG(EARLY_PCI_BRIDGE))
		pcie_rp_early_enable();
}
