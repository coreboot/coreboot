/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
 * Copyright (C) 2006,2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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
#include "mcp55.h"

static void ide_init(struct device *dev)
{
	struct southbridge_nvidia_mcp55_ide_config *conf =
	    (struct southbridge_nvidia_mcp55_ide_config *)dev->device_configuration;
	/* Enable ide devices so the linux ide driver will work */
	u32 dword;
	u16 word;
	u8 byte;

	word = pci_read_config16(dev, 0x50);
	/* Ensure prefetch is disabled */
	word &= ~((1 << 15) | (1 << 13));
	if (conf->ide1_enable) {
		word |= (1<<0);
		printk(BIOS_DEBUG, "Enable secondary ide interface\t");
	}
	if (conf->ide0_enable) {
		word |= (1<<1);
		printk(BIOS_DEBUG, "Enable primary ide interfac\n");
	}

	word |= (1<<12);
	word |= (1<<14);

	pci_write_config16(dev, 0x50, word);


	byte = 0x20 ; // Latency: 64-->32
	pci_write_config8(dev, PCI_LATENCY_TIMER, byte);

	dword = pci_read_config32(dev, 0xf8);
	dword |= 12;
	pci_write_config32(dev, 0xf8, dword);
#ifdef CONFIG_PCI_ROM_RUN
	pci_dev_init(dev);
#endif
}

struct device_operations mcp55_ide = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_NVIDIA,
			      .device = PCI_DEVICE_ID_NVIDIA_MCP55_IDE}}},
	.constructor		 = default_device_constructor,
	.phase3_scan		 = 0,
	.phase4_read_resources	 = pci_dev_read_resources,
	.phase4_set_resources	 = pci_dev_set_resources,
	.phase5_enable_resources = pci_dev_enable_resources,
	.phase6_init		 = ide_init,
	.ops_pci		 = &mcp55_pci_dev_ops_pci,
};

