/* SPDX-License-Identifier: GPL-2.0-only */

/* AC adapter, reported by EC RAM bit ACAC. */

Device (ADP1)
{
	Name (_HID, "ACPI0003")

	Method (_PSR, 0, NotSerialized)
	{
		If (ECOK) {
			Return (ACAC)
		}
		/* Assume powered before the EC region is available. */
		Return (One)
	}

	Method (_PCL, 0, NotSerialized)
	{
		Return (Package () { \_SB })
	}
}
