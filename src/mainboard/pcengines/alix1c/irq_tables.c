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
*/

#include <arch/pirq_routing.h>

/* Platform IRQs */
#define PIRQA 11
#define PIRQB 10
#define PIRQC 11
#define PIRQD 9

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
 * ALIX1.C interrupt wiring.
 *
 * Devices are:
 *
 * 00:01.0 Host bridge: Advanced Micro Devices [AMD] CS5536 [Geode companion] Host Bridge (rev 31)
 * 00:01.2 Entertainment encryption device: Advanced Micro Devices [AMD] Geode LX AES Security Block
 * 00:0d.0 Ethernet controller: VIA Technologies, Inc. VT6105M [Rhine-III] (rev 96)
 * 00:0e.0 Network controller: Intersil Corporation Prism 2.5 Wavelan chipset (rev 01)
 * 00:0f.0 ISA bridge: Advanced Micro Devices [AMD] CS5536 [Geode companion] ISA (rev 03)
 * 00:0f.2 IDE interface: Advanced Micro Devices [AMD] CS5536 [Geode companion] IDE (rev 01)
 * 00:0f.3 Multimedia audio controller: Advanced Micro Devices [AMD] CS5536 [Geode companion] Audio (rev 01)
 * 00:0f.4 USB Controller: Advanced Micro Devices [AMD] CS5536 [Geode companion] OHC (rev 02)
 * 00:0f.5 USB Controller: Advanced Micro Devices [AMD] CS5536 [Geode companion] EHC (rev 02)
 *
 * The only devices that interrupt are:
 *
 * What		Device  IRQ	PIN	PIN WIRED TO
 * -------------------------------------------------
 * AES		00:01.2 0a	01	A	A
 * 3VPCI	00:0c.0 0a	01	A	A
 * eth0		00:0d.0 0b	01	A	B
 * mpci		00:0e.0 0a	01	A	A
 * usb		00:0f.3 0b	02	B	B
 * usb		00:0f.4 0b	04	D	D
 * usb		00:0f.5 0b	04	D	D
 *
 * The only swizzled interrupt is eth0, where INTA is wired to interrupt controller line B.
 */

static const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE,
	PIRQ_VERSION,
	32 + 16 * CONFIG_IRQ_SLOT_COUNT,/* Max. number of devices on the bus */
	0x00,			/* Where the interrupt router lies (bus) */
	(0x0F << 3) | 0x0,	/* Where the interrupt router lies (dev) */
	0x00,			/* IRQs devoted exclusively to PCI usage */
	0x100B,			/* Vendor */
	0x002B,			/* Device */
	0,			/* Miniport data */
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/* u8 rfu[11] */
	0x00,			/* Checksum */
	{
		/* If you change the number of entries, change CONFIG_IRQ_SLOT_COUNT above! */

		/* bus, dev|fn,           {link, bitmap},      {link, bitmap},     {link, bitmap},     {link, bitmap},     slot, rfu */

		/* CPU */
		{0x00, (0x01 << 3) | 0x0, {{L_PIRQA, M_PIRQA}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}}, 0x0, 0x0},

		/* PCI (slot 1) */
		{0x00, (0x0C << 3) | 0x0, {{L_PIRQC, M_PIRQC}, {L_PIRQD, M_PIRQD}, {L_PIRQA, M_PIRQA}, {L_PIRQB, M_PIRQB}}, 0x4, 0x0},

		/* On-board ethernet */
		{0x00, (0x0D << 3) | 0x0, {{L_PIRQB, M_PIRQB}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}}, 0x0, 0x0},

		/* Mini PCI (slot 2) */
		{0x00, (0x0E << 3) | 0x0, {{L_PIRQA, M_PIRQA}, {L_PIRQB, M_PIRQB}, {L_PIRQC, M_PIRQC}, {L_PIRQD, M_PIRQD}}, 0x1, 0x0},

		/* Chipset slots -- f.3 wires to B, and f.4 and f.5 wires to D. */
		{0x00, (0x0F << 3) | 0x0, {{L_PIRQA, M_PIRQA}, {L_PIRQB, M_PIRQB}, {L_PIRQC, M_PIRQC}, {L_PIRQD, M_PIRQD}}, 0x0, 0x0},
	}
};

unsigned long write_pirq_routing_table(unsigned long addr)
{
	return copy_pirq_routing_table(addr, &intel_irq_routing_table);
}
