/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

#include "hudson.h"

static void sata_init(struct device *dev)
{
#if CONFIG(SOUTHBRIDGE_AMD_AGESA_YANGTZE)
	/**************************************
	 * Configure the SATA port multiplier *
	 **************************************/
	#define BYTE_TO_DWORD_OFFSET(x) (x / 4)
	#define AHCI_BASE_ADDRESS_REG 0x24
	#define MISC_CONTROL_REG 0x40
	#define UNLOCK_BIT (1 << 0)
	#define SATA_CAPABILITIES_REG 0xFC
	#define CFG_CAP_SPM (1 << 12)

	volatile u32 *ahci_ptr =
		(u32*)(uintptr_t)(pci_read_config32(dev, AHCI_BASE_ADDRESS_REG) & 0xFFFFFF00);
	u32 temp;

	/* unlock the write-protect */
	temp = pci_read_config32(dev, MISC_CONTROL_REG);
	temp |= UNLOCK_BIT;
	pci_write_config32(dev, MISC_CONTROL_REG, temp);

	/* set the SATA AHCI mode to allow port expanders */
	*(ahci_ptr + BYTE_TO_DWORD_OFFSET(SATA_CAPABILITIES_REG)) |= CFG_CAP_SPM;

	/* lock the write-protect */
	temp = pci_read_config32(dev, MISC_CONTROL_REG);
	temp &= ~UNLOCK_BIT;
	pci_write_config32(dev, MISC_CONTROL_REG, temp);
#endif
};

static struct pci_operations lops_pci = {
	/* .set_subsystem = pci_dev_set_subsystem, */
};

static struct device_operations sata_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = sata_init,
	.ops_pci = &lops_pci,
};

static const struct pci_driver sata0_driver __pci_driver = {
	.ops = &sata_ops,
	.vendor = PCI_VID_AMD,
	.device = PCI_DID_AMD_SB900_SATA,
};

static const struct pci_driver sata0_driver_ahci __pci_driver = {
	.ops = &sata_ops,
	.vendor = PCI_VID_AMD,
	.device = PCI_DID_AMD_SB900_SATA_AHCI,
};
