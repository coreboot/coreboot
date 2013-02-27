/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Travelping GmbH <info@travelping.com>
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

static const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE,		/* u32 signature */
	PIRQ_VERSION,		/* u16 version */
	32 + 16 * 10,		/* Max. number of devices on the bus */
	0x00,			/* Interrupt router bus */
	(0x1f << 3) | 0x0,	/* Interrupt router dev */
	0x1e20,			/* IRQs devoted exclusively to PCI usage */
	0x8086,			/* Vendor */
	0x24cc,			/* Device */
	0,			/* Miniport */
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* u8 rfu[11] */
	0x39,			/* Checksum (has to be set to some value that
				 * would give 0 after the sum of all bytes
				 * for this structure (including checksum).
                                 */
	{
		/* bus,        dev | fn,   {link, bitmap}, {link, bitmap}, {link, bitmap}, {link, bitmap}, slot, rfu */
		{0x01, (0x0f << 3) | 0x0, {{0x62, 0xdeb8}, {0x63, 0xdeb8}, {0x60, 0xdeb8}, {0x61, 0xdeb8}}, 0x1, 0x0},
		{0x01, (0x0a << 3) | 0x0, {{0x62, 0xdeb8}, {0x63, 0xdeb8}, {0x60, 0xdeb8}, {0x61, 0xdeb8}}, 0x2, 0x0},
		{0x01, (0x05 << 3) | 0x0, {{0x68, 0xdeb8}, {0x69, 0xdeb8}, {0x6a, 0xdeb8}, {0x6b, 0xdeb8}}, 0x3, 0x0},
		{0x01, (0x06 << 3) | 0x0, {{0x69, 0xdeb8}, {0x6a, 0xdeb8}, {0x6b, 0xdeb8}, {0x68, 0xdeb8}}, 0x4, 0x0},
		{0x01, (0x07 << 3) | 0x0, {{0x6a, 0xdeb8}, {0x6b, 0xdeb8}, {0x68, 0xdeb8}, {0x69, 0xdeb8}}, 0x5, 0x0},
		{0x01, (0x09 << 3) | 0x0, {{0x6b, 0xdeb8}, {0x68, 0xdeb8}, {0x69, 0xdeb8}, {0x6a, 0xdeb8}}, 0x6, 0x0},
		{0x00, (0x1e << 3) | 0x0, {{0x60, 0xdeb8}, {0x61, 0xdeb8}, {0x62, 0xdeb8}, {0x63, 0xdeb8}}, 0x0, 0x0},
		{0x00, (0x02 << 3) | 0x0, {{0x60, 0xdeb8}, {0x60, 0xdeb8}, {0x60, 0xdeb8}, {0x60, 0xdeb8}}, 0x0, 0x0},
		{0x00, (0x1f << 3) | 0x0, {{0x62, 0xdeb8}, {0x61, 0xdeb8}, {0x60, 0xdeb8}, {0x63, 0xdeb8}}, 0x0, 0x0},
		{0x00, (0x1d << 3) | 0x0, {{0x60, 0xdeb8}, {0x63, 0xdeb8}, {0x62, 0xdeb8}, {0x6b, 0xdeb8}}, 0x0, 0x0},
	}
};

unsigned long write_pirq_routing_table(unsigned long addr)
{
	return copy_pirq_routing_table(addr, &intel_irq_routing_table);
}
