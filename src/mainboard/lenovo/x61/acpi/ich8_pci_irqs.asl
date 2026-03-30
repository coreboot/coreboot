/* SPDX-License-Identifier: GPL-2.0-only */

/* This is board specific information: IRQ routing for the
 * 0:1e.0 PCI bridge of the ICH8.
 *
 * Bus 05 (secondary) holds the Ricoh multifunction device (device 0):
 *   05:00.0  Ricoh RL5c476 II CardBus bridge   (INT A)
 *   05:00.1  Ricoh R5C832 FireWire controller   (INT B)
 *   05:00.2  Ricoh R5C822 SD host adapter       (INT C)
 *
 * Additional device numbers (1, 2, 8) appear when an UltraBase dock
 * is attached.  Routing matches the vendor BIOS APRT/LPRT tables.
 */
If (PICM) {
	Return (Package() {
		// Ricoh multifunction device (device 0)
		Package() { 0x0000ffff, 0, 0, 16},
		Package() { 0x0000ffff, 1, 0, 17},
		Package() { 0x0000ffff, 2, 0, 18},
		Package() { 0x0000ffff, 3, 0, 19},

		// Dock device 1
		Package() { 0x0001ffff, 0, 0, 16},

		// Dock device 2 (two functions)
		Package() { 0x0002ffff, 0, 0, 21},
		Package() { 0x0002ffff, 1, 0, 22},

		// Dock device 8
		Package() { 0x0008ffff, 0, 0, 20},
	})
} Else {
	Return (Package() {
		// Ricoh multifunction device (device 0)
		Package() { 0x0000ffff, 0, \_SB.PCI0.LPCB.LNKA, 0},
		Package() { 0x0000ffff, 1, \_SB.PCI0.LPCB.LNKB, 0},
		Package() { 0x0000ffff, 2, \_SB.PCI0.LPCB.LNKC, 0},
		Package() { 0x0000ffff, 3, \_SB.PCI0.LPCB.LNKD, 0},

		// Dock device 1
		Package() { 0x0001ffff, 0, \_SB.PCI0.LPCB.LNKA, 0},

		// Dock device 2 (two functions)
		Package() { 0x0002ffff, 0, \_SB.PCI0.LPCB.LNKF, 0},
		Package() { 0x0002ffff, 1, \_SB.PCI0.LPCB.LNKG, 0},

		// Dock device 8
		Package() { 0x0008ffff, 0, \_SB.PCI0.LPCB.LNKE, 0},
	})
}
