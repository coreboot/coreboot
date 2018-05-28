/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Bachmann electronic GmbH
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
 */

#include <arch/pirq_routing.h>

/* Platform IRQs */
#define PIRQA 5
#define PIRQB 9
#define PIRQC 10
#define PIRQD 7

/* Map */
#define M_PIRQA (1 << PIRQA)	/* Bitmap of supported IRQs */
#define M_PIRQB (1 << PIRQB)	/* Bitmap of supported IRQs */
#define M_PIRQC (1 << PIRQC)	/* Bitmap of supported IRQs */
#define M_PIRQD (1 << PIRQD)	/* Bitmap of supported IRQs */

/* Link */
#define L_PIRQA  1		/* Means Slot INTx# Connects To Chipset INTA# */
#define L_PIRQB  2		/* Means Slot INTx# Connects To Chipset INTB# */
#define L_PIRQC  3		/* Means Slot INTx# Connects To Chipset INTC# */
#define L_PIRQD  4		/* Means Slot INTx# Connects To Chipset INTD# */


static const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE,				/* u32 signature */
	PIRQ_VERSION,				/* u16 version */
	32 + 16 * CONFIG_IRQ_SLOT_COUNT,	/* Max. number of devices on the bus */
	0x00,					/* Interrupt router bus */
	0x0f << 3,				/* Interrupt router dev */
	0,					/* IRQs devoted exclusively to PCI usage */
	0x100b,					/* Vendor */
	0x2b,					/* Device */
	0,					/* Miniport */
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },	/* u8 rfu[11] */
	0x20,					/* Checksum (has to be set to some value that
						 * would give 0 after the sum of all bytes
						 * for this structure (including checksum).
						 */
	{
		/* bus,        dev | fn,   {link, bitmap},     {link, bitmap},     {link, bitmap},     {link, bitmap},      slot, rfu */
		{0x00, (0x01 << 3) | 0x0, {{L_PIRQA, M_PIRQA}, {0x00, 0x0000},     {0x00, 0x0000},     {0x00, 0x0000}},     0x0, 0x0},	/* CPU */
		{0x00, (0x0f << 3) | 0x0, {{L_PIRQA, M_PIRQA}, {L_PIRQB, M_PIRQB}, {L_PIRQC, M_PIRQC}, {L_PIRQD, M_PIRQD}}, 0x0, 0x0},	/* chipset */
		{0x00, (0x04 << 3) | 0x0, {{L_PIRQA, M_PIRQA}, {0x00, 0x0000},     {0x00, 0x0000},     {0x00, 0x0000}},     0x0, 0x0},	/* ethernet */
	}
};

unsigned long write_pirq_routing_table(unsigned long addr)
{
	return copy_pirq_routing_table(addr, &intel_irq_routing_table);
}
