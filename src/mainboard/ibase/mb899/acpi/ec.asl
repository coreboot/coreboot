/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

Device(EC0)
{
	Name (_HID, EISAID("PNP0C09"))
	Name (_UID, 1)

	// _REG method requires that an operation region is defined.
	OperationRegion (ERAM, EmbeddedControl, 0x00, 0xff)
	Field (ERAM, ByteAcc, Lock, Preserve) {}

	Name (ECMD, ResourceTemplate()
	{
		IO (Decode16, 0x62, 0x62, 0, 1)
		IO (Decode16, 0x66, 0x66, 0, 1)
	})

	Method (_CRS, 0, NotSerialized)
	{
		Return (ECMD)
	}

	Method (_REG, 2)
	{
		// This method is needed by Windows XP/2000
		// for EC initialization before a driver
		// is loaded
	}

	Name (_GPE, 23)	// GPI07 / GPE23 -> Runtime SCI

	// TODO EC Query methods

	// TODO Scope _SB devices for AC power, LID, Power button

}
