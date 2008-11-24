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

static void pci_init(struct device *dev)
{

	u32 dword;
	u16 word;
#ifdef CONFIG_PCI_64BIT_PREF_MEM
	struct device pci_domain_dev;
	struct resource *mem1, *mem2;
#endif

	/* System error enable */
	dword = pci_read_config32(dev, PCI_COMMAND);
	dword |= (1<<8); /* System error enable */
	dword |= (1<<30); /* Clear possible errors */
	pci_write_config32(dev, PCI_COMMAND, dword);

#if 1
	//only need (a01,xx]
	word = pci_read_config16(dev, 0x48);
	word |= (1<<0); /* MRL2MRM */
	word |= (1<<2); /* MR2MRM */
	pci_write_config16(dev, 0x48, word);
#endif

#if 1
	dword = pci_read_config32(dev, 0x4c);
	dword |= 0x00440000; /*TABORT_SER_ENABLE Park Last Enable.*/
	pci_write_config32(dev, 0x4c, dword);
#endif

#ifdef CONFIG_PCI_64BIT_PREF_MEM
	pci_domain_dev = dev->bus->dev;
	while(pci_domain_dev) {
		if(pci_domain_dev->path.type == DEVICE_PATH_PCI_DOMAIN) break;
		pci_domain_dev = pci_domain_dev->bus->dev;
	}

	if(!pci_domain_dev) return; // impossiable
	mem1 = find_resource(pci_domain_dev, 1); // prefmem, it could be 64bit
	mem2 = find_resource(pci_domain_dev, 2); // mem
	if(mem1->base > mem2->base) {
		dword = mem2->base  & (0xffff0000UL);
		printk(BIOS_DEBUG, "PCI DOMAIN mem2 base = 0x%010Lx\n", mem2->base);
	} else {
		dword = mem1->base  & (0xffff0000UL);
		printk(BIOS_DEBUG, "PCI DOMAIN mem1 (prefmem) base = 0x%010Lx\n", mem1->base);
	}
#else
	dword = dev_root.resource[1].base & (0xffff0000UL);
	printk(BIOS_DEBUG, "dev_root mem base = 0x%010Lx\n", dev_root.resource[1].base);
#endif
	printk(BIOS_DEBUG, "[0x50] <-- 0x%08x\n", dword);
	pci_write_config32(dev, 0x50, dword); //TOM

}

struct device_operations mcp55_pci = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_NVIDIA,
			      .device = PCI_DEVICE_ID_NVIDIA_MCP55_PCI}}},
	.constructor		 = default_device_constructor,
	.reset_bus		= pci_bus_reset,
	.phase3_scan		 = pci_scan_bridge,
	.phase4_read_resources	 = pci_dev_read_resources,
	.phase4_set_resources	 = pci_set_resources,
	.phase5_enable_resources = pci_bus_enable_resources,
	.phase6_init		 = pci_init,
	.ops_pci		 = &mcp55_pci_dev_ops_pci,
};
