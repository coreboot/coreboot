/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/* This is board specific information: IRQ routing for the
 * 0:1e.0 PCI bridge of the ICH7
 */

If (PICM) {
	Return (Package() {
		Package() { 0x0005ffff, 0, 0, 16},
		Package() { 0x0005ffff, 1, 0, 17},
		Package() { 0x0005ffff, 2, 0, 18},
		Package() { 0x0005ffff, 4, 0, 19} // ?? 4 ??
	})
} Else {
	Return (Package() {
		Package() { 0x0005ffff, 0, \_SB.PCI0.LPCB.LNKA, 0},
		Package() { 0x0005ffff, 1, \_SB.PCI0.LPCB.LNKB, 0},
		Package() { 0x0005ffff, 2, \_SB.PCI0.LPCB.LNKC, 0},
		Package() { 0x0005ffff, 4, \_SB.PCI0.LPCB.LNKD, 0}, // Really 4??
	})
}
