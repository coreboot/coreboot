/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 System76
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

Device (AC)
{
	Name (_HID, "ACPI0003" /* Power Source Device */)  // _HID: Hardware ID
	Name (_PCL, Package (0x01)  // _PCL: Power Consumer List
	{
		_SB
	})

	Name (ACFG, One)

	Method (_PSR, 0, NotSerialized)  // _PSR: Power Source
	{
		Return (ACFG)
	}

	Method (_STA, 0, NotSerialized)  // _STA: Status
	{
		Return (0x0F)
	}
}
