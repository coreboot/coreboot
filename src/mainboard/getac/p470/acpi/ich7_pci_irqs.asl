/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

/* This is board specific information: IRQ routing for the
 * 0:1e.0 PCI bridge of the ICH7
 */

If (PICM) {
	Return (Package() {
		Package() { 0x0005ffff, 0, 0, 16},
		Package() { 0x0005ffff, 1, 0, 17},
		Package() { 0x0005ffff, 2, 0, 18},
		Package() { 0x0005ffff, 4, 0, 19} // ?? 4 ??
	})
} Else {
	Return (Package() {
		Package() { 0x0005ffff, 0, \_SB.PCI0.LPCB.LNKA, 0},
		Package() { 0x0005ffff, 1, \_SB.PCI0.LPCB.LNKB, 0},
		Package() { 0x0005ffff, 2, \_SB.PCI0.LPCB.LNKC, 0},
		Package() { 0x0005ffff, 4, \_SB.PCI0.LPCB.LNKD, 0}, // Really 4??
	})
}

