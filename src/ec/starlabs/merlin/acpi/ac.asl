/* SPDX-License-Identifier: GPL-2.0-only */

Device (ADP1)
{
	Name (_HID, "ACPI0003")
	Method (_STA)
	{
		Return (0x0F)
	}
	Method (_PSR, 0)
	{
		PWRS = (ECRD (RefOf(ECPS)) & 0x01)
		Return (PWRS)
	}
	Method (_PCL, 0)
	{
		Return (
			Package() { _SB }
		)
	}
}
