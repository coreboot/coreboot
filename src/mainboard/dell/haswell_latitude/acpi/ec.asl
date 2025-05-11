/* SPDX-License-Identifier: GPL-2.0-only */

/* FIXME: not working yet */
Device(EC)
{
	Name (_HID, EISAID("PNP0C09"))
	Name (_UID, 0)
	Name (_GPE, 39)

	Method (_Q66, 0, NotSerialized)
	{
		Store ("EC: _Q66", Debug)
	}
}
