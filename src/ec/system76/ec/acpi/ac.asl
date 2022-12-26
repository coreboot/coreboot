/* SPDX-License-Identifier: GPL-2.0-only */

Device (AC)
{
	Name (_HID, "ACPI0003" /* Power Source Device */)  // _HID: Hardware ID
	Name (_PCL, Package (0x01)  // _PCL: Power Consumer List
	{
		_SB
	})

	Name (ACFG, 1)

	Method (_PSR, 0, NotSerialized)  // _PSR: Power Source
	{
		Return (ACFG)
	}

	Method (_STA, 0, NotSerialized)  // _STA: Status
	{
		Return (0x0F)
	}
}
