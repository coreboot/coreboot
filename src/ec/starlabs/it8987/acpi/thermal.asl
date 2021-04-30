/* SPDX-License-Identifier: GPL-2.0-only */

Method(_QF0)				// Thermal event.
{
	If (LEqual (DBGS, 0x00))
	{
		/* Only handle the numerous thermal events if we are */
		/* NOT doing ACPI Debugging. */
		If (CondRefOf (\_TZ.TZ01))
		{
			Notify (\_TZ.TZ01, 0x80)
		}
	}
}
