/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <option.h>
#include "sb700.h"

static void ide_init(struct device *dev)
{
	struct southbridge_amd_sb700_config *conf;
	/* Enable ide devices so the linux ide driver will work */
	u32 dword;
	u8 byte;
	uint8_t nvram;
	uint8_t sata_ahci_mode;

	sata_ahci_mode = 0;
	if (get_option(&nvram, "sata_ahci_mode") == CB_SUCCESS)
		sata_ahci_mode = !!nvram;

	conf = dev->chip_info;

	/* RPR9.1 disable MSI */
	/* TODO: For A14, it should set as 1. I doubt it. */
	dword = pci_read_config32(dev, 0x70);
	dword &= ~(1 << 16);
	pci_write_config32(dev, 0x70, dword);

	if (!sata_ahci_mode) {
		/* Enable UDMA on all devices, it will become UDMA0 (default PIO is PIO0) */
		byte = pci_read_config8(dev, 0x54);
		byte |= 0xf;
		pci_write_config8(dev, 0x54, byte);

		/* Enable I/O Access&& Bus Master */
		dword = pci_read_config16(dev, 0x4);
		dword |= 1 << 2;
		pci_write_config16(dev, 0x4, dword);

		/* set ide as primary, if you want to boot from IDE, you'd better set it
		 * in $vendor/$mainboard/devicetree.cb */
		if (conf->boot_switch_sata_ide == 1) {
			struct device *sm_dev = dev_find_slot(0, PCI_DEVFN(0x14, 0));
			byte = pci_read_config8(sm_dev, 0xad);
			byte |= 1 << 4;
			pci_write_config8(sm_dev, 0xad, byte);
		}
	}
}

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

static struct device_operations ide_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = ide_init,
	.scan_bus = 0,
	.ops_pci = &lops_pci,
};

static const struct pci_driver ide_driver __pci_driver = {
	.ops = &ide_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB700_IDE,
};
