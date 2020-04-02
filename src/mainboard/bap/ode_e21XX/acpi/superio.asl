/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
