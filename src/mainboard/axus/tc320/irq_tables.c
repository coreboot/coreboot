/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Juergen Beisert <juergen@kreuzholzen.de>
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

/**
 * @file
 * Interrupt routing description for the AXUS TC320 board.
 * It was not possible to read back the PIRQ table. There was no BIOS to ask
 * for it, only a bootloader for an embedded OS.
 * But with the method described here:
 *    http://coreboot.org/Creating_Valid_IRQ_Tables
 * it was possible to detect the physical IRQ routing on this board.
 *
 * This is the physical routing on this board:
 *
 *    IRQ          5530       USB     Network
 * controller  northbridge  device     device
 *                          00.13.0   00.0e.00
 * --------------------------------------------
 *    11          INTA#      INTA#      n.c.
 *    15          INTB#       n.c.     INTA#
 *                INTC#       n.c.      n.c.
 *                INTD#       n.c.      n.c.
 */

#include <arch/pirq_routing.h>

#define INT_A 0x01
#define INT_B 0x02
#define INT_C 0x03
#define INT_D 0x04

/*
 * The USB controller should be connected to IRQ11,
 * the network controller should be connected to IRQ15.
 */

#define IRQ_BITMAP_LINK0 0x0800
#define IRQ_BITMAP_LINK1 0x8000
#define IRQ_BITMAP_LINK2 0x0000
#define IRQ_BITMAP_LINK3 0x0000

/** Reserved interrupt channels for exclusive PCI usage. */
#define IRQ_DEVOTED_TO_PCI (IRQ_BITMAP_LINK0 | IRQ_BITMAP_LINK1)

/**
 * Routing description.
 * Documentation at: http://www.microsoft.com/whdc/archive/pciirq.mspx
 */
static const struct irq_routing_table intel_irq_routing_table = {
	.signature = PIRQ_SIGNATURE,	/* PIRQ signature */
	.version = PIRQ_VERSION,	/* PIRQ version */
	.size = 32 + 16 * CONFIG_IRQ_SLOT_COUNT,/* Max. CONFIG_IRQ_SLOT_COUNT devices */
	.rtr_bus = 0x00,		/* Interrupt router bus */
	.rtr_devfn = (0x12 << 3) | 0x0,	/* Interrupt router device */
	.exclusive_irqs = IRQ_DEVOTED_TO_PCI,	/* IRQs devoted to PCI */
	.rtr_vendor = 0x1078,		/* Vendor */
	.rtr_device = 0x0100,		/* Device */
	.miniport_data = 0,		/* Miniport data */
	.checksum = 0xe3,		/* Checksum */
	.slots = {
		/*
		 * Definition for "slot#1". There is no real slot,
		 * the USB device is embedded...
		 */
		[0] = {
			.bus = 0x00,
			.devfn = (0x13 << 3) | 0x0,
			.irq = {
				/*      Link   Bitmap */
				[0] = { INT_A, IRQ_BITMAP_LINK0 },
				[1] = { INT_B, IRQ_BITMAP_LINK1 },
				[2] = { INT_C, IRQ_BITMAP_LINK2 },
				[3] = { INT_D, IRQ_BITMAP_LINK3 },
			},
			.slot = 0x0,
		},
		/*
		 * Definition for "slot#2". There is no real slot,
		 * the network device is soldered...
		 */
		[1] = {
			.bus = 0x00,
			.devfn = (0x0e << 3) | 0x0,
			.irq = {
				/*      Link   Bitmap */
				[0] = { INT_B, IRQ_BITMAP_LINK1 },
				[1] = { INT_C, IRQ_BITMAP_LINK2 },
				[2] = { INT_D, IRQ_BITMAP_LINK3 },
				[3] = { INT_A, IRQ_BITMAP_LINK0	},
			},
			.slot = 0x0,
		}
	}
};

/**
 * Copy the IRQ routing table to memory.
 *
 * @param[in] addr Destination address (between 0xF0000...0x100000).
 * @return TODO.
 */
unsigned long write_pirq_routing_table(unsigned long addr)
{
	return copy_pirq_routing_table(addr, &intel_irq_routing_table);
}
