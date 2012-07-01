/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Ricardo Martins <rasmartins@gmail.com>
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

#include <arch/pirq_routing.h>

/* Platform IRQs */
#define PIRQA 7

/* Map */
#define M_PIRQA (1 << PIRQA)

/* Link */
#define L_PIRQA	 1
#define L_PIRQB	 2
#define L_PIRQC	 3
#define L_PIRQD	 4

#define NC {0x00, 0x0000}

const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE,		/* u32 signature */
	PIRQ_VERSION,		/* u16 version */
	32 + 16 * 3,		/* Max. number of devices on the bus */
	0x00,			/* Interrupt router bus */
	(0x0f << 3) | 0x0,	/* Interrupt router dev */
	0,			/* IRQs devoted exclusively to PCI usage */
	0x1022,			/* Vendor */
	0x2090,			/* Device */
	0,			/* Miniport */
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	0x4b,			/* Checksum */
	{
		/* Host bridge.  */
		{0x00, (0x01 << 3) | 0x0,
		 {{L_PIRQA, M_PIRQA}, NC, NC, NC},
		 0x0, 0x0},
		/* ISA bridge. */
		{0x00, (0x0f << 3) | 0x0,
		 {NC, {L_PIRQB, M_PIRQA}, NC, {L_PIRQD, M_PIRQA}},
		 0x0, 0x0},
		/* Ethernet. */
		{0x00, (0x0e << 3) | 0x0,
		 {{L_PIRQD, M_PIRQA}, NC, NC, NC},
		 0x0, 0x0},
	}
};

unsigned long write_pirq_routing_table(unsigned long addr)
{
	return copy_pirq_routing_table(addr);
}
