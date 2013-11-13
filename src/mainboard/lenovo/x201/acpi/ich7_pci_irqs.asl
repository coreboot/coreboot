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
		Package (0x04) { 0x0000FFFF, 0x00, 0x00, 0x10 },
		Package (0x04) { 0x0000FFFF, 0x01, 0x00, 0x11 },
		Package (0x04) { 0x0000FFFF, 0x02, 0x00, 0x12 },
		Package (0x04) { 0x0001FFFF, 0x00, 0x00, 0x10 },
		Package (0x04) { 0x0002FFFF, 0x00, 0x00, 0x15 },
		Package (0x04) { 0x0002FFFF, 0x01, 0x00, 0x16 },
		Package (0x04) { 0x0008FFFF, 0x00, 0x00, 0x14 }
	})
 } Else {
	Return (Package() {
		Package (0x04) { 0x0000FFFF, 0x00, \_SB.PCI0.LPCB.LNKA, 0x00 },
		Package (0x04) { 0x0000FFFF, 0x01, \_SB.PCI0.LPCB.LNKB, 0x00 },
		Package (0x04) { 0x0000FFFF, 0x02, \_SB.PCI0.LPCB.LNKC, 0x00 },
		Package (0x04) { 0x0001FFFF, 0x00, \_SB.PCI0.LPCB.LNKA, 0x00 },
		Package (0x04) { 0x0002FFFF, 0x00, \_SB.PCI0.LPCB.LNKF, 0x00 },
		Package (0x04) { 0x0002FFFF, 0x01, \_SB.PCI0.LPCB.LNKG, 0x00 },
		Package (0x04) { 0x0008FFFF, 0x00, \_SB.PCI0.LPCB.LNKE, 0x00 }
	})
}
