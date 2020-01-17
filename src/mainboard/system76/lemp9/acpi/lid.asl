/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 System76
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

Device (LID0)
{
	Name (_HID, EisaId ("PNP0C0D"))
	Name (_PRW, Package () { 0x29 /* GPP_D9 */, 3 })

	Method (_LID, 0, NotSerialized) {
		DEBUG = "LID: _LID"
		If (^^PCI0.LPCB.EC0.ECOK) {
			Return (^^PCI0.LPCB.EC0.LSTE)
		} Else {
			Return (One)
		}
	}

	Method (_PSW, 1, NotSerialized) {
		DEBUG = Concatenate("LID: _PSW: ", ToHexString(Arg0))
		If (^^PCI0.LPCB.EC0.ECOK) {
			^^PCI0.LPCB.EC0.LWKE = Arg0
		}
	}
}
