/* SPDX-License-Identifier: GPL-2.0-only */

Device (LID)
{
	Name (_HID, EisaId ("PNP0C0D"))
	Method (_LID, 0, NotSerialized)
	{
		Local0 = CLID
		Return (Local0)
	}
}

Method (_Q0A, 0, NotSerialized)
{
	Printf ("EC: LID STATUS")
	Notify (LID, 0x80)
}
