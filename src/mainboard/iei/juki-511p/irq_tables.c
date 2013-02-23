/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Nikolay Petukhov <nikolay.petukhov@gmail.com>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/pirq_routing.h>

#define IRQ_BITMAP_LINK0 0x0800		/* chipset's INTA# input should be routed to IRQ11 */
#define IRQ_BITMAP_LINK1 0x0400		/* chipset's INTB# input should be routed to IRQ10 */
#define IRQ_BITMAP_LINK2 0x0000		/* chipset's INTC# input should be routed to nothing (disabled) */
#define IRQ_BITMAP_LINK3 0x0000		/* chipset's INTD# input should be routed to nothing (disabled) */

static const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE,	 /* u32 signature */
	PIRQ_VERSION,	 /* u16 version */
	32+16*CONFIG_IRQ_SLOT_COUNT,	 /* There can be a total of CONFIG_IRQ_SLOT_COUNT devices on the bus */
	0x00,		 /* Where the interrupt router lies (bus) */
	(0x12<<3)|0x0,	 /* Where the interrupt router lies (dev) */
	0xc00,		 /* IRQs devoted exclusively to PCI usage */
	0x1078,		 /* Vendor */
	0x2,		 /* Device */
	0,		 /* Miniport data */
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* u8 rfu[11] */
	0x57,		 /* u8 checksum. This has to be set to some
			    value that would give 0 after the sum of all
			    bytes for this structure (including checksum) */

	.slots = {
		[0] = {
			.slot = 0x0,	/* should be 0 when it is no real slot. My device is soldered */
			.bus = 0x00,
			.devfn = (0x13<<3)|0x0,	/* 0x13 is my USB OHCI */
			.irq = {
				[0] = {	/* <-- 0 means this is INTA# output from the device or slot */
					.link = 0x01,			/* 0x01 means its connected to INTA# input at chipset */
					.bitmap = IRQ_BITMAP_LINK0
				},
				[1] = {	/* <-- 1 means this is INTB# output from the device or slot */
					.link = 0x02,			/* 0x02 means its connected to INTB# input at chipset */
					.bitmap = IRQ_BITMAP_LINK1
				},
				[2] = {	/* <-- 2 means this is INTC# output from the device or slot */
					.link = 0x03,			/* 0x03 means its connected to INTC# input at chipset */
					.bitmap = IRQ_BITMAP_LINK2
				},
				[3] = {	/* <-- 3 means this is INTD# output from the device or slot */
					.link = 0x04,			/* 0x04 means its connected to INTD# input at chipset */
					.bitmap = IRQ_BITMAP_LINK3
				}
			}
		},

		[1] = {
			.slot = 0x0,	/* means also "on board" */
			.bus = 0x00,
			.devfn = (0x0e<<3)|0x0,	/* 0x0e is my Realtek Network device */
			.irq = {
				[0] = {	/* <-- 0 means this is INTA# output from the device or slot */
					.link = 0x02,			/* 0x02 means its connected to INTB# input at chipset */
					.bitmap = IRQ_BITMAP_LINK1
				},
				[1] = {	/* <-- 1 means this is INTB# output from the device or slot */
					.link = 0x03,			/* 0x03 means its connected to INTC# input at chipset */
					.bitmap = IRQ_BITMAP_LINK2
				},
				[2] = {	/* <-- 2 means this is INTC# output from the device or slot */
					.link = 0x04,			/* 0x04 means its connected to INTD# input at chipset */
					.bitmap = IRQ_BITMAP_LINK3
				},
				[3] = {	/* <-- 3 means this is INTD# output from the device or slot */
					.link = 0x01,			/* 0x01 means its connected to INTA# input at chipset */
					.bitmap = IRQ_BITMAP_LINK0
				}
			}
		}
	}
};

/**
 * Copy the IRQ routing table to memory.
 *
 * @param addr Destination address (between 0xF0000...0x100000).
 * @return The end address of the pirq routing table in memory.
 */
unsigned long write_pirq_routing_table(unsigned long addr)
{
	return copy_pirq_routing_table(addr, &intel_irq_routing_table);
}
