/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <soc/southbridge.h>

static void soc_enable_sata_features(struct device *dev)
{
	u8 *ahci_ptr;
	u32 misc_ctl, cap_cfg;

	u32 temp;

	/* unlock the write-protect */
	misc_ctl = pci_read_config32(dev, SATA_MISC_CONTROL_REG);
	misc_ctl |= SATA_MISC_SUBCLASS_WREN;
	pci_write_config32(dev, SATA_MISC_CONTROL_REG, misc_ctl);

	/* set the SATA AHCI mode to allow port expanders */
	ahci_ptr = (u8 *)(uintptr_t)ALIGN_DOWN(
		pci_read_config32(dev, PCI_BASE_ADDRESS_5), 256);

	cap_cfg = read32(ahci_ptr + SATA_CAPABILITIES_REG);
	cap_cfg |= SATA_CAPABILITY_SPM;
	write32(ahci_ptr + SATA_CAPABILITIES_REG, cap_cfg);

	/* lock the write-protect */
	temp = pci_read_config32(dev, SATA_MISC_CONTROL_REG);
	temp &= ~SATA_MISC_SUBCLASS_WREN;
	pci_write_config32(dev, SATA_MISC_CONTROL_REG, temp);
};

static void sata_init(struct device *dev)
{
	soc_enable_sata_features(dev);
}

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

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_AMD_CZ_SATA,
	PCI_DEVICE_ID_AMD_CZ_SATA_AHCI,
	0
};

static const struct pci_driver sata0_driver __pci_driver = {
	.ops = &sata_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.devices = pci_device_ids,
};
