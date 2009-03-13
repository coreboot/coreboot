/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

// Intel PCI to PCI bridge 0:1e.0

Device (PCIB)
{
	Name (_ADR, 0x001e0000)

	Device (SLT1)
	{
		Name (_ADR, 0x00000000)
		Name (_PRW, Package(){ 11, 4 })
	}

	Device (SLT2)
	{
		Name (_ADR, 0x00010000)
		Name (_PRW, Package(){ 11, 4 })
	}

	Device (SLT3)
	{
		Name (_ADR, 0x00020000)
		Name (_PRW, Package(){ 11, 4 })
	}

	// TODO: How many slots, where?

	// PCI Interrupt Routing.
	// If PICM is set, interrupts are routed over the i8259, otherwise 
	// over the IOAPIC. (Really? If they're above 15 they need to be routed
	// fixed over the IOAPIC?)

	Method (_PRT)
	{
		If (PICM) {
			Return (Package() {
				// PCI Slot 1 routes FGHE
				Package() { 0x0000ffff, 0, 0, 16}, /* Firewire */
				Package() { 0x0000ffff, 1, 0, 22},
				Package() { 0x0000ffff, 2, 0, 23},
				Package() { 0x0000ffff, 3, 0, 20},

				// PCI Slot 2 routes GFEH (but is EFGH now, because that actually works)
				Package() { 0x0001ffff, 0, 0, 20},
				Package() { 0x0001ffff, 1, 0, 21},
				Package() { 0x0001ffff, 2, 0, 22},
				Package() { 0x0001ffff, 3, 0, 23},

				// PCI Slot 3 routes CDBA
				Package() { 0x0002ffff, 0, 0, 18},
				Package() { 0x0002ffff, 1, 0, 19},
				Package() { 0x0002ffff, 2, 0, 17},
				Package() { 0x0002ffff, 3, 0, 16}
			})
		} Else {
			Return (Package() {
				// PCI Slot 1 routes FGHE
				Package() { 0x0000ffff, 0, \_SB.PCI0.LPCB.LNKF, 0},
				Package() { 0x0000ffff, 1, \_SB.PCI0.LPCB.LNKE, 0},
				Package() { 0x0000ffff, 2, \_SB.PCI0.LPCB.LNKH, 0},
				Package() { 0x0000ffff, 3, \_SB.PCI0.LPCB.LNKG, 0},

				// PCI Slot 2 routes GFEH
				Package() { 0x0001ffff, 0, \_SB.PCI0.LPCB.LNKG, 0},
				Package() { 0x0001ffff, 1, \_SB.PCI0.LPCB.LNKF, 0},
				Package() { 0x0001ffff, 2, \_SB.PCI0.LPCB.LNKE, 0},
				Package() { 0x0001ffff, 3, \_SB.PCI0.LPCB.LNKH, 0},

				// PCI Slot 3 routes CDBA
				Package() { 0x0002ffff, 0, \_SB.PCI0.LPCB.LNKC, 0},
				Package() { 0x0002ffff, 1, \_SB.PCI0.LPCB.LNKD, 0},
				Package() { 0x0002ffff, 2, \_SB.PCI0.LPCB.LNKB, 0},
				Package() { 0x0002ffff, 3, \_SB.PCI0.LPCB.LNKA, 0},
			})
		}
	}

}

