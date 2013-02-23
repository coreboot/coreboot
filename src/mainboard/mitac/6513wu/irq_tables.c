/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 Michael Gold <mgold@ncf.ca>
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
 * Each of PIRQA..D can be routed to IRQ 3-7, 9-12, 14, or 15; but the
 * selected IRQs can't be shared with ISA devices (Intel DS 290655-003,
 * section 5.7.6).
 *
 * Correspondingly, the IRQs used on the Super I/O (4,6,7,10,12) are
 * excluded from the masks, leaving 0xca28 (3,5,9,11,14,15).
 */

static const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE,         /* u32 signature */
	PIRQ_VERSION,           /* u16 version */
	32 + 16 * CONFIG_IRQ_SLOT_COUNT,/* Max. number of devices on the bus */
	0x00,                   /* Interrupt router bus */
	(0x1f << 3) | 0x0,      /* Interrupt router dev */
	0,                      /* IRQs devoted exclusively to PCI usage */
	0x8086,                 /* Vendor */
	0x7000,                 /* Device */
	0,                      /* Miniport */
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* u8 rfu[11] */
	0xb6,                   /* Checksum (has to be set to some value that
	                         * would give 0 after the sum of all bytes
	                         * for this structure (including checksum).
	                         */
	{
		/* bus,        dev | fn,   {link, bitmap}, {link, bitmap}, {link, bitmap}, {link, bitmap}, slot, rfu */
		{0x00, (0x1f << 3) | 0x0, {{0x00, 0x0000}, {0x61, 0xca28}, {0x00, 0x0000}, {0x63, 0xca28}}, 0x0, 0x0},
		{0x00, (0x1e << 3) | 0x0, {{0x60, 0xca28}, {0x61, 0xca28}, {0x62, 0xca28}, {0x63, 0xca28}}, 0x0, 0x0},
		{0x00, (0x01 << 3) | 0x0, {{0x60, 0xca28}, {0x00, 0x0000}, {0x00, 0x0000}, {0x00, 0x0000}}, 0x0, 0x0},
		{0x01, (0x05 << 3) | 0x0, {{0x63, 0xca28}, {0x00, 0x0000}, {0x00, 0x0000}, {0x00, 0x0000}}, 0x0, 0x0},
		{0x01, (0x08 << 3) | 0x0, {{0x60, 0xca28}, {0x61, 0xca28}, {0x62, 0xca28}, {0x63, 0xca28}}, 0x1, 0x0},
		{0x01, (0x09 << 3) | 0x0, {{0x61, 0xca28}, {0x62, 0xca28}, {0x63, 0xca28}, {0x60, 0xca28}}, 0x2, 0x0},
		{0x01, (0x0a << 3) | 0x0, {{0x62, 0xca28}, {0x63, 0xca28}, {0x60, 0xca28}, {0x61, 0xca28}}, 0x3, 0x0},
		{0x01, (0x0b << 3) | 0x0, {{0x63, 0xca28}, {0x60, 0xca28}, {0x61, 0xca28}, {0x62, 0xca28}}, 0x4, 0x0},
	}
};

unsigned long write_pirq_routing_table(unsigned long addr)
{
	return copy_pirq_routing_table(addr, &intel_irq_routing_table);
}
