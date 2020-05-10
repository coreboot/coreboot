/* SPDX-License-Identifier: GPL-2.0-only */

//SCOPE EC0

Device (LID)
{
	Name (_HID, EisaId ("PNP0C0D"))  // _HID: Hardware ID
	Method (_LID, 0, NotSerialized)  // _LID: Lid Status
	{
		Store ("-----> LID0: _LID", Debug)
		Store ("<----- LID0: _LID", Debug)
		Return (LIDS)
	}
}
