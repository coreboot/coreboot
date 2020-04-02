/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/*
 * This is board specific information:
 * IRQ routing for the 0:1e.0 PCI bridge of the ICH7
 */

If (PICM) {
	Return (Package() {
		/* PCI1 SLOT 1 */
		Package() { 0x0000ffff, 0, 0, 0x13},
		Package() { 0x0000ffff, 1, 0, 0x10},
		Package() { 0x0000ffff, 2, 0, 0x11},
		Package() { 0x0000ffff, 3, 0, 0x12},

		/* PCI1 SLOT 2 */
		Package() { 0x0001ffff, 0, 0, 0x10},
		Package() { 0x0001ffff, 1, 0, 0x11},
		Package() { 0x0001ffff, 2, 0, 0x12},
		Package() { 0x0001ffff, 3, 0, 0x13},
	})
} Else {
	Return (Package() {
		Package() { 0x0000ffff, 0, \_SB.PCI0.LPCB.LNKD, 0},
		Package() { 0x0000ffff, 1, \_SB.PCI0.LPCB.LNKA, 0},
		Package() { 0x0000ffff, 2, \_SB.PCI0.LPCB.LNKB, 0},
		Package() { 0x0000ffff, 3, \_SB.PCI0.LPCB.LNKC, 0},

		Package() { 0x0001ffff, 0, \_SB.PCI0.LPCB.LNKA, 0},
		Package() { 0x0001ffff, 1, \_SB.PCI0.LPCB.LNKB, 0},
		Package() { 0x0001ffff, 2, \_SB.PCI0.LPCB.LNKC, 0},
		Package() { 0x0001ffff, 3, \_SB.PCI0.LPCB.LNKD, 0},
	})
}
