/* SPDX-License-Identifier: GPL-2.0-only */

/* This is board specific information: IRQ routing for the
 * 0:1e.0 PCI bridge of the ICH9
 */

If (PICM) {
	Return (Package() {
		// PCI Device 1, Ricoh R5C847 routes DBC
		Package() { 0x0001ffff, 0, 0, 19},
		Package() { 0x0001ffff, 1, 0, 17},
		Package() { 0x0001ffff, 2, 0, 18},
	})
} Else {
	Return (Package() {
		// PCI Device 1, Ricoh R5C847 routes DBC
		Package() { 0x0001ffff, 0, \_SB.PCI0.LPCB.LNKD, 0},
		Package() { 0x0001ffff, 1, \_SB.PCI0.LPCB.LNKB, 0},
		Package() { 0x0001ffff, 2, \_SB.PCI0.LPCB.LNKC, 0},
	})
}
