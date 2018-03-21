/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Google Inc.
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

Device (TBMC)
{
	Name (_HID, "GOOG0006")
	Name (_UID, 1)
	Name (_DDN, "Tablet Motion Control")
	Method (TBMC)
	{
		If (LEqual (^^RCTM, One)) {
			Return (0x1)
		} Else {
			Return (0x0)
		}
	}
	Method(_STA, 0)
	{
		Return (0xB)
	}
}
