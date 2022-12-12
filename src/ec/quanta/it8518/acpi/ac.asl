/* SPDX-License-Identifier: GPL-2.0-only */

// Scope (EC0)

Device (AC)
{
	Name (_HID, "ACPI0003")
	Name (_PCL, Package () { \_SB })

	Method (_PSR)
	{
		Local0 = ACPW
		Return(Local0)
	}

	Method (_STA)
	{
		Return (0x0F)
	}
}
