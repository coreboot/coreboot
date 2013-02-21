/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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
 */

/* This is board specific information: IRQ routing for IvyBridge */

// PCI Interrupt Routing
Method(_PRT)
{
	If (PICM) {
		Return (Package() {
			// Onboard graphics (IGD)	0:2.0
			Package() { 0x0002ffff, 0, 0, 16 },
			// High Definition Audio	0:1b.0
			Package() { 0x001bffff, 0, 0, 22 },
			// PCIe Root Ports		0:1c.x
			Package() { 0x001cffff, 0, 0, 19 },
			Package() { 0x001cffff, 1, 0, 20 },
			Package() { 0x001cffff, 2, 0, 17 },
			Package() { 0x001cffff, 3, 0, 18 },
			// EHCI	#1			0:1d.0
			Package() { 0x001dffff, 0, 0, 19 },
			// EHCI	#2			0:1a.0
			Package() { 0x001affff, 0, 0, 20 },
			// LPC devices			0:1f.0
			Package() { 0x001fffff, 0, 0, 21 },
			Package() { 0x001fffff, 1, 0, 22 },
			Package() { 0x001fffff, 2, 0, 23 },
			Package() { 0x001fffff, 3, 0, 16 },
		})
	} Else {
		Return (Package() {
			// Onboard graphics (IGD)	0:2.0
			Package() { 0x0002ffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },
			// High Definition Audio	0:1b.0
			Package() { 0x001bffff, 0, \_SB.PCI0.LPCB.LNKG, 0 },
			// PCIe Root Ports		0:1c.x
			Package() { 0x001cffff, 0, \_SB.PCI0.LPCB.LNKD, 0 },
			Package() { 0x001cffff, 1, \_SB.PCI0.LPCB.LNKE, 0 },
			Package() { 0x001cffff, 2, \_SB.PCI0.LPCB.LNKB, 0 },
			Package() { 0x001cffff, 3, \_SB.PCI0.LPCB.LNKC, 0 },
			// EHCI	#1			0:1d.0
			Package() { 0x001dffff, 0, \_SB.PCI0.LPCB.LNKD, 0 },
			// EHCI	#2			0:1a.0
			Package() { 0x001affff, 0, \_SB.PCI0.LPCB.LNKE, 0 },
			// LPC device			0:1f.0
			Package() { 0x001fffff, 0, \_SB.PCI0.LPCB.LNKF, 0 },
			Package() { 0x001fffff, 1, \_SB.PCI0.LPCB.LNKG, 0 },
			Package() { 0x001fffff, 2, \_SB.PCI0.LPCB.LNKH, 0 },
			Package() { 0x001fffff, 3, \_SB.PCI0.LPCB.LNKA, 0 },
		})
	}
}

