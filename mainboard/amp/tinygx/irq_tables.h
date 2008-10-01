/*
* This file is part of the coreboot project.
*
* Copyright (C) 2007 Advanced Micro Devices, Inc.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
*/

#include <pirq_routing.h>

/* Number of slots and devices in the PIR table */
#define SLOT_COUNT 4

/* Platform IRQs */
#define PIRQA 10
#define PIRQB 11
#define PIRQC 10
#define PIRQD 11

/* Map */
#define M_PIRQA (1 << PIRQA)	/* Bitmap of supported IRQs */
#define M_PIRQB (1 << PIRQB)	/* Bitmap of supported IRQs */
#define M_PIRQC (1 << PIRQC)	/* Bitmap of supported IRQs */
#define M_PIRQD (1 << PIRQD)	/* Bitmap of supported IRQs */

/* Link */
#define L_PIRQA 1		/* Means Slot INTx# Connects To Chipset INTA# */
#define L_PIRQB 2		/* Means Slot INTx# Connects To Chipset INTB# */
#define L_PIRQC 3		/* Means Slot INTx# Connects To Chipset INTC# */
#define L_PIRQD 4		/* Means Slot INTx# Connects To Chipset INTD# */

/*
 * AMP TinyGX interrupt wiring.
 *
 * Devices are: FIXME
 */
const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE,
	PIRQ_VERSION,
	32 + 16 * SLOT_COUNT,	/* Max. number of devices on the bus */
	0x00,			/* Where the interrupt router lies (bus) */
	(0x0F << 3) | 0x0,      /* Where the interrupt router lies (dev) */
	0x00,			/* IRQs devoted exclusively to PCI usage */
	0x100B,			/* Vendor */
	0x002B,			/* Device */
	0,			/* Crap (miniport) */
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/* u8 rfu[11] */
	0x00,			/* Checksum */
	{
		/* If you change the number of entries, change SLOT_COUNT above! */

		/* bus, dev|fn,           {link, bitmap},      {link, bitmap},     {link, bitmap},     {link, bitmap},     slot, rfu */

		/* CPU */
		{0x00, (0x01 << 3) | 0x0, {{L_PIRQA, M_PIRQA}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}}, 0x0, 0x0},

		/* Chipset slots -- f.3 wires to B, and f.4 and f.5 wires to D. */
		{0x00, (0x0F << 3) | 0x0, {{L_PIRQA, M_PIRQA}, {L_PIRQB, M_PIRQB}, {L_PIRQC, M_PIRQC}, {L_PIRQD, M_PIRQD}}, 0x0, 0x0},

		/* On-board ethernet */
		{0x00, (0x0D << 3) | 0x0, {{L_PIRQB, M_PIRQB}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}}, 0x0, 0x0},

		/* PCI (slot 1) */
		{0x00, (0x0E << 3) | 0x0, {{L_PIRQC, M_PIRQC}, {L_PIRQD, M_PIRQD}, {L_PIRQA, M_PIRQA}, {L_PIRQB, M_PIRQB}}, 0x1, 0x0},
	}
};
