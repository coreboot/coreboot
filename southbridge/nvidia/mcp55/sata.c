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


static void sata_init(struct device *dev)
{
	u32 dword;

	struct southbridge_nvidia_mcp55_sata_config *conf =
	    (struct southbridge_nvidia_mcp55_sata_config *)dev->device_configuration;

	dword = pci_read_config32(dev, 0x50);
	/* Ensure prefetch is disabled */
	dword &= ~((1 << 15) | (1 << 13));
	if(conf) {
		if (conf->sata1_enable) {
			dword |= (1<<0);
			printk(BIOS_DEBUG, "Enable secondary SATA interface\t");
		}
		if (conf->sata0_enable) {
			dword |= (1<<1);
			printk(BIOS_DEBUG, "Enable primary SATA interface\n");
		}
	} else {
		dword |= (1<<1) | (1<<0);
		printk(BIOS_DEBUG, "Enable primary and secondary SATA interfaces\n");
	}


#if 1
	dword &= ~(0x1f<<24);
	dword |= (0x15<<24);
#endif
	pci_write_config32(dev, 0x50, dword);

	dword = pci_read_config32(dev, 0xf8);
	dword |= 2;
	pci_write_config32(dev, 0xf8, dword);


#warning finish set subsystem in mcp55 sata
#if 0
	pci_write_config32(dev, 0x40,
		((device & 0xffff) << 16) | (vendor & 0xffff));
#endif
}

struct device_operations mcp55_sata = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_NVIDIA,
			      .device = PCI_DEVICE_ID_NVIDIA_MCP55_SATA1}}},
	.constructor		 = default_device_constructor,
	.phase3_scan		 = 0,
	.phase4_read_resources	 = pci_dev_read_resources,
	.phase4_set_resources	 = pci_dev_set_resources,
	.phase5_enable_resources = pci_dev_enable_resources,
	.phase6_init		 = sata_init,
	.ops_pci		 = &pci_dev_ops_pci,
};
