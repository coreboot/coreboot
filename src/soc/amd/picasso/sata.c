/*
 * This file is part of the coreboot project.
 *
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
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <amdblocks/sata.h>
#include <soc/southbridge.h>

void soc_enable_sata_features(struct device *dev)
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
