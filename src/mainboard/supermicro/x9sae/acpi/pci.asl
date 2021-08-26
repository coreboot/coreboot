/* SPDX-License-Identifier: GPL-2.0-only */

// Intel PCI to PCI bridge 0:1e.0

Device (PCIB)
{
	Name (_ADR, 0x001E0000)  // _ADR: Address
	Name (_PRW, Package(){ 13, 4 }) // Power Resources for Wake

	Method (_PRT)  // _PRT: PCI Interrupt Routing Table
	{
		If (PICM) {
			Return (Package() {
				Package() { 0x0001ffff, 0, 0, 0x16 },
				Package() { 0x0001ffff, 1, 0, 0x15 },
				Package() { 0x0001ffff, 2, 0, 0x14 },
				Package() { 0x0001ffff, 3, 0, 0x13 },
				Package() { 0x0002ffff, 0, 0, 0x12 },
				Package() { 0x0002ffff, 1, 0, 0x13 },
				Package() { 0x0002ffff, 2, 0, 0x11 },
				Package() { 0x0002ffff, 3, 0, 0x10 },
				Package() { 0x0003ffff, 0, 0, 0x13 },
				Package() { 0x0003ffff, 1, 0, 0x12 },
				Package() { 0x0003ffff, 2, 0, 0x15 },
				Package() { 0x0003ffff, 3, 0, 0x16 },
				Package() { 0x0000ffff, 0, 0, 0x10 },
				Package() { 0x0000ffff, 1, 0, 0x11 },
				Package() { 0x0000ffff, 2, 0, 0x12 },
				Package() { 0x0000ffff, 3, 0, 0x13 },
			})
		}
		Return (Package() {
			Package() { 0x0001ffff, 0, \_SB.PCI0.LPCB.LNKG, 0 },
			Package() { 0x0001ffff, 1, \_SB.PCI0.LPCB.LNKF, 0 },
			Package() { 0x0001ffff, 2, \_SB.PCI0.LPCB.LNKE, 0 },
			Package() { 0x0001ffff, 3, \_SB.PCI0.LPCB.LNKH, 0 },
			Package() { 0x0002ffff, 0, \_SB.PCI0.LPCB.LNKC, 0 },
			Package() { 0x0002ffff, 1, \_SB.PCI0.LPCB.LNKD, 0 },
			Package() { 0x0002ffff, 2, \_SB.PCI0.LPCB.LNKB, 0 },
			Package() { 0x0002ffff, 3, \_SB.PCI0.LPCB.LNKA, 0 },
			Package() { 0x0003ffff, 0, \_SB.PCI0.LPCB.LNKD, 0 },
			Package() { 0x0003ffff, 1, \_SB.PCI0.LPCB.LNKC, 0 },
			Package() { 0x0003ffff, 2, \_SB.PCI0.LPCB.LNKF, 0 },
			Package() { 0x0003ffff, 3, \_SB.PCI0.LPCB.LNKG, 0 },
			Package() { 0x0000ffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },
			Package() { 0x0000ffff, 1, \_SB.PCI0.LPCB.LNKB, 0 },
			Package() { 0x0000ffff, 2, \_SB.PCI0.LPCB.LNKC, 0 },
			Package() { 0x0000ffff, 3, \_SB.PCI0.LPCB.LNKD, 0 },
		})
	}
}
