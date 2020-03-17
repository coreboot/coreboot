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

Device (PEGP)
{
	Name (_ADR, 0x00010000)

	Method (_STA)
	{
		Return (((\_SB.PCI0.MCHC.DVEN >> 3) & 1) * 0xf)
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
		Return (((\_SB.PCI0.MCHC.DVEN >> 2) & 1) * 0xf)
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
		Return (((\_SB.PCI0.MCHC.DVEN >> 1) & 1) * 0xf)
	}

	Device (DEV0)
	{
		Name(_ADR, 0x00000000)
	}
}
