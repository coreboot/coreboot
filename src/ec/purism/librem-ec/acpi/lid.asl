/* SPDX-License-Identifier: GPL-2.0-only */

Device (LID0)
{
	Name (_HID, EisaId ("PNP0C0D"))
	Name (_PRW, Package () { EC_GPE_SWI, 3 })

	Method (_LID, 0, NotSerialized) {
		Printf ("LID: _LID")
		If (^^PCI0.LPCB.EC0.ECOK) {
			Return (^^PCI0.LPCB.EC0.LSTE)
		} Else {
			Return (One)
		}
	}

	Method (_PSW, 1, NotSerialized) {
		Printf ("LID: _PSW: %o", ToHexString(Arg0))
		If (^^PCI0.LPCB.EC0.ECOK) {
			^^PCI0.LPCB.EC0.LWKE = Arg0
		}
	}
}
