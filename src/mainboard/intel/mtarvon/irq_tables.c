/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Arastra, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include <arch/pirq_routing.h>

static const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE, /* u32 signature */
	PIRQ_VERSION,   /* u16 version   */
	32+16*CONFIG_IRQ_SLOT_COUNT, /* u16 Table size 32+(16*devices)  */
	0x00,       /* u8 Bus 0 */
	(0x1f << 3) | 0x0, /* u8 Device 1f, Function 0 */
	0x0000,     /* u16 reserve IRQ for PCI */
	0x8086,     /* u16 Vendor */
	0x2670,     /* Device ID */
	0x00000000, /* u32 miniport_data */
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* u8 rfu[11] */
	0x49,   /*  u8 checksum - mod 256 checksum must give zero */
	{  /* bus, devfn, {link, bitmap}, {link, bitmap}, {link, bitmap}, {link, bitmap}, slot, rfu  */
	    {0x00, 0xf8, {{0x62, 0xdc78}, {0x61, 0xdcf8}, {0x00, 0x0000}, {0x00, 0x0000}}, 0x00,  0x00},
	}
};

unsigned long write_pirq_routing_table(unsigned long addr)
{
	return copy_pirq_routing_table(addr, &intel_irq_routing_table);
}

