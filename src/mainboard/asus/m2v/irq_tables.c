/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Tobias Diedrich <ranma+coreboot@tdiedrich.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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
 * IRQ Routing Table
 *
 * Documentation at : http://www.microsoft.com/hwdev/busbios/PCIIRQ.HTM
 */
#include <string.h>
#include <stdint.h>
#include <arch/pirq_routing.h>
#include <device/pci_ids.h>

/* Free irqs are 3, 5, 10 and 11 */
#define IRQBM ((1<<3)|(1<<5)|(1<<10)|(1<<11))

#define LNKA 1
#define LNKB 2
#define LNKC 3
#define LNKD 4

/*
 * For simplicity map LNK[E-H] to LNK[A-D].
 * This also means we are 82C596 compatible.
 * Needs 0:11.0 0x46[4] set to 0.
 */
#define LNKE 1
#define LNKF 2
#define LNKG 3
#define LNKH 4

static const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE, /* u32 signature */
	PIRQ_VERSION,   /* u16 version   */
	32+16*CONFIG_IRQ_SLOT_COUNT,        /* there can be total CONFIG_IRQ_SLOT_COUNT devices on the bus */
	0,           /* Where the interrupt router lies (bus) */
	(0x11<<3)|0, /* Where the interrupt router lies (dev) */
	0,           /* IRQs devoted exclusively to PCI usage */
	PCI_VENDOR_ID_VIA,         /* Compatible Vendor (VIA) */
	PCI_DEVICE_ID_VIA_82C596,  /* Compatible Device (82C596) */
	0,           /* Miniport data */
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* u8 rfu[11] */
	0x5f,        /*  u8 checksum, this has to be set to some value that would give 0 after the sum of all bytes for this structure (including checksum) */
	{
		/* bus,        dev | fn,   {link, bitmap}, {link, bitmap}, {link, bitmap}, {link, bitmap}, slot, rfu */
		/* peg bridge */
		{0x00, (0x02 << 3) | 0x0, {{LNKH, IRQBM}, {LNKH, IRQBM}, {LNKH, IRQBM}, {LNKH, IRQBM}}, 0x0, 0x0},
		/* pcie bridge */
		{0x00, (0x03 << 3) | 0x0, {{LNKH, IRQBM}, {LNKH, IRQBM}, {LNKH, IRQBM}, {LNKH, IRQBM}}, 0x0, 0x0},
		/* sata/ide */
		{0x00, (0x0f << 3) | 0x0, {{0x00, 0x0000}, {LNKB, IRQBM}, {0x00, 0x0000}, {0x00, 0x0000}}, 0x0, 0x0},
		/* usb */
		{0x00, (0x10 << 3) | 0x0, {{LNKA, IRQBM}, {LNKB, IRQBM}, {LNKC, IRQBM}, {LNKD, IRQBM}}, 0x0, 0x0},
		/* agp bus? */
		{0x01, (0x00 << 3) | 0x0, {{LNKA, IRQBM}, {LNKB, IRQBM}, {LNKC, IRQBM}, {LNKD, IRQBM}}, 0x0, 0x0},
		/* pcie graphics */
		{0x02, (0x00 << 3) | 0x0, {{LNKH, IRQBM}, {LNKH, IRQBM}, {LNKH, IRQBM}, {LNKH, IRQBM}}, 0x6, 0x0},
		/* onboard pcie atl1 ethernet */
		{0x03, (0x00 << 3) | 0x0, {{LNKH, IRQBM}, {LNKH, IRQBM}, {LNKH, IRQBM}, {LNKH, IRQBM}}, 0x0, 0x0},
		/* pcie slot */
		{0x04, (0x00 << 3) | 0x0, {{LNKH, IRQBM}, {LNKH, IRQBM}, {LNKH, IRQBM}, {LNKH, IRQBM}}, 0x5, 0x0},
		/* onboard marvell mv6121 sata */
		{0x05, (0x00 << 3) | 0x0, {{LNKH, IRQBM}, {LNKH, IRQBM}, {LNKH, IRQBM}, {LNKH, IRQBM}}, 0x0, 0x0},
		/* Azalia HDAC */
		{0x06, (0x01 << 3) | 0x0, {{LNKB, IRQBM}, {0x00, 0x0000}, {0x00, 0x0000}, {0x00, 0x0000}}, 0x0, 0x0},
		/* PCI slots */
		{0x07, (0x06 << 3) | 0x0, {{LNKA, IRQBM}, {LNKB, IRQBM}, {LNKC, IRQBM}, {LNKD, IRQBM}}, 0x1, 0x0},
		{0x07, (0x07 << 3) | 0x0, {{LNKB, IRQBM}, {LNKC, IRQBM}, {LNKD, IRQBM}, {LNKA, IRQBM}}, 0x2, 0x0},
		{0x07, (0x08 << 3) | 0x0, {{LNKC, IRQBM}, {LNKD, IRQBM}, {LNKA, IRQBM}, {LNKB, IRQBM}}, 0x3, 0x0},
		{0x07, (0x09 << 3) | 0x0, {{LNKD, IRQBM}, {LNKA, IRQBM}, {LNKB, IRQBM}, {LNKC, IRQBM}}, 0x4, 0x0},
	}
};

unsigned long write_pirq_routing_table(unsigned long addr)
{
	return copy_pirq_routing_table(addr, &intel_irq_routing_table);
}
