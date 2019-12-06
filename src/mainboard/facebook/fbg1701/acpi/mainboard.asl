/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
 * Copyright (C) 2015 Intel Corp.
 * Copyright (C) 2018 Eltan B.V.
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
	Device (PWRB)
	{
		Name (_HID, EisaId ("PNP0C0C"))
		Name (_UID, 1)
	}
}

/*
 * Onboard CPLD
 */
Scope (\_SB.PCI0.LPCB)
{
	Device (CPLD)		/* Onboard CPLD */
	{
		Name(_HID, EISAID("PNP0C01"))
		Name(_CRS, ResourceTemplate()
		{
			/* Reserve 0x280 to 0x2BF for the CPLD */
			FixedIO (0x0280, 0x40)
			IRQNoFlags () {7}
		})
	}
}
