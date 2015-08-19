/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015  Damien Zammit <damien@zamaudio.com>
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

/* This is board specific information: IRQ routing for x4x */

/* PCI Interrupt Routing */
Method(_PRT)
{
	If (PICM) {
		Return (Package() {
			/* Internal GFX */
			Package() { 0x0002ffff, 0, 0, 16 },
			/* High Definition Audio	0:1b.0 */
			Package() { 0x001bffff, 0, 0, 22 },
			/* PCIe Root Ports		0:1c.x */
			Package() { 0x001cffff, 0, 0, 16 },
			Package() { 0x001cffff, 1, 0, 17 },
			Package() { 0x001cffff, 2, 0, 18 },
			Package() { 0x001cffff, 3, 0, 19 },
			/* USB and EHCI			0:1d.x */
			Package() { 0x001dffff, 0, 0, 23 },
			Package() { 0x001dffff, 1, 0, 19 },
			Package() { 0x001dffff, 2, 0, 18 },
			Package() { 0x001dffff, 3, 0, 16 },
			Package() { 0x001dffff, 0, 0, 23 },
			/* SMBUS/SATA/PATA	0:1f.2, 0:1f.3 */
			Package() { 0x001fffff, 1, 0, 19 },
			Package() { 0x001fffff, 1, 0, 19 },
			Package() { 0x001fffff, 0, 0, 18 },
		})
	} Else {
		Return (Package() {
			/* Internal GFX */
			Package() { 0x0002ffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },
			/* High Definition Audio	0:1b.0 */
			Package() { 0x001bffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },
			/* PCIe Root Ports		0:1c.x */
			Package() { 0x001cffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },
			Package() { 0x001cffff, 1, \_SB.PCI0.LPCB.LNKB, 0 },
			Package() { 0x001cffff, 2, \_SB.PCI0.LPCB.LNKC, 0 },
			Package() { 0x001cffff, 3, \_SB.PCI0.LPCB.LNKD, 0 },
			/* USB and EHCI			0:1d.x */
			Package() { 0x001dffff, 0, \_SB.PCI0.LPCB.LNKH, 0 },
			Package() { 0x001dffff, 1, \_SB.PCI0.LPCB.LNKD, 0 },
			Package() { 0x001dffff, 2, \_SB.PCI0.LPCB.LNKC, 0 },
			Package() { 0x001dffff, 3, \_SB.PCI0.LPCB.LNKA, 0 },
			Package() { 0x001dffff, 0, \_SB.PCI0.LPCB.LNKH, 0 },
			/* SMBUS/SATA/PATA		0:1f.2, 0:1f.3 */
			Package() { 0x001fffff, 1, \_SB.PCI0.LPCB.LNKD, 0 },
			Package() { 0x001fffff, 1, \_SB.PCI0.LPCB.LNKD, 0 },
			Package() { 0x001fffff, 0, \_SB.PCI0.LPCB.LNKC, 0 },
		})
	}
}
