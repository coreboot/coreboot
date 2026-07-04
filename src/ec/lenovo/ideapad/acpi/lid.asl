/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Lid switch. The EC raises a query on lid open/close (see _Q0C/_Q0D);
 * those handlers call LIDW() to update the tracked state and notify the OS.
 */

/* Lid state, 1 = open, 0 = closed; initialised to open. */
Name (LIDS, One)

Device (LID0)
{
	Name (_HID, EisaId ("PNP0C0D"))

	Method (_LID, 0, NotSerialized)
	{
		Return (LIDS)
	}
}

Method (LIDW, 1, NotSerialized)
{
	LIDS = Arg0
	Notify (LID0, 0x80)
}
