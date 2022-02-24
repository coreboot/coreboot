/* SPDX-License-Identifier: GPL-2.0-only */

Device (LID0)
{
	Name (_HID, EisaId ("PNP0C0D"))
	Method (_STA)
	{
		Return (0x0F)
	}
	Method (_LID, 0)
	{
		// 0x00 == Closed
		// 0x01 == Open
		Return (ECRD (RefOf (LSTE)))
	}
}
