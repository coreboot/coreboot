/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Arthur Heymans <arthur@aheymans.xyz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* This is board specific information: IRQ routing for x4x */
/* Uses reset defaults + some undocumented device taken from vendor DSDT*/

/* PCI Interrupt Routing */
Method(_PRT)
{
	If (PICM) {
		Return (Package() {
			/* PEG */
			Package() { 0x0001ffff, 0, 0, 0x10 },
			/* Internal GFX */
			Package() { 0x0002ffff, 0, 0, 0x10 },
			/* ME */
			Package() { 0x0003ffff, 0, 0, 0x10 },
			Package() { 0x0003ffff, 1, 0, 0x11 },
			Package() { 0x0003ffff, 2, 0, 0x12 },
			/* ?? */
			Package() { 0x0016ffff, 0, 0, 0x12 },
			Package() { 0x0016ffff, 1, 0, 0x13 },
			/* GBE			        0:19.0 */
			Package() { 0x0019ffff, 0, 0, 0x10 },
			/* USB and EHCI */
			Package() { 0x001affff, 0, 0, 0x10 },
			Package() { 0x001affff, 1, 0, 0x11 },
			Package() { 0x001affff, 2, 0, 0x12 },
			/* High Definition Audio	0:1b.0 */
			Package() { 0x001bffff, 0, 0, 0x10 },
			/* PCIe Root Ports		0:1c.x */
			Package() { 0x001cffff, 0, 0, 0x10 },
			Package() { 0x001cffff, 1, 0, 0x11 },
			Package() { 0x001cffff, 2, 0, 0x12 },
			Package() { 0x001cffff, 3, 0, 0x13 },
			/* USB and EHCI			0:1d.x */
			Package() { 0x001dffff, 0, 0, 0x10 },
			Package() { 0x001dffff, 1, 0, 0x11 },
			Package() { 0x001dffff, 2, 0, 0x12 },
			Package() { 0x001dffff, 3, 0, 0x13 },
			/* SMBUS/SATA/PATA	0:1f.2, 0:1f.3 */
			Package() { 0x001fffff, 1, 0, 0x11 },
			Package() { 0x001fffff, 2, 0, 0x12 },
			Package() { 0x001fffff, 3, 0, 0x13 },
		})
	} Else {
		Return (Package() {
			/* PEG */
			Package() { 0x0001ffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },
			/* Internal GFX */
			Package() { 0x0002ffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },
			/* ME */
			Package() { 0x0003ffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },
			Package() { 0x0003ffff, 1, \_SB.PCI0.LPCB.LNKB, 0 },
			Package() { 0x0003ffff, 2, \_SB.PCI0.LPCB.LNKC, 0 },

			Package() { 0x0016ffff, 0, \_SB.PCI0.LPCB.LNKC, 0 },
			Package() { 0x0016ffff, 1, \_SB.PCI0.LPCB.LNKD, 0 },
			/* GBE */
			Package() { 0x0019ffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },
			/* USB */
			Package() { 0x001affff, 0, \_SB.PCI0.LPCB.LNKA, 0 },
			Package() { 0x001affff, 1, \_SB.PCI0.LPCB.LNKB, 0 },
			Package() { 0x001affff, 2, \_SB.PCI0.LPCB.LNKC, 0 },
			/* High Definition Audio	0:1b.0 */
			Package() { 0x001bffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },
			/* PCIe Root Ports		0:1c.x */
			Package() { 0x001cffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },
			Package() { 0x001cffff, 1, \_SB.PCI0.LPCB.LNKB, 0 },
			Package() { 0x001cffff, 2, \_SB.PCI0.LPCB.LNKC, 0 },
			Package() { 0x001cffff, 3, \_SB.PCI0.LPCB.LNKD, 0 },
			/* USB and EHCI			0:1d.x */
			Package() { 0x001dffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },
			Package() { 0x001dffff, 1, \_SB.PCI0.LPCB.LNKB, 0 },
			Package() { 0x001dffff, 2, \_SB.PCI0.LPCB.LNKC, 0 },
			Package() { 0x001dffff, 3, \_SB.PCI0.LPCB.LNKD, 0 },
			/* PATA/SATA/SMBUS		0:1f.1-3 */
			Package() { 0x001fffff, 1, \_SB.PCI0.LPCB.LNKB, 0 },
			Package() { 0x001fffff, 2, \_SB.PCI0.LPCB.LNKC, 0 },
			Package() { 0x001fffff, 3, \_SB.PCI0.LPCB.LNKD, 0 },
		})
	}
}
