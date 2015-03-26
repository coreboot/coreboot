/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

Scope (\_SB)
{
	Device(RMOP)
	{
		Name (_HID, "GOOG9999")
		Name (_CID, "GOOG9999")
		Name (_UID, 1)

		Name (RBUF, ResourceTemplate()
		{
			Memory32Fixed (ReadWrite, 0, 0, MRES)
		})

		Method (_CRS)
		{
			CreateDwordField (^RBUF, ^MRES._BAS, RBAS)
			CreateDwordField (^RBUF, ^MRES._LEN, RLEN)
			Store (\RMOB, RBAS)
			Store (\RMOL, RLEN)
			Return (^RBUF)
		}
	}
}
