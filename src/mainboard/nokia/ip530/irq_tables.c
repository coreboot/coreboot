/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Marc Bertens <mbertens@xs4all.nl>
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

#define PIRQ_IRQ_MASK 0x0c60

static const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE,		/* u32 signature */
	PIRQ_VERSION,		/* u16 version */
	32 + 16 * CONFIG_IRQ_SLOT_COUNT,/* Max. number of devices on the bus */
	0x00,			/* Interrupt router bus */
	(0x07 << 3) | 0x0,	/* Interrupt router dev */
	0,			/* IRQs devoted exclusively to PCI usage */
	0x8086,			/* Vendor */
	0x122e,			/* Device */
	0,			/* Miniport */
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* u8 rfu[11] */
	0xD7,			/* Checksum */
	{
		/**
		 * Rebuild of the PIRQ table, to fix the non-working on-board NIC and PCMCIA controller.
		 *          FEDCBA9876543210
		 * 0x1E20 = 0001111000100000
		 * 0x0C60 = 0000110001100000
		 */
		// Southbridge 82371EB, INTD = 0x63
		{ 0x00, (0x07 << 3) | 0x0, {{0x00, PIRQ_IRQ_MASK}, {0x00, PIRQ_IRQ_MASK}, {0x00, PIRQ_IRQ_MASK}, {0x00, PIRQ_IRQ_MASK}}, 0x0, 0x0 },
		// On-board PCI-to-PCI bridge
		{ 0x01, (0x00 << 3) | 0x0, {{0x60, PIRQ_IRQ_MASK}, {0x61, PIRQ_IRQ_MASK}, {0x62, PIRQ_IRQ_MASK}, {0x63, PIRQ_IRQ_MASK}}, 0x0, 0x0 },
		// ETH1 on front panel, INTA = 0x62 = ok
		{ 0x00, (0x0d << 3) | 0x0, {{0x62, PIRQ_IRQ_MASK}, {0x00, PIRQ_IRQ_MASK}, {0x00, PIRQ_IRQ_MASK}, {0x00, PIRQ_IRQ_MASK}}, 0x0, 0x0 },

		// ETH2 on front panel, 0x63
		{ 0x00, (0x0e << 3) | 0x0, {{0x63, PIRQ_IRQ_MASK}, {0x00, PIRQ_IRQ_MASK}, {0x00, PIRQ_IRQ_MASK}, {0x00, PIRQ_IRQ_MASK}}, 0x0, 0x0 },
		// ETH3 on front panel = 0x60
		{ 0x02, (0x04 << 3) | 0x0, {{0x60, PIRQ_IRQ_MASK}, {0x00, PIRQ_IRQ_MASK}, {0x00, PIRQ_IRQ_MASK}, {0x00, PIRQ_IRQ_MASK}}, 0x0, 0x0 },

		// ETH4 on front panel, INTA = 0x61 = ok
		{ 0x02, (0x05 << 3) | 0x0, {{0x61, PIRQ_IRQ_MASK}, {0x00, PIRQ_IRQ_MASK}, {0x00, PIRQ_IRQ_MASK}, {0x00, PIRQ_IRQ_MASK}}, 0x0, 0x0 },
		// PCMCIA/Cardbus controller, INTA = 0x60 = ok, INTB = 0x61 = ok
		{ 0x00, (0x0f << 3) | 0x0, {{0x60, PIRQ_IRQ_MASK}, {0x61, PIRQ_IRQ_MASK}, {0x00, PIRQ_IRQ_MASK}, {0x00, PIRQ_IRQ_MASK}}, 0x0, 0x0 },
		// Bridge for slot 1 (top)
		{ 0x02, (0x07 << 3) | 0x0, {{0x61, PIRQ_IRQ_MASK}, {0x62, PIRQ_IRQ_MASK}, {0x63, PIRQ_IRQ_MASK}, {0x64, PIRQ_IRQ_MASK}}, 0x0, 0x0 },
		// PCI compact slots 1 (top)
		{ 0x03, (0x04 << 3) | 0x0, {{0x61, PIRQ_IRQ_MASK}, {0x62, PIRQ_IRQ_MASK}, {0x63, PIRQ_IRQ_MASK}, {0x60, PIRQ_IRQ_MASK}}, 0x1, 0x0 },
		{ 0x03, (0x05 << 3) | 0x0, {{0x62, PIRQ_IRQ_MASK}, {0x63, PIRQ_IRQ_MASK}, {0x60, PIRQ_IRQ_MASK}, {0x61, PIRQ_IRQ_MASK}}, 0x2, 0x0 },
		{ 0x03, (0x06 << 3) | 0x0, {{0x63, PIRQ_IRQ_MASK}, {0x60, PIRQ_IRQ_MASK}, {0x61, PIRQ_IRQ_MASK}, {0x62, PIRQ_IRQ_MASK}}, 0x3, 0x0 },
		{ 0x03, (0x07 << 3) | 0x0, {{0x60, PIRQ_IRQ_MASK}, {0x61, PIRQ_IRQ_MASK}, {0x62, PIRQ_IRQ_MASK}, {0x63, PIRQ_IRQ_MASK}}, 0x4, 0x0 },
		// Bridge for slot 2 (middle)
		{ 0x02, (0x06 << 3) | 0x0, {{0x61, PIRQ_IRQ_MASK}, {0x62, PIRQ_IRQ_MASK}, {0x63, PIRQ_IRQ_MASK}, {0x60, PIRQ_IRQ_MASK}}, 0x0, 0x0 },
		// PCI compact slots 2 (middle)
		{ 0x04, (0x04 << 3) | 0x0, {{0x61, PIRQ_IRQ_MASK}, {0x62, PIRQ_IRQ_MASK}, {0x63, PIRQ_IRQ_MASK}, {0x60, PIRQ_IRQ_MASK}}, 0x5, 0x0 },
		{ 0x04, (0x05 << 3) | 0x0, {{0x62, PIRQ_IRQ_MASK}, {0x63, PIRQ_IRQ_MASK}, {0x60, PIRQ_IRQ_MASK}, {0x61, PIRQ_IRQ_MASK}}, 0x6, 0x0 },
		{ 0x04, (0x06 << 3) | 0x0, {{0x63, PIRQ_IRQ_MASK}, {0x60, PIRQ_IRQ_MASK}, {0x61, PIRQ_IRQ_MASK}, {0x62, PIRQ_IRQ_MASK}}, 0x7, 0x0 },
		{ 0x04, (0x07 << 3) | 0x0, {{0x60, PIRQ_IRQ_MASK}, {0x61, PIRQ_IRQ_MASK}, {0x62, PIRQ_IRQ_MASK}, {0x63, PIRQ_IRQ_MASK}}, 0x8, 0x0 },
		// Bridge for slot 3 (bottom)
		{ 0x00, (0x10 << 3) | 0x0, {{0x61, PIRQ_IRQ_MASK}, {0x62, PIRQ_IRQ_MASK}, {0x63, PIRQ_IRQ_MASK}, {0x60, PIRQ_IRQ_MASK}}, 0x0, 0x0 },
		// PCI compact slots 3 (bottom)
		{ 0x05, (0x04 << 3) | 0x0, {{0x61, PIRQ_IRQ_MASK}, {0x62, PIRQ_IRQ_MASK}, {0x63, PIRQ_IRQ_MASK}, {0x60, PIRQ_IRQ_MASK}}, 0x9, 0x0 },
		{ 0x05, (0x05 << 3) | 0x0, {{0x62, PIRQ_IRQ_MASK}, {0x63, PIRQ_IRQ_MASK}, {0x60, PIRQ_IRQ_MASK}, {0x61, PIRQ_IRQ_MASK}}, 0xA, 0x0 },
		{ 0x05, (0x06 << 3) | 0x0, {{0x63, PIRQ_IRQ_MASK}, {0x60, PIRQ_IRQ_MASK}, {0x61, PIRQ_IRQ_MASK}, {0x62, PIRQ_IRQ_MASK}}, 0xB, 0x0 },
		{ 0x05, (0x07 << 3) | 0x0, {{0x60, PIRQ_IRQ_MASK}, {0x61, PIRQ_IRQ_MASK}, {0x62, PIRQ_IRQ_MASK}, {0x63, PIRQ_IRQ_MASK}}, 0xC, 0x0 },
	}
};

unsigned long write_pirq_routing_table(unsigned long addr)
{
	return copy_pirq_routing_table(addr, &intel_irq_routing_table);
}

/**
 * TODO: This stub function is here until the point is solved in the
 * main code of coreboot. See also arch/x86/boot/pirq_tables.c.
 */
void pirq_assign_irqs(const unsigned char pIntAtoD[4])
{
	return;
}
