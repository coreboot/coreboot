/* SPDX-License-Identifier: GPL-2.0-only */

Device (LID)
{
	Name (_HID, "PNP0C0D")
	Name (_PRW, Package () { EC_GPE_LID, 3 })

	Method (_LID)
	{
		Printf ("LID: _LID: state=%o", ToDecimalString (\_SB.PCI0.LPCB.EC0.LSTE))

		Return (\_SB.PCI0.LPCB.EC0.LSTE)
	}

	Method (_PSW, 1)
	{
		Printf ("LID: _PSW: set lid wake enable=%o", ToDecimalString (Arg0))

		\_SB.PCI0.LPCB.EC0.LWKE = Arg0
	}
}
