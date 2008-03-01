/*
* This file is part of the coreboot project.
*
* Copyright (C) 2007 Advanced Micro Devices, Inc.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
*/

#include <types.h>
#include <lib.h>
#include <console.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <msr.h>
#include <io.h>
#include <pirq_routing.h>
#include <amd_geodelx.h>
#include <irq_tables.h>
#include "cs5536.h"

unsigned long write_pirq_routing_table(unsigned long addr)
{
	int i, j, k, num_entries;
	unsigned char pirq[4];
	u16 chipset_irq_map;
	u32 pciAddr, pirtable_end;
	struct irq_routing_table *pirq_tbl;

	pirtable_end = copy_pirq_routing_table(addr);

	/* Set up chipset IRQ steering. */
	pciAddr = 0x80000000 | (CHIPSET_DEV_NUM << 11) | 0x5C;
	chipset_irq_map = (PIRQD << 12 | PIRQC << 8 | PIRQB << 4 | PIRQA);
	printk(BIOS_DEBUG, "%s(%08X, %04X)\n", __FUNCTION__, pciAddr,
		     chipset_irq_map);
	outl(pciAddr & ~3, 0xCF8);
	outl(chipset_irq_map, 0xCFC);

	pirq_tbl = (struct irq_routing_table *) (addr);
	num_entries = (pirq_tbl->size - 32) / 16;

	/* Set PCI IRQs. */
	for (i = 0; i < num_entries; i++) {
		printk(BIOS_DEBUG, "PIR Entry %d Dev/Fn: %X Slot: %d\n", i,
			     pirq_tbl->slots[i].devfn, pirq_tbl->slots[i].slot);
		for (j = 0; j < 4; j++) {
			printk(BIOS_DEBUG, "INT: %c bitmap: %x ", 'A' + j,
				     pirq_tbl->slots[i].irq[j].bitmap);
			/* Finds lsb in bitmap to IRQ#. */
			for (k = 0; 
			     (!((pirq_tbl->slots[i].irq[j].bitmap >> k) & 1)) 
				     && (pirq_tbl->slots[i].irq[j].bitmap != 0);
			     k++);
			pirq[j] = k;
			printk(BIOS_DEBUG, "PIRQ: %d\n", k);
		}

		/* Bus, device, slots IRQs for {A,B,C,D}. */
		pci_assign_irqs(pirq_tbl->slots[i].bus,
				pirq_tbl->slots[i].devfn >> 3, pirq);
	}

	/* Put the PIR table in memory and checksum. */
	return pirtable_end;
}
