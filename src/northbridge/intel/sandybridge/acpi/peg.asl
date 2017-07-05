/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017-2018 Patrick Rudolph <siro@das-labor.org>
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

Device (PEGP)
{
	Name (_ADR, 0x00010000)

	Method (_STA)
	{
		ShiftRight (\_SB.PCI0.MCHC.DVEN, 3, Local0)
		Return (And (Local0, 1))
	}

	Device (DEV0)
	{
		Name(_ADR, 0x00000000)
	}
}

Device (PEG1)
{
	Name (_ADR, 0x00010001)

	Method (_STA)
	{
		ShiftRight (\_SB.PCI0.MCHC.DVEN, 2, Local0)
		Return (And (Local0, 1))
	}

	Device (DEV0)
	{
		Name(_ADR, 0x00000000)
	}
}

Device (PEG2)
{
	Name (_ADR, 0x00010002)

	Method (_STA)
	{
		ShiftRight (\_SB.PCI0.MCHC.DVEN, 1, Local0)
		Return (And (Local0, 1))
	}

	Device (DEV0)
	{
		Name(_ADR, 0x00000000)
	}
}

Device (PEG6)
{
	Name (_ADR, 0x00060000)

	Method (_STA)
	{
		ShiftRight (\_SB.PCI0.MCHC.DVEN, 13, Local0)
		Return (And (Local0, 1))
	}

	Device (DEV0)
	{
		Name(_ADR, 0x00000000)
	}
}
