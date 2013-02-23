/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 LiPPERT Embedded Computers GmbH
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

/* Based on irq_tables.c from the SpaceRunner-LX mainboard. */

#include <arch/pirq_routing.h>
#include <console/console.h>
#include <arch/io.h>
#include <arch/pirq_routing.h>
#include "southbridge/amd/cs5536/cs5536.h"

/* Platform IRQs */
#define PIRQA 10
#define PIRQB 11
#define PIRQC 5
#define PIRQD 15

/* Map */
#define M_PIRQA (1 << PIRQA)	/* Bitmap of supported IRQs */
#define M_PIRQB (1 << PIRQB)	/* Bitmap of supported IRQs */
#define M_PIRQC (1 << PIRQC)	/* Bitmap of supported IRQs */
#define M_PIRQD (1 << PIRQD)	/* Bitmap of supported IRQs */

/* Link */
#define L_PIRQA	 1		/* Means Slot INTx# Connects To Chipset INTA# */
#define L_PIRQB	 2		/* Means Slot INTx# Connects To Chipset INTB# */
#define L_PIRQC	 3		/* Means Slot INTx# Connects To Chipset INTC# */
#define L_PIRQD	 4		/* Means Slot INTx# Connects To Chipset INTD# */

static const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE,		/* u32 signature */
	PIRQ_VERSION,		/* u16 version   */
	32 + 16 * CONFIG_IRQ_SLOT_COUNT,/* There can be total CONFIG_IRQ_SLOT_COUNT devices on the bus */
	0x00,			/* Where the interrupt router lies (bus) */
	(0x0F << 3) | 0x0,	/* Where the interrupt router lies (dev) */
	0x00,			/* IRQs devoted exclusively to PCI usage */
	0x100B,			/* Vendor */
	0x002B,			/* Device */
	0,			/* Miniport data */
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/* u8 rfu[11] */
	0x36,			/* u8 checksum, this has to set to some value that would give 0 after the sum of all bytes for this structure (including checksum) */
	{
		/* If you change the number of entries, change the CONFIG_IRQ_SLOT_COUNT above! */
		/* bus, dev|fn,           {link, bitmap},      {link, bitmap},     {link, bitmap},     {link, bitmap},     slot, rfu */
		{0x00, (0x01 << 3) | 0x0, {{L_PIRQA, M_PIRQA}, {0x00, 0x00},       {0x00, 0x00},       {0x00, 0x00}},       0x0, 0x0},	/* CPU */
		{0x00, (0x0F << 3) | 0x0, {{L_PIRQA, M_PIRQA}, {L_PIRQB, M_PIRQB}, {L_PIRQC, M_PIRQC}, {L_PIRQD, M_PIRQD}}, 0x0, 0x0},	/* chipset */
		{0x00, (0x0E << 3) | 0x0, {{L_PIRQC, M_PIRQC}, {0x00, 0x00},       {0x00, 0x00},       {0x00, 0x00}},       0x0, 0x0},	/* ethernet */
		{0x00, (0x0B << 3) | 0x0, {{L_PIRQA, M_PIRQA}, {L_PIRQB, M_PIRQB}, {L_PIRQC, M_PIRQC}, {L_PIRQD, M_PIRQD}}, 0x1, 0x0},	/* slot1 */
		{0x00, (0x0A << 3) | 0x0, {{L_PIRQB, M_PIRQB}, {L_PIRQC, M_PIRQC}, {L_PIRQD, M_PIRQD}, {L_PIRQA, M_PIRQA}}, 0x2, 0x0},	/* slot2 */
		{0x00, (0x09 << 3) | 0x0, {{L_PIRQC, M_PIRQC}, {L_PIRQD, M_PIRQD}, {L_PIRQA, M_PIRQA}, {L_PIRQB, M_PIRQB}}, 0x3, 0x0},	/* slot3 */
		{0x00, (0x08 << 3) | 0x0, {{L_PIRQD, M_PIRQD}, {L_PIRQA, M_PIRQA}, {L_PIRQB, M_PIRQB}, {L_PIRQC, M_PIRQC}}, 0x4, 0x0},	/* slot4 */
		{0x00, (0x0D << 3) | 0x0, {{L_PIRQB, M_PIRQB}, {L_PIRQA, M_PIRQA}, {0x00, 0x00},       {0x00, 0x00}},       0x5, 0x0},	/* Mini-PCI */
	}
};

unsigned long write_pirq_routing_table(unsigned long addr)
{
	return copy_pirq_routing_table(addr, &intel_irq_routing_table);
}
