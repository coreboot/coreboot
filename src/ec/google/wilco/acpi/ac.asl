/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
