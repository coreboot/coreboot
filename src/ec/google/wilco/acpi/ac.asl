/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

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
