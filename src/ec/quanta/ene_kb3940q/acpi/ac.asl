/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

// Scope (EC0)

Device (AC)
{
	Name (_HID, "ACPI0003")
	Name (_PCL, Package () { \_SB })

	Method (_PSR)
	{
		Store(ADPT, Local0)
		Return(Local0)
	}

	Method (_STA)
	{
		Return (0x0F)
	}
}
