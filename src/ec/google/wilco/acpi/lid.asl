/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

Device (LID0)
{
	Name (_HID, EisaId ("PNP0C0D"))
	Name (_UID, 1)
	Name (_STA, 0xf)

	Method (_LID, 0, NotSerialized)
	{
		Store (R (P1LC), \LIDS)
		Return (\LIDS)
	}
}
