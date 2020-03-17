/*
 * This file is part of the coreboot project.
 *
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

/* Intel PCH HDA */

// Intel High Definition Audio (Azalia) 0:1b.0

Device (HDEF)
{
	Name (_ADR, 0x001b0000)

	Name (PRWH, Package(){ 0x0d, 3 }) // LPT-H
	Name (PRWL, Package(){ 0x6d, 3 }) // LPT-LP

	Method (_PRW, 0) { // Power Resources for Wake
		If (\ISLP ()) {
			Return (PRWL)
		} Else {
			Return (PRWH)
		}
	}
}
