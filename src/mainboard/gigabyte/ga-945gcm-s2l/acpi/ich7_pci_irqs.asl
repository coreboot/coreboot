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

/* This is board specific information: IRQ routing for the
 * 0:1e.0 PCI bridge of the ICH7
 */


If (PICM) {
	Return (Package() {
		Package (0x04) { 0x0000FFFF, 0x00, 0x00, 0x14 },
		Package (0x04) { 0x0000FFFF, 0x01, 0x00, 0x13 },
		Package (0x04) { 0x0000FFFF, 0x02, 0x00, 0x12 },
		Package (0x04) { 0x0000FFFF, 0x03, 0x00, 0x10 },
		Package (0x04) { 0x0001FFFF, 0x00, 0x00, 0x13 },
		Package (0x04) { 0x0001FFFF, 0x01, 0x00, 0x12 },
		Package (0x04) { 0x0001FFFF, 0x02, 0x00, 0x10 },
		Package (0x04) { 0x0001FFFF, 0x03, 0x00, 0x14 },

	})
} Else {
	Return (Package() {
		Package (0x04) { 0x0000FFFF, 0x00, \_SB.PCI0.LPCB.LNKE, 0x00 },
		Package (0x04) { 0x0000FFFF, 0x01, \_SB.PCI0.LPCB.LNKD, 0x00 },
		Package (0x04) { 0x0000FFFF, 0x02, \_SB.PCI0.LPCB.LNKC, 0x00 },
		Package (0x04) { 0x0000FFFF, 0x03, \_SB.PCI0.LPCB.LNKA, 0x00 },
		Package (0x04) { 0x0001FFFF, 0x00, \_SB.PCI0.LPCB.LNKD, 0x00 },
		Package (0x04) { 0x0001FFFF, 0x01, \_SB.PCI0.LPCB.LNKC, 0x00 },
		Package (0x04) { 0x0001FFFF, 0x02, \_SB.PCI0.LPCB.LNKA, 0x00 },
		Package (0x04) { 0x0001FFFF, 0x03, \_SB.PCI0.LPCB.LNKE, 0x00 },
	})
}
