/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <soc/sata.h>

#include "chip.h"

static void sata_init(struct device *dev)
{
	u32 reg32;
	u32 abar;

	printk(BIOS_DEBUG, "SATA: Initializing...\n");

	/* SATA configuration is handled by the FSP */

	/* Enable BARs */
	pci_write_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER |
						     PCI_COMMAND_MEMORY |
						     PCI_COMMAND_IO);

	printk(BIOS_DEBUG, "SATA: Controller in AHCI mode.\n");

	/* Set the controller mode */
	reg32 = pci_read_config32(dev, SATAGC);
	reg32 &= ~SATAGC_AHCI;
	pci_write_config32(dev, SATAGC, reg32);

	/* Initialize AHCI memory-mapped space */
	abar = pci_read_config32(dev, PCI_BASE_ADDRESS_5);
	printk(BIOS_DEBUG, "ABAR: %08X\n", abar);

	/* Enable AHCI Mode */
	reg32 = read32((void *)(abar + 0x04));
	reg32 |= (1 << 31);
	write32((void *)(abar + 0x04), reg32);
}

static void sata_enable(struct device *dev) { /* TODO */ }

static struct device_operations sata_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = sata_init,
	.enable = sata_enable,
	.ops_pci = &soc_pci_ops,
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_DNV_SATA_AHCI_1,
	PCI_DID_INTEL_DNV_SATA_AHCI_2,
	0
};

static const struct pci_driver soc_sata __pci_driver = {
	.ops = &sata_ops,
	.vendor = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
