/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <types.h>
#include <lib.h>
#include <console.h>
#include <device/pci.h>
#include <msr.h>
#include <legacy.h>
#include <device/pci_ids.h>
#include <statictree.h>
#include <config.h>
#include "sb600.h"

static void ide_init(struct device *dev)
{
	struct southbridge_amd_sb600_ide_dts_config *conf;
	/* Enable ide devices so the linux ide driver will work */
	u32 dword;
	u8 byte;
	conf = dev->device_configuration;

	/* RPR10.1 disable MSI */
	dword = pci_read_config32(dev, 0x70);
	dword &= ~(1 << 16);
	pci_write_config32(dev, 0x70, dword);

	/* Ultra DMA mode */
	byte = pci_read_config8(dev, 0x54);
	byte |= 1 << 0;
	pci_write_config8(dev, 0x54, byte);
	byte = pci_read_config8(dev, 0x56);
	byte &= ~(7 << 0);
	byte |= 5 << 0;		/* mode 5 */
	pci_write_config8(dev, 0x56, byte);

	/* Enable I/O Access&& Bus Master */
	dword = pci_read_config16(dev, 0x4);
	dword |= 1 << 2;
	pci_write_config16(dev, 0x4, dword);

#if CONFIG_PCI_ROM_RUN == 1
	pci_dev_init(dev);
#endif

}

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

struct device_operations sb600_ide = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_ATI,
			      .device = PCI_DEVICE_ID_ATI_SB600_IDE}}},
	.constructor		 = default_device_constructor,
	.phase4_read_resources	 = pci_dev_read_resources,
	.phase4_set_resources	 = pci_dev_set_resources,
	.phase5_enable_resources = pci_dev_enable_resources,
	.phase6_init		 = ide_init,
	.ops_pci          = &lops_pci
};
