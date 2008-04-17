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
#define IRQ_SLOT_COUNT 6

/* Platform IRQs */
#define PIRQA 9
#define PIRQB 10
#define PIRQC 11
#define PIRQD 12

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
 * ALIX.2C3 interrupt wiring.
 *
 * Devices are:
 *
 * 00:01.0 Host bridge: Advanced Micro Devices [AMD] Unknown device 2080 (rev 31)
 * 00:01.2 Entertainment encryption device: Advanced Micro Devices [AMD] Geode LX AES Security Block
 * 00:09.0 Ethernet controller: VIA Technologies, Inc. VT6105M [Rhine-III] (rev 96)
 * 00:0a.0 Ethernet controller: VIA Technologies, Inc. VT6105M [Rhine-III] (rev 96)
 * 00:0b.0 Ethernet controller: VIA Technologies, Inc. VT6105M [Rhine-III] (rev 96)
 * 00:0f.0 ISA bridge: Advanced Micro Devices [AMD] CS5536 [Geode companion] ISA (rev 03)
 * 00:0f.2 IDE interface: Advanced Micro Devices [AMD] CS5536 [Geode companion] IDE (rev 01)
 * 00:0f.3 Multimedia audio controller: Advanced Micro Devices [AMD] CS5536 [Geode companion] Audio (rev 01)
 * 00:0f.4 USB Controller: Advanced Micro Devices [AMD] CS5536 [Geode companion] OHC (rev 02)
 * 00:0f.5 USB Controller: Advanced Micro Devices [AMD] CS5536 [Geode companion] EHC (rev 02)
 *
 * The only devices that interrupt are:
 
 * What         Device  IRQ     PIN     PIN WIRED TO
 * -------------------------------------------------
 * AES          00:01.2 09      01      A       A
 * eth0 	      00:09.0 09      01      A       B
 * eth1 	      00:0a.0 10      01      A       C
 * eth2 	      00:0b.0 11      01      A       D
 * minipci      00:0c.0 09      01      A       A
 * audio        00:0f.3 05      02      B       internal
 * usb (ohci)   00:0f.4 15      04      D       internal
 * usb (ehci)   00:0f.5 15      04      D       internal
 *
 */

const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE,
	PIRQ_VERSION,
	32 + 16 * IRQ_SLOT_COUNT,	/* Max. number of devices on the bus */
	0x00,			/* Where the interrupt router lies (bus) */
	(0x0F << 3) | 0x0,      /* Where the interrupt router lies (dev) */
	0x00,			/* IRQs devoted exclusively to PCI usage */
	0x100B,			/* Vendor */
	0x002B,			/* Device */
	0,			/* Crap (miniport) */
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/* u8 rfu[11] */
	0x00,			/* Checksum */
	{
		/* If you change the number of entries, change IRQ_SLOT_COUNT above! */

		/* bus, dev|fn,           {link, bitmap},      {link, bitmap},     {link, bitmap},     {link, bitmap},     slot, rfu */

		/* CPU */
		{0x00, (0x01 << 3) | 0x0, {{L_PIRQA, M_PIRQA}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}}, 0x0, 0x0},

		/* On-board eth0 */
		{0x00, (0x09 << 3) | 0x0, {{L_PIRQB, M_PIRQB}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}}, 0x0, 0x0},

		/* On-board eth1 */
		{0x00, (0x0A << 3) | 0x0, {{L_PIRQC, M_PIRQC}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}}, 0x0, 0x0},

		/* On-board eth2 */
		{0x00, (0x0B << 3) | 0x0, {{L_PIRQD, M_PIRQD}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}}, 0x0, 0x0},

		/* Mini PCI (slot 1) */
		{0x00, (0x0C << 3) | 0x0, {{L_PIRQA, M_PIRQA}, {L_PIRQB, M_PIRQB}, {L_PIRQC, M_PIRQC}, {L_PIRQD, M_PIRQD}}, 0x1, 0x0},

		/* Chipset slots */
		{0x00, (0x0F << 3) | 0x0, {{L_PIRQA, M_PIRQA}, {L_PIRQB, M_PIRQB}, {L_PIRQC, M_PIRQC}, {L_PIRQD, M_PIRQD}}, 0x0, 0x0},
	}
};
