/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Nikolay Petukhov <nikolay.petukhov@gmail.com>
 * Copyright (C) 2007 Kenji Noguchi <tokyo246@gmail.com>
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
#include <console/console.h>
#include <device/pci.h>

/* Platform IRQs */
#define PIRQA 11
#define PIRQB 10
#define PIRQC  9
#define PIRQD 12

/* Link */
#define LINK_PIRQA 1
#define LINK_PIRQB 2
#define LINK_PIRQC 3
#define LINK_PIRQD 4
#define LINK_NONE 0

/* Map */
#define IRQ_BITMAP_LINKA (1 << PIRQA)
#define IRQ_BITMAP_LINKB (1 << PIRQB)
#define IRQ_BITMAP_LINKC (1 << PIRQC)
#define IRQ_BITMAP_LINKD (1 << PIRQD)
#define IRQ_BITMAP_NOLINK 0x0

#define EXCLUSIVE_PCI_IRQS (IRQ_BITMAP_LINKA | IRQ_BITMAP_LINKB | IRQ_BITMAP_LINKC | IRQ_BITMAP_LINKD)

static const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE,	 /* u32 signature */
	PIRQ_VERSION,	 /* u16 version */
	32+16*CONFIG_IRQ_SLOT_COUNT,	 /* There can be a total of CONFIG_IRQ_SLOT_COUNT devices on the bus */
	0x00,		 /* Where the interrupt router lies (bus) */
	(0x12<<3)|0x0,	 /* Where the interrupt router lies (dev) */
	EXCLUSIVE_PCI_IRQS,		 /* IRQs devoted exclusively to PCI usage */
	0x1078,		 /* Vendor */
	0x1,		 /* Device */
	0,		 /* Miniport data */
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* u8 rfu[11] */
	0x60,		 /* u8 checksum. This has to be set to some
			    value that would give 0 after the sum of all
			    bytes for this structure (including checksum) */

	.slots = {
		[0] = {
			.slot = 0x0,	/* means also "on board" */
			.bus = 0x00,
			.devfn = (0x13<<3)|0x0,	/* 0x13 is USB OHCI */
			.irq = {
				[0] = {	/* <-- 0 means this is INTA# output from the device or slot */
					.link = LINK_PIRQA,
					.bitmap = IRQ_BITMAP_LINKA
				},
				[1] = {	/* <-- 1 means this is INTB# output from the device or slot */
					.link = LINK_NONE,
					.bitmap = IRQ_BITMAP_NOLINK
				},
				[2] = {	/* <-- 2 means this is INTC# output from the device or slot */
					.link = LINK_NONE,
					.bitmap = IRQ_BITMAP_NOLINK
				},
				[3] = {	/* <-- 3 means this is INTD# output from the device or slot */
					.link = LINK_NONE,
					.bitmap = IRQ_BITMAP_NOLINK
				}
			}
		},

		[1] = {
			.slot = 0x0,	/* means also "on board" */
			.bus = 0x00,
			.devfn = (0x15<<3)|0x0,	/* 0x15 is NSC Network device */
			.irq = {
				[0] = {	/* <-- 0 means this is INTA# output from the device or slot */
					.link = LINK_PIRQB,
					.bitmap = IRQ_BITMAP_LINKB
				},
				[1] = {	/* <-- 1 means this is INTB# output from the device or slot */
					.link = LINK_NONE,
					.bitmap = IRQ_BITMAP_NOLINK
				},
				[2] = {	/* <-- 2 means this is INTC# output from the device or slot */
					.link = LINK_NONE,
					.bitmap = IRQ_BITMAP_NOLINK
				},
				[3] = {	/* <-- 3 means this is INTD# output from the device or slot */
					.link = LINK_NONE,
					.bitmap = IRQ_BITMAP_NOLINK
				}
			}
		},

		[2] = {
			.slot = 0x1,	/* This is a Mini PCI slot */
			.bus = 0x00,
			.devfn = (0x14<<3)|0x0,
			.irq = {
				[0] = {	/* <-- 0 means this is INTA# output from the device or slot */
					.link = LINK_PIRQC,
					.bitmap = IRQ_BITMAP_LINKC
				},
				[1] = {	/* <-- 1 means this is INTB# output from the device or slot */
					/* NEEDSWORK: not confirmed. No device to test which uses both INTA and INTB  */
					.link = LINK_PIRQD,
					.bitmap = IRQ_BITMAP_LINKD
				},
				[2] = {	/* No INTC# for Mini PCI */
					.link = LINK_NONE,
					.bitmap = IRQ_BITMAP_NOLINK
				},
				[3] = {	/* No INTD# for Mini PCI */
					.link = LINK_NONE,
					.bitmap = IRQ_BITMAP_NOLINK
				}
			}
		},
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
