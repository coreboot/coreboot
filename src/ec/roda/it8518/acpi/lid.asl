/* SPDX-License-Identifier: GPL-2.0-only */

//SCOPE EC0

Device (LID)
{
	Name (_HID, EisaId ("PNP0C0D"))  // _HID: Hardware ID
	Method (_LID, 0, NotSerialized)  // _LID: Lid Status
	{
		Printf ("-----> LID0: _LID")
		Printf ("<----- LID0: _LID")
		Return (LIDS)
	}
}
