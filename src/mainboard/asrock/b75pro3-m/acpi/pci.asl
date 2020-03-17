/*
 * This file is part of the coreboot project.
 *
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

Device (P0P1)
{
	Name (_ADR, 0x001E0000)

	Method (_PRT, 0)
	{
		If (PICM) {
			Return (Package () {
				Package () {0x0001FFFF, 0, 0, 0x14},
				Package () {0x0001FFFF, 1, 0, 0x15},
				Package () {0x0001FFFF, 2, 0, 0x16},
				Package () {0x0001FFFF, 3, 0, 0x17},

				Package () {0x0002FFFF, 0, 0, 0x17},
				Package () {0x0002FFFF, 1, 0, 0x14},
				Package () {0x0002FFFF, 2, 0, 0x15},
				Package () {0x0002FFFF, 3, 0, 0x16}
			})
		} Else {
			Return (Package () {
				Package () {0x0001FFFF, 0, \_SB.PCI0.LPCB.LNKE, 0},
				Package () {0x0001FFFF, 1, \_SB.PCI0.LPCB.LNKF, 0},
				Package () {0x0001FFFF, 2, \_SB.PCI0.LPCB.LNKG, 0},
				Package () {0x0001FFFF, 3, \_SB.PCI0.LPCB.LNKH, 0},

				Package () {0x0002FFFF, 0, \_SB.PCI0.LPCB.LNKH, 0},
				Package () {0x0002FFFF, 1, \_SB.PCI0.LPCB.LNKE, 0},
				Package () {0x0002FFFF, 2, \_SB.PCI0.LPCB.LNKF, 0},
				Package () {0x0002FFFF, 3, \_SB.PCI0.LPCB.LNKG, 0}
			})
		}
	}
}
