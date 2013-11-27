/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Vladimir Serbinenko
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

/* This is board specific information: IRQ routing.
 */


// PCI Interrupt Routing
Method(_PRT)
{
	If (PICM) {
		Return (Package() {
			Package() { 0x0001ffff, 0, 0, 0x10 },
			Package() { 0x0002ffff, 0, 0, 0x10 }, // VGA
			Package() { 0x0003ffff, 0, 0, 0x10 },
			Package() { 0x0016ffff, 0, 0, 0x10 }, // ME
			Package() { 0x0016ffff, 1, 0, 0x11 }, // ME
			Package() { 0x0016ffff, 2, 0, 0x12 }, // ME
			Package() { 0x0016ffff, 3, 0, 0x13 }, // ME
			Package() { 0x0019ffff, 0, 0, 0x14 }, // Ethernet
			Package() { 0x001affff, 0, 0, 0x14 }, // USB
			Package() { 0x001affff, 1, 0, 0x15 }, // USB
			Package() { 0x001affff, 2, 0, 0x16 }, // USB
			Package() { 0x001affff, 3, 0, 0x17 }, // USB
			Package() { 0x001bffff, 1, 0, 0x11 }, // Audio
			Package() { 0x001cffff, 0, 0, 0x14 }, // PCI bridge
			Package() { 0x001cffff, 1, 0, 0x15 }, // PCI bridge
			Package() { 0x001cffff, 2, 0, 0x16 }, // PCI bridge
			Package() { 0x001cffff, 3, 0, 0x17 }, // PCI bridge
			Package() { 0x001dffff, 0, 0, 0x10 }, // USB
			Package() { 0x001dffff, 1, 0, 0x11 }, // USB
			Package() { 0x001dffff, 2, 0, 0x12 }, // USB
			Package() { 0x001dffff, 3, 0, 0x13 }, // USB
			Package() { 0x001fffff, 0, 0, 0x17 }, // LPC
			Package() { 0x001fffff, 1, 0, 0x10 }, // IDE
			Package() { 0x001fffff, 2, 0, 0x11 }, // SATA
			Package() { 0x001fffff, 3, 0, 0x13 }  // SMBUS
		})
	} Else {
		Return (Package() {
			Package() { 0x0001ffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },
			Package() { 0x0002ffff, 0, \_SB.PCI0.LPCB.LNKA, 0 }, // VGA
			Package() { 0x0003ffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },
			Package() { 0x0016ffff, 0, \_SB.PCI0.LPCB.LNKA, 0 }, // ME
			Package() { 0x0016ffff, 1, \_SB.PCI0.LPCB.LNKB, 0 }, // ME
			Package() { 0x0016ffff, 2, \_SB.PCI0.LPCB.LNKC, 0 }, // ME
			Package() { 0x0016ffff, 3, \_SB.PCI0.LPCB.LNKD, 0 }, // ME
			Package() { 0x0019ffff, 0, \_SB.PCI0.LPCB.LNKE, 0 }, // Ethernet
			Package() { 0x001affff, 0, \_SB.PCI0.LPCB.LNKE, 0 }, // USB
			Package() { 0x001affff, 1, \_SB.PCI0.LPCB.LNKF, 0 }, // USB
			Package() { 0x001affff, 2, \_SB.PCI0.LPCB.LNKG, 0 }, // USB
			Package() { 0x001affff, 3, \_SB.PCI0.LPCB.LNKH, 0 }, // USB
			Package() { 0x001bffff, 1, \_SB.PCI0.LPCB.LNKB, 0 }, // Audio
			Package() { 0x001cffff, 0, \_SB.PCI0.LPCB.LNKE, 0 }, // PCI
			Package() { 0x001cffff, 1, \_SB.PCI0.LPCB.LNKF, 0 }, // PCI
			Package() { 0x001cffff, 2, \_SB.PCI0.LPCB.LNKG, 0 }, // PCI
			Package() { 0x001cffff, 3, \_SB.PCI0.LPCB.LNKH, 0 }, // PCI
			Package() { 0x001dffff, 0, \_SB.PCI0.LPCB.LNKA, 0 }, // USB
			Package() { 0x001dffff, 1, \_SB.PCI0.LPCB.LNKB, 0 }, // USB
			Package() { 0x001dffff, 2, \_SB.PCI0.LPCB.LNKC, 0 }, // USB
			Package() { 0x001dffff, 3, \_SB.PCI0.LPCB.LNKD, 0 }, // USB
			Package() { 0x001fffff, 0, \_SB.PCI0.LPCB.LNKH, 0 }, // LPC
			Package() { 0x001fffff, 1, \_SB.PCI0.LPCB.LNKA, 0 }, // IDE
			Package() { 0x001fffff, 2, \_SB.PCI0.LPCB.LNKB, 0 }, // SATA
			Package() { 0x001fffff, 3, \_SB.PCI0.LPCB.LNKD, 0 }  // SMBus
		})
	}
}
