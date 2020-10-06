/* SPDX-License-Identifier: GPL-2.0-only */

Device (LID0)
{
	Name (_HID, EisaId ("PNP0C0D"))
	Name (_UID, 1)
	Name (_STA, 0xf)

	Method (_LID, 0, NotSerialized)
	{
		\LIDS = R (P1LC)
		Return (\LIDS)
	}
}
