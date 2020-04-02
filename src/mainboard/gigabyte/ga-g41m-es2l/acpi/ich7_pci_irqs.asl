/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/* This is board specific information:
 * IRQ routing for the 0:1e.0 PCI bridge of the ICH7
 */

If (PICM) {
	Return (Package() {
		Package() { 0x0000ffff, 0, 0, 20},
		Package() { 0x0000ffff, 1, 0, 19},
		Package() { 0x0000ffff, 2, 0, 18},
		Package() { 0x0000ffff, 3, 0, 16},

		Package() { 0x0001ffff, 0, 0, 19},
		Package() { 0x0001ffff, 1, 0, 18},
		Package() { 0x0001ffff, 2, 0, 16},
		Package() { 0x0001ffff, 3, 0, 20},
	})
} Else {
	Return (Package() {
		Package() { 0x0000ffff, 0, \_SB.PCI0.LPCB.LNKE, 0},
		Package() { 0x0000ffff, 1, \_SB.PCI0.LPCB.LNKD, 0},
		Package() { 0x0000ffff, 2, \_SB.PCI0.LPCB.LNKC, 0},
		Package() { 0x0000ffff, 3, \_SB.PCI0.LPCB.LNKA, 0},

		Package() { 0x0001ffff, 0, \_SB.PCI0.LPCB.LNKD, 0},
		Package() { 0x0001ffff, 1, \_SB.PCI0.LPCB.LNKC, 0},
		Package() { 0x0001ffff, 2, \_SB.PCI0.LPCB.LNKA, 0},
		Package() { 0x0001ffff, 3, \_SB.PCI0.LPCB.LNKE, 0},
	})
}
