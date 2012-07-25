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
#include <device/pci_ids.h>
#include <string.h> /* <- For memset */


#include <console/console.h>

const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE,		/* u32 signature */
	PIRQ_VERSION,		/* u16 version */
	32 + 16 * 13,		/* Max. number of devices on the bus */
	0x00,			/* Interrupt router bus */
	(0x11 << 3) | 0x0,	/* Interrupt router dev */
	0,			/* IRQs devoted exclusively to PCI usage */
	0x1106,			/* Vendor */
	0x8410,			/* Device */
	0,			/* Miniport */
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* u8 rfu[11] */
	0xb8,			/* Checksum (has to be set to some value that
	* would give 0 after the sum of all bytes
	* for this structure (including checksum).
	*/
	{
		/* bus,        dev | fn,   {link, bitmap}, {link, bitmap}, {link, bitmap}, {link, bitmap}, slot, rfu */
		{0x00, (0x01 << 3) | 0x0, {{0x09, 0xdcf8}, {0x09, 0xdcf8}, {0x00, 0x0000}, {0x00, 0x0000}}, 0x0, 0x0},
		{0x00, (0x03 << 3) | 0x0, {{0x09, 0xdcf8}, {0x09, 0xdcf8}, {0x09, 0xdcf8}, {0x09, 0xdcf8}}, 0x0, 0x0},
		{0x00, (0x0a << 3) | 0x0, {{0x01, 0xdcf8}, {0x02, 0xdcf8}, {0x03, 0xdcf8}, {0x05, 0xdcf8}}, 0x0, 0x0},
		{0x00, (0x0b << 3) | 0x0, {{0x01, 0xdcf8}, {0x00, 0x0000}, {0x00, 0x0000}, {0x00, 0x0000}}, 0x0, 0x0},
		{0x00, (0x0c << 3) | 0x0, {{0x01, 0xdcf8}, {0x00, 0x0000}, {0x00, 0x0000}, {0x00, 0x0000}}, 0x0, 0x0},
		{0x00, (0x0d << 3) | 0x0, {{0x01, 0xdcf8}, {0x00, 0x0000}, {0x00, 0x0000}, {0x00, 0x0000}}, 0x0, 0x0},
		{0x00, (0x0f << 3) | 0x0, {{0x02, 0xdcf8}, {0x00, 0x0000}, {0x00, 0x0000}, {0x00, 0x0000}}, 0x0, 0x0},
		{0x00, (0x10 << 3) | 0x0, {{0x01, 0xdcf8}, {0x02, 0xdcf8}, {0x03, 0xdcf8}, {0x05, 0xdcf8}}, 0x0, 0x0},
		{0x00, (0x14 << 3) | 0x0, {{0x02, 0xdcf8}, {0x00, 0x0000}, {0x00, 0x0000}, {0x00, 0x0000}}, 0x0, 0x0},
		{0x01, (0x00 << 3) | 0x0, {{0x09, 0xdcf8}, {0x09, 0xdcf8}, {0x09, 0xdcf8}, {0x09, 0xdcf8}}, 0x1, 0x0},
		{0x02, (0x00 << 3) | 0x0, {{0x09, 0xdcf8}, {0x09, 0xdcf8}, {0x09, 0xdcf8}, {0x09, 0xdcf8}}, 0x2, 0x0},
		{0x03, (0x00 << 3) | 0x0, {{0x09, 0xdcf8}, {0x09, 0xdcf8}, {0x09, 0xdcf8}, {0x09, 0xdcf8}}, 0x0, 0x0},
		{0x05, (0x00 << 3) | 0x0, {{0x09, 0xdcf8}, {0x09, 0xdcf8}, {0x09, 0xdcf8}, {0x09, 0xdcf8}}, 0x0, 0x0},
	}
};

static void fill_pirq_table_header(struct irq_routing_table *pirq)
{
	pirq->signature = PIRQ_SIGNATURE;
	pirq->version = PIRQ_VERSION;
	pirq->rtr_bus = 0;
	pirq->rtr_devfn = (0x11 << 3) | 0x0;
	pirq->exclusive_irqs = 0;
	pirq->rtr_vendor = PCI_VENDOR_ID_VIA;
	pirq->rtr_device = PCI_DEVICE_ID_VIA_VX900_LPC;
	pirq->miniport_data = 0;
	memset(pirq->rfu, 0, sizeof(pirq->rfu));
}

static void fill_pirq_table_checksum(struct irq_routing_table *pirq)
{
	size_t i;
	u8 checksum = 0;
	u8* base = (void*)pirq;
	for (i = 0; i < pirq->size; i++) {
		checksum += base[i];
	}
	pirq->checksum -= checksum;
}

static unsigned long smart_write_pirq_routing_table(unsigned long addr)
{
	/* Align the table to 16 bytes just to be sure */
	addr = (addr + 0xf) & ~0xf;
	printk(BIOS_INFO, "Writing PIRQ routing table to 0x%lx...\n", addr);

	/* Now start messing with the actual table */
	struct irq_routing_table *pirq = (void *) addr;
	/* Take care of the header */
	fill_pirq_table_header(pirq);

	/* Ya' though we forgot about the checksum, didn't ya' ? */
	fill_pirq_table_checksum(pirq);

	return addr;
}

unsigned long write_pirq_routing_table(unsigned long addr)
{
	smart_write_pirq_routing_table(addr);

	/* The auto generation is not done, so just copy the static table */
	return copy_pirq_routing_table(addr);
}
