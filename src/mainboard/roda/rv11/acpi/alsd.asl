/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 secunet Security Networks AG
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

Device (ALSD)
{
	Name (_HID, "ACPI0008")  // _HID: Hardware ID
	Method (_STA, 0, NotSerialized)  // _STA: Status
	{
		Return (0x0F)
	}

	Method (_ALI, 0, NotSerialized)  // _ALI: Ambient Light Illuminance
	{
		Store (\_SB.PCI0.LPCB.EC0.LUXH, Local0)
		Or (ShiftLeft (Local0, 8), \_SB.PCI0.LPCB.EC0.LUXL, Local0)
		Store ("-----> _ALI: ", Debug)
		Store (Local0, Debug)
		Store (\_SB.PCI0.LPCB.EC0.LUXH, Debug)
		Store (\_SB.PCI0.LPCB.EC0.LUXL, Debug)
		Return (Local0)
	}

	Name (_ALR, Package (0x05)  // _ALR: Ambient Light Response
	{
		Package (0x02)
		{
			0x46,
			Zero
		},

		Package (0x02)
		{
			0x49,
			0x0A
		},

		Package (0x02)
		{
			0x55,
			0x50
		},

		Package (0x02)
		{
			0x64,
			0x012C
		},

		Package (0x02)
		{
			0x96,
			0x03E8
		}
	})
}
