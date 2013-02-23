/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Siemens AG, Inc.
 * (Written by Josef Kellermann <joseph.kellermann@heitec.de> for Siemens AG, Inc.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

/* THERMAL */
Scope(\_TZ) {
	Name (KELV, 2732)
	Name (THOT, 800)
	Name (TCRT, 850)

	ThermalZone(TZ00) {
		Method(_AC0,0) {	/* Active Cooling 0 (0=highest fan speed) */
			/* DBGO("\\_TZ\\TZ00\\_AC0\n") */
			Return(Add(0, 2730))
		}
		Method(_AL0,0) {	/* Returns package of cooling device to turn on */
			/* DBGO("\\_TZ\\TZ00\\_AL0\n") */
			Return(Package() {\_TZ.TZ00.FAN0})
		}
		Device (FAN0) {
			Name(_HID, EISAID("PNP0C0B"))
			Name(_PR0, Package() {PFN0})
		}

		PowerResource(PFN0,0,0) {
			Method(_STA) {
				Store(0xF,Local0)
				Return(Local0)
			}
			Method(_ON) {
				/* DBGO("\\_TZ\\TZ00\\FAN0 _ON\n") */
			}
			Method(_OFF) {
				/* DBGO("\\_TZ\\TZ00\\FAN0 _OFF\n") */
			}
		}

		// Processors used for active cooling
		Method (_PSL, 0, Serialized)
		{
			If (MPEN) {
				Return (Package() {\_PR.CPU0, \_PR.CPU1})
			}
			Return (Package() {\_PR.CPU0})
		}

		Method(_HOT,0) {	/* return hot temp in tenths degree Kelvin */
			/* DBGO("\\_TZ\\TZ00\\_HOT\n") */
			Return (Add (THOT, KELV))
		}
		Method(_CRT,0) {	/* return critical temp in tenths degree Kelvin */
			/* DBGO("\\_TZ\\TZ00\\_CRT\n") */
			Return (Add (TCRT, KELV))
		}
		Method(_TMP,0) {	/* return current temp of this zone */
			Store (SMBR (0x07, 0x4C,, 0x00), Local0)
			If (LGreater (Local0, 0x10)) {
				Store (Local0, Local1)
			}
			Else {
				Add (Local0, THOT, Local0)
				Return (Add (400, KELV))
			}

			Store (SMBR (0x07, 0x4C, 0x01), Local0)
			/* only the two MSBs in the external temperature low byte are used, resolution 0.25. We ignore it */
			/* Store (SMBR (0x07, 0x4C, 0x10), Local2) */
			If (LGreater (Local0, 0x10)) {
				If (LGreater (Local0, Local1)) {
					Store (Local0, Local1)
				}

				Multiply (Local1, 10, Local1)
				Return (Add (Local1, KELV))
			}
			Else {
				Add (Local0, THOT, Local0)
				Return (Add (400 , KELV))
			}
		} /* end of _TMP */
	} /* end of TZ00 */
}
