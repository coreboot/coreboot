/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2015 Damien Zammit <damien@zamaudio.com>
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

Device (PEGP)
{
	Name (_ADR, 0x00010000)

	// PCI Interrupt Routing.
	Method (_PRT)
	{
		If (PICM) {
			Return (Package() {
				Package() { 0x0001ffff, 0, 0, 16 },
				Package() { 0x0001ffff, 1, 0, 17 },
				Package() { 0x0001ffff, 2, 0, 18 },
				Package() { 0x0001ffff, 3, 0, 19 },
			})
		} Else {
			Return (Package() {
				Package() { 0x0001ffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },
				Package() { 0x0001ffff, 1, \_SB.PCI0.LPCB.LNKB, 0 },
				Package() { 0x0001ffff, 2, \_SB.PCI0.LPCB.LNKC, 0 },
				Package() { 0x0001ffff, 3, \_SB.PCI0.LPCB.LNKD, 0 },
			})
		}

	}
}
