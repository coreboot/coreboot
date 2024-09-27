/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <soc/sata.h>

static void sata_init(struct device *dev)
{
	uint32_t reg32, abar;

	printk(BIOS_DEBUG, "Set SATA controller in AHCI mode.\n");

	uint16_t reg16 = pci_read_config16(dev, SATA_MAP);
	reg16 &= ~(3 << 6);
	reg16 |= SATA_MAP_AHCI;
	pci_write_config16(dev, SATA_MAP, reg16);

	/* Initialize AHCI memory-mapped space */
	abar = pci_read_config32(dev, SATA_ABAR);

	/* Enable AHCI Mode */
	reg32 = read32p(abar + SATA_GHC);
	reg32 |= SATA_GHC_AE;
	write32p(abar + SATA_GHC, reg32);
}

static struct device_operations snr_sata_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = sata_init,
	.final = pci_dev_request_bus_master,
	.ops_pci = &pci_dev_ops_pci,
};

static const unsigned short snr_sata_ids[] = {
	PCI_DID_INTEL_SNR_SATA0,
	PCI_DID_INTEL_SNR_SATA2,
	0,
};

static const struct pci_driver snr_sata_driver __pci_driver = {
	.ops = &snr_sata_ops,
	.vendor = PCI_VID_INTEL,
	.devices = snr_sata_ids,
};
