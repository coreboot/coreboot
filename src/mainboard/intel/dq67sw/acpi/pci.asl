/* SPDX-License-Identifier: GPL-2.0-only */

/* Intel PCI to PCI bridge 0:1e.0 */

Device (PCIB)
{
	Name (_ADR, 0x001E0000)

	/* From vendor FW: Power Resources for Wake */
	Name (_PRW, Package(){ 11, 4 })

	Method (_PRT)
	{
		If (PICM) {
			Return (Package() {
				/* PCI slot */
				Package() { 0x0000ffff, 0, 0, 0x10 },
				Package() { 0x0000ffff, 1, 0, 0x11 },
				Package() { 0x0000ffff, 2, 0, 0x12 },
				Package() { 0x0000ffff, 3, 0, 0x13 },

				/* on-board IEEE1394 controller */
				Package() { 0x0003ffff, 0, 0, 0x14 },
			})
		}
		Return (Package() {
			/* PCI slot */
			Package() { 0x0000ffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },
			Package() { 0x0000ffff, 1, \_SB.PCI0.LPCB.LNKB, 0 },
			Package() { 0x0000ffff, 2, \_SB.PCI0.LPCB.LNKC, 0 },
			Package() { 0x0000ffff, 3, \_SB.PCI0.LPCB.LNKD, 0 },

			/* on-board IEEE1394 controller */
			Package() { 0x0003ffff, 0, \_SB.PCI0.LPCB.LNKE, 0 },
		})
	}
}
