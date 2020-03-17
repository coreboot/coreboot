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

/* SuperIO support for Windows */

Device (UAR1) {
	Name (_HID, EISAID ("PNP0501"))
	Name (_UID, 1)
	Name (_CRS, ResourceTemplate ()
	{
		IO (Decode16, 0x03F8, 0x03F8, 0x08, 0x08)
		IRQNoFlags () {4}
	})
	Name (_PRS, ResourceTemplate ()
	{
		IO (Decode16, 0x03F8, 0x03F8, 0x08, 0x08)
		IRQNoFlags () {4}
	})
}

Device (UAR2) {
	Name (_HID, EISAID ("PNP0501"))
	Name (_UID, 2)
	Name (_CRS, ResourceTemplate ()
	{
		IO (Decode16, 0x02F8, 0x02F8, 0x08, 0x08)
		IRQNoFlags () {3}
	})
	Name (_PRS, ResourceTemplate ()
	{
		IO (Decode16, 0x02F8, 0x02F8, 0x08, 0x08)
		IRQNoFlags () {3}
	})
}
