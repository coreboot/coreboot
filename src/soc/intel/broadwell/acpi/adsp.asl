/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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

Device (ADSP)
{
	Method (_HID, 0, Serialized)
	{
		If (\ISWP ()) {
			// WildcatPoint
			Return ("INT3438")
		}

		// LynxPoint-LP
		Return ("INT33C8")
	}
	Name (_UID, 1)
	Name (_DDN, "Intel Smart Sound Technology")

	Name (RBUF, ResourceTemplate ()
	{
		Memory32Fixed (ReadWrite, 0x00000000, 0x00100000, BAR0)
		Memory32Fixed (ReadWrite, 0x00000000, 0x00001000, BAR1)
		Interrupt (ResourceConsumer, Level, ActiveLow, Exclusive, , , ) {3}
	})

	Method (_CRS, 0, NotSerialized)
	{
		// Update BAR address and length if set in NVS
		If (LNotEqual (\S8B0, Zero)) {
			CreateDwordField (^RBUF, ^BAR0._BAS, B8A0)
			CreateDwordField (^RBUF, ^BAR1._BAS, B8A1)
			Store (\S8B0, B8A0)
			Store (\S8B1, B8A1)
		}

		Return (RBUF)
	}

	Method (_STA, 0, NotSerialized)
	{
		If (LEqual (\S8EN, 0)) {
			Return (0x0)
		} Else {
			Return (0xF)
		}
	}

	Device (I2S0)
	{
		Name (_ADR, 0)
	}

	Device (I2S1)
	{
		Name (_ADR, 1)
	}
}
