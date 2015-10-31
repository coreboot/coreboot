/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* This is board specific information: IRQ routing for the
 * i945
 */


// PCI Interrupt Routing
Method(_PRT)
{
	If (PICM) {
		Return (Package() {
			Package() { 0x0001FFFF, 0, 0, 0x10 },
			Package() { 0x0002FFFF, 0, 0, 0x10 },
			Package() { 0x0007FFFF, 0, 0, 0x10 },
			Package() { 0x001BFFFF, 0, 0, 0x16 },
			Package() { 0x001CFFFF, 0, 0, 0x11 },
			Package() { 0x001CFFFF, 1, 0, 0x10 },
			Package() { 0x001CFFFF, 2, 0, 0x12 },
			Package() { 0x001CFFFF, 3, 0, 0x13 },
			Package() { 0x001DFFFF, 0, 0, 0x15 },
			Package() { 0x001DFFFF, 1, 0, 0x13 },
			Package() { 0x001DFFFF, 2, 0, 0x12 },
			Package() { 0x001DFFFF, 3, 0, 0x10 },
			Package() { 0x001EFFFF, 0, 0, 0x16 },
			Package() { 0x001EFFFF, 1, 0, 0x14 },
			Package() { 0x001FFFFF, 0, 0, 0x12 },
			Package() { 0x001FFFFF, 1, 0, 0x13 },
			Package() { 0x001FFFFF, 3, 0, 0x10 }
		})
	} Else {
		Return (Package() {
			Package() { 0x0001FFFF, 0, \_SB.PCI0.LPCB.LNKA, 0 },
			Package() { 0x0002FFFF, 0, \_SB.PCI0.LPCB.LNKA, 0 },
			Package() { 0x0007FFFF, 0, \_SB.PCI0.LPCB.LNKA, 0 },
			Package() { 0x001BFFFF, 0, \_SB.PCI0.LPCB.LNKG, 0 },
			Package() { 0x001CFFFF, 0, \_SB.PCI0.LPCB.LNKB, 0 },
			Package() { 0x001CFFFF, 1, \_SB.PCI0.LPCB.LNKA, 0 },
			Package() { 0x001CFFFF, 2, \_SB.PCI0.LPCB.LNKC, 0 },
			Package() { 0x001CFFFF, 3, \_SB.PCI0.LPCB.LNKD, 0 },
			Package() { 0x001DFFFF, 0, \_SB.PCI0.LPCB.LNKH, 0 },
			Package() { 0x001DFFFF, 1, \_SB.PCI0.LPCB.LNKD, 0 },
			Package() { 0x001DFFFF, 2, \_SB.PCI0.LPCB.LNKC, 0 },
			Package() { 0x001DFFFF, 3, \_SB.PCI0.LPCB.LNKA, 0 },
			Package() { 0x001EFFFF, 0, \_SB.PCI0.LPCB.LNKG, 0 },
			Package() { 0x001EFFFF, 1, \_SB.PCI0.LPCB.LNKE, 0 },
			Package() { 0x001FFFFF, 0, \_SB.PCI0.LPCB.LNKC, 0 },
			Package() { 0x001FFFFF, 1, \_SB.PCI0.LPCB.LNKD, 0 },
			Package() { 0x001FFFFF, 3, \_SB.PCI0.LPCB.LNKA, 0 }
		})
	}
}
