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
#include <arch/io.h>
#include <southbridge/amd/cs5536/cs5536.h>

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
 * ALIX.2D3 interrupt wiring.
 *
 * Devices are:
 * 00:01.0 Host bridge [0600]: Advanced Micro Devices [AMD] CS5536 [Geode companion] Host Bridge [1022:2080] (rev 33)
 * 00:01.2 Entertainment encryption device [1010]: Advanced Micro Devices [AMD] Geode LX AES Security Block [1022:2082]
 * 00:09.0 Ethernet controller [0200]: VIA Technologies, Inc. VT6105M [Rhine-III] [1106:3053] (rev 96)
 * 00:0a.0 Ethernet controller [0200]: VIA Technologies, Inc. VT6105M [Rhine-III] [1106:3053] (rev 96)
 * 00:0b.0 Ethernet controller [0200]: VIA Technologies, Inc. VT6105M [Rhine-III] [1106:3053] (rev 96)
 * 00:0f.0 ISA bridge [0601]: Advanced Micro Devices [AMD] CS5536 [Geode companion] ISA [1022:2090] (rev 03)
 * 00:0f.2 IDE interface [0101]: Advanced Micro Devices [AMD] CS5536 [Geode companion] IDE [1022:209a] (rev 01)
 * 00:0f.4 USB Controller [0c03]: Advanced Micro Devices [AMD] CS5536 [Geode companion] OHC [1022:2094] (rev 02)
 * 00:0f.5 USB Controller [0c03]: Advanced Micro Devices [AMD] CS5536 [Geode companion] EHC [1022:2095] (rev 02)

 * The only devices that interrupt are:
 *
 * What         Device  IRQ     PIN     PIN WIRED TO
 * -------------------------------------------------
 * AES          00:01.2 0a      01      A       A
 * eth0         00:09.0 0b      01      A       B
 * eth1         00:0a.0 0b      01      A       C
 * eth2         00:0b.0 0b      01      A       D
 * mpci         00:0c.0 0a      01      A       A
 * mpci         00:0c.0 0b      02      B       B
 * usb          00:0f.4 0b      04      D       D
 * usb          00:0f.5 0b      04      D       D
 *
 * The only swizzled interrupts are the ethernet controllers, where INTA is wired to
 * interrupt controller lines B, C and D.
 */

static const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE,
	PIRQ_VERSION,
	32 + 16 * CONFIG_IRQ_SLOT_COUNT,/* Max. number of devices on the bus */
	0x00,			/* Where the interrupt router lies (bus) */
	(0x0F << 3) | 0x0,      /* Where the interrupt router lies (dev) */
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

		/* On-board ethernet (Left) */
		{0x00, (0x09 << 3) | 0x0, {{L_PIRQB, M_PIRQB}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}}, 0x0, 0x0},

		/* On-board ethernet (Middle, ALIX.2D3 only) */
		{0x00, (0x0A << 3) | 0x0, {{L_PIRQC, M_PIRQC}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}}, 0x0, 0x0},

		/* On-board ethernet (Right) */
		{0x00, (0x0B << 3) | 0x0, {{L_PIRQD, M_PIRQD}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}}, 0x0, 0x0},

		/* Mini PCI (slot 1) */
		{0x00, (0x0C << 3) | 0x0, {{L_PIRQA, M_PIRQA}, {L_PIRQB, M_PIRQB}, {0x00, 0x00}, {0x00, 0x00}}, 0x0, 0x0},

		/* Mini PCI (slot 2, ALIX.2D2 only) */
		{0x00, (0x0E << 3) | 0x0, {{L_PIRQC, M_PIRQC}, {L_PIRQD, M_PIRQD}, {0x00, 0x00}, {0x00, 0x00}}, 0x0, 0x0},

		/* Chipset slots -- f.3 wires to B, and f.4 and f.5 wires to D. */
		{0x00, (0x0F << 3) | 0x0, {{L_PIRQA, M_PIRQA}, {L_PIRQB, M_PIRQB}, {L_PIRQC, M_PIRQC}, {L_PIRQD, M_PIRQD}}, 0x0, 0x0},
	}
};

unsigned long write_pirq_routing_table(unsigned long addr)
{
	return copy_pirq_routing_table(addr, &intel_irq_routing_table);
}
