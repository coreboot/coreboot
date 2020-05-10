/* SPDX-License-Identifier: GPL-2.0-only */

//SCOPE EC0

Device (AC)
{
	Name (_HID, "ACPI0003")
	Name (_PCL, Package () { \_SB })

	Method (_PSR, 0, NotSerialized)  // _PSR: Power Source
	{
		Store ("-----> AC: _PSR", Debug)
		Store ("<----- AC: _PSR", Debug)
		Return (PWRS)
	}
}
