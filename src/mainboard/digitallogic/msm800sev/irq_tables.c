/*
* This file is part of the LinuxBIOS project.
*
* Copyright (C) 2007 Advanced Micro Devices
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA      02110-1301 USA
*/

#include <arch/pirq_routing.h>
#include <console/console.h>
#include <arch/io.h>
#include <arch/pirq_routing.h>
#include "../../../southbridge/amd/cs5536/cs5536.h"

/* Platform IRQs */
#define PIRQA 11
#define PIRQB 5
#define PIRQC 10
#define PIRQD 10

/* Map */
#define M_PIRQA (1 << PIRQA)  /* Bitmap of supported IRQs */
#define M_PIRQB (1 << PIRQB)  /* Bitmap of supported IRQs */
#define M_PIRQC (1 << PIRQC)  /* Bitmap of supported IRQs */
#define M_PIRQD (1 << PIRQD)  /* Bitmap of supported IRQs */

/* Link */
#define L_PIRQA  1 /* Means Slot INTx# Connects To Chipset INTA# */
#define L_PIRQB  2 /* Means Slot INTx# Connects To Chipset INTB# */
#define L_PIRQC  3 /* Means Slot INTx# Connects To Chipset INTC# */
#define L_PIRQD  4 /* Means Slot INTx# Connects To Chipset INTD# */

const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE,  /* u32 signature */
	PIRQ_VERSION,    /* u16 version   */
	32+16*9,	 /* There can be total 9 devices on the bus */
	0x00,		 /* Where the interrupt router lies (bus) */
	(0x0f<<3)|0x0,   /* Where the interrupt router lies (dev) */
	0,		 /* IRQs devoted exclusively to PCI usage */
	0x100b,		 /* Vendor */
	0x2b,		 /* Device */
	0,		 /* Crap (miniport) */
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* u8 rfu[11] */
	0xe,		 /* u8 checksum. This has to be set to some
			    value that would give 0 after the sum of all
			    bytes for this structure (including checksum) */
	{
		/* bus,     dev|fn,   {link, bitmap}, {link, bitmap}, {link, bitmap}, {link, bitmap},  slot, rfu */
		{0x00,(0x01<<3)|0x0, {{0x01, 0x0400}, {0x00, 0x0000}, {0x00, 0x0000}, {0x00, 0x00000}}, 0x0, 0x0},
		{0x00,(0x0f<<3)|0x0, {{0x01, 0x0400}, {0x02, 0x0800}, {0x03, 0x0400}, {0x04, 0x00800}}, 0x0, 0x0},
		{0x00,(0x13<<3)|0x0, {{0x01, 0x0400}, {0x00, 0x0000}, {0x00, 0x0000}, {0x00, 0x00000}}, 0x0, 0x0},
		{0x00,(0x12<<3)|0x0, {{0x03, 0x0400}, {0x00, 0x0000}, {0x00, 0x0000}, {0x00, 0x00000}}, 0x0, 0x0},
		{0x00,(0x11<<3)|0x0, {{0x01, 0x0400}, {0x02, 0x0800}, {0x00, 0x0000}, {0x00, 0x00000}}, 0x0, 0x0},
		{0x00,(0x0a<<3)|0x0, {{0x01, 0x0400}, {0x02, 0x0800}, {0x03, 0x0400}, {0x04, 0x00800}}, 0x1, 0x0},
		{0x00,(0x0b<<3)|0x0, {{0x02, 0x0800}, {0x03, 0x0400}, {0x04, 0x0800}, {0x01, 0x00400}}, 0x2, 0x0},
		{0x00,(0x0c<<3)|0x0, {{0x03, 0x0400}, {0x04, 0x0800}, {0x01, 0x0400}, {0x02, 0x00800}}, 0x3, 0x0},
		{0x00,(0x0d<<3)|0x0, {{0x04, 0x0800}, {0x01, 0x0400}, {0x02, 0x0800}, {0x03, 0x00400}}, 0x4, 0x0},
	}
};


unsigned long write_pirq_routing_table(unsigned long addr){
       int i, j, k, num_entries;
       unsigned int pirq[4];
       uint16_t chipset_irq_map;
       uint32_t pciAddr, pirtable_end;
       struct irq_routing_table *pirq_tbl;

       pirtable_end = copy_pirq_routing_table(addr);

       /* Set up chipset IRQ steering */
       pciAddr = 0x80000000 | (CHIPSET_DEV_NUM << 11) | 0x5C;
       chipset_irq_map = (11 << 12 | 10 << 8 | 11 << 4 | 10);
       printk_debug("%s(%08X, %04X)\n", __FUNCTION__, pciAddr, chipset_irq_map);
       outl(pciAddr & ~3, 0xCF8);
       outl(chipset_irq_map, 0xCFC);

       pirq_tbl = (struct irq_routing_table *)(addr);
       num_entries = (pirq_tbl->size - 32)/16;

       /* Set PCI IRQs */
       for (i=0; i < num_entries; i++){
               printk_debug("PIR Entry %d Dev/Fn: %X Slot: %d\n", i, pirq_tbl->slots[i].devfn, pirq_tbl->slots[i].slot);
               for (j = 0; j < 4; j++){
               printk_debug("INT: %c bitmap: %x ", 'A'+j, pirq_tbl->slots[i].irq[j].bitmap);
                       for (k = 0; (!((pirq_tbl->slots[i].irq[j].bitmap >> k) & 1)) && (pirq_tbl->slots[i].irq[j].bitmap != 0); k++); /* finds lsb in bitmap to IRQ# */
                       pirq[j] = k;
                       printk_debug("PIRQ: %d\n", k);
               }
               pci_assign_irqs(pirq_tbl->slots[i].bus, pirq_tbl->slots[i].devfn, pirq);        /* bus, device, slots IRQs for {A,B,C,D} */
       }

       /* put the PIR table in memory and checksum */
       return pirtable_end;
}

