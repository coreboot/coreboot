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

		Method (_LID)
		{
			if (\_SB.PCI0.LPCB.EC.LIDS > 1)
			{
				Return (One)
			}
			else
			{
				Return (Zero)
			}
		}

		Method (_STA)
		{
			Return (_LID)
		}
	}

	Device (PWRB)
	{
		Name (_HID, EisaId ("PNP0C0C"))

		Method (_STA)
		{
			Return (0xF)
		}

		Name (_PRW, Package () { 27, 4 })
	}

}
