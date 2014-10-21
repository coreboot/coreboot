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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <delay.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include "hudson.h"


static void sata_init(struct device *dev)
{
#if IS_ENABLED(CONFIG_SOUTHBRIDGE_AMD_AGESA_YANGTZE)
	/**************************************
	 * Configure the SATA port multiplier *
	 **************************************/
	#define BYTE_TO_DWORD_OFFSET(x) (x/4)
	#define AHCI_BASE_ADDRESS_REG 0x24
	#define MISC_CONTROL_REG 0x40
	#define UNLOCK_BIT (1<<0)
	#define SATA_CAPABILITIES_REG 0xFC
	#define CFG_CAP_SPM (1<<12)

	volatile u32 *ahci_ptr =
		(u32*)(pci_read_config32(dev, AHCI_BASE_ADDRESS_REG) & 0xFFFFFF00);
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
	.scan_bus = 0,
	.ops_pci = &lops_pci,
};

static const struct pci_driver sata0_driver __pci_driver = {
	.ops = &sata_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_ATI_SB900_SATA,
};

static const struct pci_driver sata0_driver_ahci __pci_driver = {
	.ops = &sata_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_ATI_SB900_SATA_AHCI,
};
