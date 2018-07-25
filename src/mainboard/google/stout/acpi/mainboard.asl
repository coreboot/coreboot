/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Google Inc.
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

Scope (\_SB)
{
	Device (LID0)
	{
		Name (_HID, EisaId("PNP0C0D"))
		Name (_PRW, Package() {0x18, 4})

		Method (_LID, 0)
		{
			Store (\_SB.PCI0.LPCB.EC0.HPLD, \LIDS)
			Return (\LIDS)
		}

		Method (_PSW, 1)
		{
			// Enable/Disable LID as a wake source
			Store (Arg0, \_SB.PCI0.LPCB.EC0.HWLO)
		}
	}
}
