/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Joseph Smith <joe@smittys.pointclark.net>
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

const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE,  /* u32 signature */
	PIRQ_VERSION,    /* u16 version   */
	32+16*IRQ_SLOT_COUNT,	 /* there can be total 10 devices on the bus */
	0x00,		 /* Where the interrupt router lies (bus) */
	(0x1f<<3)|0x0,   /* Where the interrupt router lies (dev) */
	0,		 /* IRQs devoted exclusively to PCI usage */
	0x8086,		 /* Vendor */
	0x24c0,		 /* Device */
	0,		 /* Crap (miniport) */
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* u8 rfu[11] */
	0x14,         /*  u8 checksum , this hase to set to some value that would give 0 after the sum of all bytes for this structure (including checksum) */
	{
		/* bus,     dev|fn,   {link, bitmap}, {link, bitmap}, {link, bitmap}, {link, bitmap},  slot, rfu */
		{0x00,(0x1d<<3)|0x0, {{0x60, 0x1ef8}, {0x61, 0x1ef8}, {0x62, 0x1ef8}, {0x63, 0x01ef8}}, 0x0, 0x0}, /* [A] USB UHCI Controller #1 */
		{0x00,(0x1d<<3)|0x1, {{0x61, 0x1ef8}, {0x62, 0x1ef8}, {0x63, 0x1ef8}, {0x60, 0x01ef8}}, 0x0, 0x0}, /* [B] USB UHCI Controller #2 */
		{0x00,(0x1d<<3)|0x2, {{0x62, 0x1ef8}, {0x63, 0x1ef8}, {0x60, 0x1ef8}, {0x61, 0x01ef8}}, 0x0, 0x0}, /* [C] USB UHCI Controller #3 */
		{0x00,(0x1d<<3)|0x7, {{0x63, 0x1ef8}, {0x60, 0x1ef8}, {0x61, 0x1ef8}, {0x62, 0x01ef8}}, 0x0, 0x0}, /* [D] USB2 EHCI Controller */
		{0x00,(0x1f<<3)|0x1, {{0x60, 0x1ef8}, {0x61, 0x1ef8}, {0x62, 0x1ef8}, {0x63, 0x01ef8}}, 0x0, 0x0}, /* [A] IDE Controller */
		{0x00,(0x1f<<3)|0x3, {{0x61, 0x1ef8}, {0x62, 0x1ef8}, {0x63, 0x1ef8}, {0x60, 0x01ef8}}, 0x0, 0x0}, /* [B] SMBus Controller */
		{0x00,(0x02<<3)|0x0, {{0x60, 0x1ef8}, {0x61, 0x1ef8}, {0x62, 0x1ef8}, {0x63, 0x01ef8}}, 0x0, 0x0}, /* [A] VGA compatible controller */
		{0x00,(0x1f<<3)|0x5, {{0x61, 0x1ef8}, {0x62, 0x1ef8}, {0x63, 0x1ef8}, {0x60, 0x01ef8}}, 0x0, 0x0}, /* [B] AC'97 Audio Controller */
		{0x01,(0x08<<3)|0x0, {{0x68, 0x1ef8}, {0x00, 0x0000}, {0x00, 0x0000}, {0x00, 0x00000}}, 0x0, 0x0}, /* [A] Ethernet controller */
		{0x00,(0x1f<<3)|0x6, {{0x61, 0x1ef8}, {0x62, 0x1ef8}, {0x63, 0x1ef8}, {0x64, 0x01ef8}}, 0x0, 0x0}, /* [B] AC'97 Modem Controller */
	}
};

unsigned long write_pirq_routing_table(unsigned long addr)
{
	return copy_pirq_routing_table(addr);
}
