/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Luis Correia <luis.f.correia@gmail.com>
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

/*
 *    IRQ          5530       USB     Network   Network   Network    free
 * controller  northbridge  device    device#0  device#1  device#2   slot
 *                          00.13.0   00.0a.00  00.0b.00  00.0c.00  00.0f.00
 * ------------------------------------------------------------------------
 *    14          INTA#      INTA#      n.c.      n.c.     n.c.      INTA#
 *    5           INTB#       n.c.      n.c.      n.c.    INTA#       n.c.
 *    10          INTC#       n.c.      n.c.     INTA#     n.c.       n.c.
 *    11          INTD#       n.c.     INTA#      n.c.     n.c.       n.c.
 */

/*
 * - the USB controller should be connected to IRQ14
 * - the network controller #0 should be connected to IRQ11
 * - the network controller #1 should be connected to IRQ10
 * - the network controller #2 should be connected to IRQ5
 * - the additional PCI slot must share the IRQ with the internal USB
 */

/* Bit  9 means IRQ 9 is available for this cs5530 INT input. */
#define IRQ_BITMAP_LINK0 0x0200
/* Bit  5 means IRQ 5 is available for this cs5530 INT input. */
#define IRQ_BITMAP_LINK1 0x0020
/* Bit 10 means IRQ10 is available for this cs5530 INT input. */
#define IRQ_BITMAP_LINK2 0x0400
/* Bit 11 means IRQ11 is available for this cs5530 INT input. */
#define IRQ_BITMAP_LINK3 0x0800

static const struct irq_routing_table intel_irq_routing_table = {
	.signature = PIRQ_SIGNATURE,	/* u32 signature */
	.version = PIRQ_VERSION,	/* u16 version   */
	.size = 32+16*CONFIG_IRQ_SLOT_COUNT,	/* There can be total 4 devices on the bus */
	.rtr_bus = 0x00,		/* Where the interrupt router lies (bus) */
	.rtr_devfn = (0x12<<3)|0x0,	/* Where the interrupt router lies (dev) */
	.exclusive_irqs = 0x4C20,	/* IRQs devoted exclusively to PCI usage */
	.rtr_vendor = 0x1078,		/* Vendor */
	.rtr_device = 0x0100,		/* Device */
	.miniport_data = 0,		/* Miniport data */
	.checksum = 0xBF+16,		/* TODO! calculate correct sum ! */
/*
 * Definition for "slot#0". There is no real slot,
 * the network device is soldered...
 */
	.slots = {
		[0] = {
			.bus = 0x00,
			.devfn = (0x0a<<3)|0x0,
			.irq = {
				[0] = {
					.link = 0x03,	/* INT C */
					.bitmap = IRQ_BITMAP_LINK2
				},
				[1] = {
					.link = 0x02,	/* INT B */
					.bitmap = IRQ_BITMAP_LINK1
				},
				[2] = {	/* = device INTA output */
					.link = 0x01,	/* INT A */
					.bitmap = IRQ_BITMAP_LINK0
				},
				[3] = {
					.link = 0x04,	/* = cs5530 INT D input */
					.bitmap = IRQ_BITMAP_LINK3
				}
			},
			.slot = 0x3,	/* soldered */
		},
/*
 * Definition for "slot#1". There is no real slot,
 * the network device is soldered...
 *
 * Configuration is ommited on purpose in the attempt of solving the
 * issue with IRQ panics (this is device is actually eth1).

		[1] = {
			.bus = 0x00,
			.devfn = (0x0b<<3)|0x0,
			.irq = {
				[0] = {
					.link = 0x04,
					.bitmap = IRQ_BITMAP_LINK3
				},
				[1] = {
					.link = 0x03,
					.bitmap = IRQ_BITMAP_LINK2
				},
				[2] = {
					.link = 0x02,
					.bitmap = IRQ_BITMAP_LINK1
				},
				[3] = {
					.link = 0x01,
					.bitmap = IRQ_BITMAP_LINK0
				}
			},
			.slot = 0x2,
		},
 */

/*
 * Definition for "slot#2". There is no real slot,
 * the network device is soldered...
 */
		[2] = {
			.bus = 0x00,
			.devfn = (0x0c<<3)|0x0,
			.irq = {
				[0] = {
					.link = 0x01,	/* INT A */
					.bitmap = IRQ_BITMAP_LINK0
				},
				[1] = {
					.link = 0x04,	/* INT D */
					.bitmap = IRQ_BITMAP_LINK3
				},
				[2] = {	/* = device INTA output */
					.link = 0x03,	/* INT C */
					.bitmap = IRQ_BITMAP_LINK2
				},
				[3] = {
					.link = 0x02,	/* = cs5530 INT B input */
					.bitmap = IRQ_BITMAP_LINK1
				}
			},
			.slot = 0x1,	/* soldered */
		},
/*
 * This is a free PCI slot.
 */
		[3] = {
			.bus = 0x00,
			.devfn = (0x0f<<3)|0x0,
			.irq = {
				[0] = {	/* = device INTA output */
					.link = 0x04,	/* INT D */
					.bitmap = IRQ_BITMAP_LINK3
				},
				[1] = {
					.link = 0x03,	/* = cs5530 INT C input */
					.bitmap = IRQ_BITMAP_LINK2
				},
				[2] = {
					.link = 0x02,	/* INT B */
					.bitmap = IRQ_BITMAP_LINK1
				},
				[3] = {
					.link = 0x01,	/* INT A */
					.bitmap = IRQ_BITMAP_LINK0
				}
			},
			.slot = 0x6,	/* FIXME: should be not 0, as it defines a real slot */
		},
/*
 * Definition for "slot#3". There is no real slot,
 * the USB device is embedded...
 */
		[4] = {
			.bus = 0x00,
			.devfn = (0x13<<3)|0x0,
			.irq = {
				[0] = {
					.link = 0x02,	/* INT B */
					.bitmap = IRQ_BITMAP_LINK1
				},
				[1] = {
					.link = 0x01,	/* INT A */
					.bitmap = IRQ_BITMAP_LINK0
				},
				[2] = {
					.link = 0x04,	/* INT D */
					.bitmap = IRQ_BITMAP_LINK3
				},
				[3] = {
					.link = 0x03,	/* INT C */
					.bitmap = IRQ_BITMAP_LINK2
				}
			},
			.slot = 0x5,	/* chip internal */
		}
	}
};

/**
 * Copy the IRQ routing table to memory.
 *
 * @param addr Destination address (between 0xF0000...0x100000).
 * @return The end address of the pirq routing table in memory.
 **/
unsigned long write_pirq_routing_table(unsigned long addr)
{
	return copy_pirq_routing_table(addr, &intel_irq_routing_table);
}
