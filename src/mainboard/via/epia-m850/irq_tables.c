/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <arch/pirq_routing.h>
#include <console/console.h>
#include <device/pci_ids.h>
#include <string.h> /* <- For memset */

#define _OFF 0x00
#define ___OFF 0x0000
#define LNKA 1
#define LNKB 2
#define LNKC 3
#define LNKD 4
#define LNKE 5
#define LNKF 6
#define LNKG 7
#define LNKH 8
#define BITMAP 0xdce0
/* The link that carries the SATA interrupt has its own mask, just in case
 * we want to make sure our SATA controller gets mapped to IRQ 14 */
#define B_SATA BITMAP

const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE,			/* u32 signature */
	PIRQ_VERSION,			/* u16 version */
	32 + 16 * 13,			/* Max. number of devices on the bus */
	0x00,				/* Interrupt router bus */
	(0x11 << 3) | 0x0,		/* Interrupt router dev */
	0,				/* IRQs devoted exclusively for PCI */
	PCI_VENDOR_ID_VIA,		/* Vendor */
	PCI_DEVICE_ID_VIA_VX900_LPC,	/* Device */
	0,				/* Miniport */
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* u8 rfu[11] */
	0x19,			/* Checksum (has to be set to some value that
				 * would give 0 after the sum of all bytes
				 * for this structure (including checksum). */
	{
		/* bus,        dev | fn,   {link, bitmap}, {link, bitmap}, {link, bitmap}, {link, bitmap}, slot, rfu */
		{0x00, (0x01 << 3) | 0x0, {{LNKH, BITMAP}, {LNKH, BITMAP}, {_OFF, ___OFF}, {_OFF, ___OFF}}, 0x0, 0x0},
		{0x00, (0x03 << 3) | 0x0, {{LNKH, BITMAP}, {LNKH, BITMAP}, {LNKH, BITMAP}, {LNKH, BITMAP}}, 0x0, 0x0},
		{0x00, (0x0a << 3) | 0x0, {{LNKA, BITMAP}, {LNKB, B_SATA}, {LNKC, BITMAP}, {LNKD, BITMAP}}, 0x0, 0x0},
		{0x00, (0x0b << 3) | 0x0, {{LNKA, BITMAP}, {_OFF, ___OFF}, {_OFF, ___OFF}, {_OFF, ___OFF}}, 0x0, 0x0},
		{0x00, (0x0c << 3) | 0x0, {{LNKA, BITMAP}, {_OFF, ___OFF}, {_OFF, ___OFF}, {_OFF, ___OFF}}, 0x0, 0x0},
		{0x00, (0x0d << 3) | 0x0, {{LNKA, BITMAP}, {_OFF, ___OFF}, {_OFF, ___OFF}, {_OFF, ___OFF}}, 0x0, 0x0},
		{0x00, (0x0f << 3) | 0x0, {{LNKB, B_SATA}, {_OFF, ___OFF}, {_OFF, ___OFF}, {_OFF, ___OFF}}, 0x0, 0x0},
		{0x00, (0x10 << 3) | 0x0, {{LNKA, BITMAP}, {LNKB, B_SATA}, {LNKC, BITMAP}, {LNKD, BITMAP}}, 0x0, 0x0},
		{0x00, (0x14 << 3) | 0x0, {{LNKB, B_SATA}, {_OFF, ___OFF}, {_OFF, ___OFF}, {_OFF, ___OFF}}, 0x0, 0x0},
		{0x01, (0x00 << 3) | 0x0, {{LNKH, BITMAP}, {LNKH, BITMAP}, {LNKH, BITMAP}, {LNKH, BITMAP}}, 0x1, 0x0},
		{0x02, (0x00 << 3) | 0x0, {{LNKH, BITMAP}, {LNKH, BITMAP}, {LNKH, BITMAP}, {LNKH, BITMAP}}, 0x2, 0x0},
		{0x03, (0x00 << 3) | 0x0, {{LNKH, BITMAP}, {LNKH, BITMAP}, {LNKH, BITMAP}, {LNKH, BITMAP}}, 0x0, 0x0},
		{0x04, (0x00 << 3) | 0x0, {{LNKH, BITMAP}, {LNKH, BITMAP}, {LNKH, BITMAP}, {LNKH, BITMAP}}, 0x0, 0x0},
	}
};

unsigned long write_pirq_routing_table(unsigned long addr)
{
	return copy_pirq_routing_table(addr, &intel_irq_routing_table);
}
