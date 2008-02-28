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
#include "../../../southbridge/amd/cs5536/cs5536.h"



/* Platform IRQs */
#define PIRQA 10
#define PIRQB 11
#define PIRQC 10
#define PIRQD 11

/* Map */
#define M_PIRQA (1 << PIRQA)	/* Bitmap of supported IRQs */
#define M_PIRQB (1 << PIRQB)	/* Bitmap of supported IRQs */
#define M_PIRQC (1 << PIRQC)	/* Bitmap of supported IRQs */
#define M_PIRQD (1 << PIRQD)	/* Bitmap of supported IRQs */

/* Link */
#define L_PIRQA 1		/* Means Slot INTx# Connects To Chipset INTA# */
#define L_PIRQB 2		/* Means Slot INTx# Connects To Chipset INTB# */
#define L_PIRQC 3		/* Means Slot INTx# Connects To Chipset INTC# */
#define L_PIRQD 4		/* Means Slot INTx# Connects To Chipset INTD# */

const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE,
	PIRQ_VERSION,
	32 + 16 * 5,		/* Max. number of devices on the bus */
	0x00,			/* Where the interrupt router lies (bus) */
	(0x0F << 3) | 0x0,      /* Where the interrupt router lies (dev) */
	0x00,			/* IRQs devoted exclusively to PCI usage */
	0x100B,			/* Vendor */
	0x002B,			/* Device */
	0,			/* Crap (miniport) */
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/* u8 rfu[11] */
	0x00,			/* Checksum */
	{
	 /* bus, dev|fn,           {link, bitmap},      {link, bitmap},     {link, bitmap},     {link, bitmap},     slot, rfu */
	 {0x00, (0x01 << 3) | 0x0, {{L_PIRQA, M_PIRQA}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}}, 0x0, 0x0},	/* cpu */
	 {0x00, (0x0F << 3) | 0x0, {{L_PIRQA, M_PIRQA}, {L_PIRQB, M_PIRQB}, {L_PIRQC, M_PIRQC}, {L_PIRQD, M_PIRQD}}, 0x0, 0x0},	/* chipset */
	 {0x00, (0x0D << 3) | 0x0, {{L_PIRQC, M_PIRQC}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}}, 0x0, 0x0},	/* ethernet */
	}
};

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
