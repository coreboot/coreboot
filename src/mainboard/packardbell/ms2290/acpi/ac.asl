/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2011 Sven Schnelle <svens@stackframe.org>
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

Device(AC)
{
	Name(_HID, "ACPI0003")
	Name(_UID, 0x00)
	Name(_PCL, Package() { \_SB } )

	Method(_PSR, 0, NotSerialized)
	{
		return (HPAC)
	}

	Method(_STA, 0, NotSerialized)
	{
		Return (0x0f)
	}
}

/* AC status change */
Method(_Q50, 0, NotSerialized)
{
	Notify (AC, 0x80)
}

/* AC status change */
Method(_Q51, 0, NotSerialized)
{
	Notify (AC, 0x80)
}
