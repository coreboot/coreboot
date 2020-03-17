/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * This is board specific information:
 * IRQ routing for the 0:1e.0 PCI bridge of the ICH7
 */

If (PICM) {
	Return (Package() {
		Package() { 0x0000ffff, 0, 0, 0x10},
		Package() { 0x0000ffff, 1, 0, 0x11},
		Package() { 0x0000ffff, 2, 0, 0x12},
		Package() { 0x0000ffff, 3, 0, 0x13},

		Package() { 0x0001ffff, 0, 0, 0x11},
		Package() { 0x0001ffff, 1, 0, 0x12},
		Package() { 0x0001ffff, 2, 0, 0x13},
		Package() { 0x0001ffff, 3, 0, 0x10},
	})
} Else {
	Return (Package() {
		Package() { 0x0000ffff, 0, \_SB.PCI0.LPCB.LNKA, 0},
		Package() { 0x0000ffff, 1, \_SB.PCI0.LPCB.LNKB, 0},
		Package() { 0x0000ffff, 2, \_SB.PCI0.LPCB.LNKC, 0},
		Package() { 0x0000ffff, 3, \_SB.PCI0.LPCB.LNKD, 0},

		Package() { 0x0001ffff, 0, \_SB.PCI0.LPCB.LNKB, 0},
		Package() { 0x0001ffff, 1, \_SB.PCI0.LPCB.LNKC, 0},
		Package() { 0x0001ffff, 2, \_SB.PCI0.LPCB.LNKD, 0},
		Package() { 0x0001ffff, 3, \_SB.PCI0.LPCB.LNKA, 0},
	})
}
