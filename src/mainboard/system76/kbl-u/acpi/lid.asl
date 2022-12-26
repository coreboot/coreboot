/* SPDX-License-Identifier: GPL-2.0-only */

Device (LID0)
{
	Name (_HID, EisaId ("PNP0C0D"))
	Name (_PRW, Package () { 0x13 /* GPP_C19 */, 3 })

	Method (_LID, 0, NotSerialized) {
		DEBUG = "LID: _LID"
		If (^^PCI0.LPCB.EC0.ECOK) {
			Return (^^PCI0.LPCB.EC0.LSTE)
		} Else {
			Return (1)
		}
	}

	Method (_PSW, 1, NotSerialized) {
		DEBUG = Concatenate("LID: _PSW: ", ToHexString(Arg0))
		If (^^PCI0.LPCB.EC0.ECOK) {
			^^PCI0.LPCB.EC0.LWKE = Arg0
		}
	}
}
