/*
 * This file is part of the coreboot project.
 *
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

Scope (\_SB)
{
	Device (LID0)
	{
		Name (_HID, EisaId ("PNP0C0D"))

		Method (_STA)
		{
			Return (0xF)
		}

		Method (_LID)
		{
			Return (\_SB.PCI0.LPCB.EC.LIDS)
		}
	}

	Device (SLPB)
	{
		Name (_HID, EisaId ("PNP0C0E"))

		Method (_STA)
		{
			Return (0xF)
		}
	}
}
