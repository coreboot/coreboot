/* SPDX-License-Identifier: GPL-2.0-only */

Device (AC)
{
	Name (_HID, "ACPI0003")
	Name (_UID, 1)
	Name (_PCL, Package () { \_SB, BAT0, BAT1 })

	Method (_STA)
	{
		Return (0xf)
	}

	Method (_PSR)
	{
		Return (R (ACEX))
	}
}
