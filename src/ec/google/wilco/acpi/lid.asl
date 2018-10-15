/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
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

Device (LID)
{
	Name (_HID, EisaId ("PNP0C0D"))
	Name (_UID, 1)
	Name (_STA, 0xf)

	Method (_LID, 0, NotSerialized)
	{
		Return (R (P1LC))
	}
}
