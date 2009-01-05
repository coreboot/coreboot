/*
 * This file is part of the coreboot project.
 *
 * Copyright 2004 Tyan Computer
 *  by yhlu@tyan.com
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
#include <console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "ck804.h"

static void pci_init(struct device *dev)
{
	u32 dword;

	dword = pci_read_config32(dev, 0x04);
	dword |= (1 << 8);	/* System error enable */
	dword |= (1 << 30);	/* Clear possible errors */
	pci_write_config32(dev, 0x04, dword);

#if 0
	word = pci_read_config16(dev, 0x48);
	word |= (1 << 0);	/* MRL2MRM */
	word |= (1 << 2);	/* MR2MRM */
	pci_write_config16(dev, 0x48, word);
#endif

#if 1
	dword = pci_read_config32(dev, 0x4c);
	dword |= 0x00440000;	/* TABORT_SER_ENABLE Park Last Enable. */
	pci_write_config32(dev, 0x4c, dword);
#endif

#warning 64-bit disabled here
#if 0
	struct device * pci_domain_dev;
	struct resource *mem1, *mem2;
	pci_domain_dev = dev->bus->dev;

	if (!pci_domain_dev)
		return;		/* Impossible */

	mem1 = find_resource(pci_domain_dev, 1);	// prefmem, it could be 64bit
	mem2 = find_resource(pci_domain_dev, 2);	// mem
	if (mem1->base > mem2->base) {
		dword = mem2->base & (0xffff0000UL);
		printk(BIOS_DEBUG, "PCI DOMAIN mem2 base = 0x%010Lx\n", mem2->base);
	} else {
		dword = mem1->base & (0xffff0000UL);
		printk(BIOS_DEBUG, "PCI DOMAIN mem1 (prefmem) base = 0x%010Lx\n",
			     mem1->base);
	}
#else
	dword = dev_root.resource[1].base & (0xffff0000UL);
	printk(BIOS_DEBUG, "dev_root mem base = 0x%010Lx\n",
		     dev_root.resource[1].base);
#endif

	printk(BIOS_DEBUG, "[0x50] <-- 0x%08x\n", dword);
	pci_write_config32(dev, 0x50, dword);	/* TOM */
}

struct device_operations ck804_pci_ops = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_NVIDIA,
			 .device = PCI_DEVICE_ID_NVIDIA_CK804_PCI}}},
	.phase3_chip_setup_dev	 = ck804_enable,
	.phase3_scan		 = pci_scan_bridge,
	.phase4_read_resources	 = pci_dev_read_resources,
	.phase4_set_resources	 = pci_set_resources,
	.phase5_enable_resources = pci_dev_enable_resources,
	.phase6_init		 = pci_init,
	.ops_pci		 = NULL,
};

