/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Lubomir Rintel <lkundrak@v3.sk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

Method (_PRT, 0) {
	If (LEqual (^ISAC.APIC, Zero)) {
		Return (Package (20) {
			/* AGP bridge */
			Package (4) { 0x0001ffff, 0, ^ISAC.LNKA, 0x00 },
			Package (4) { 0x0001ffff, 1, ^ISAC.LNKB, 0x00 },
			Package (4) { 0x0001ffff, 2, ^ISAC.LNKC, 0x00 },
			Package (4) { 0x0001ffff, 3, ^ISAC.LNKD, 0x00 },

			/* IDE interface */
			Package (4) { 0x000fffff, 0, ^ISAC.LNKA, 0x00 },
			Package (4) { 0x000fffff, 1, ^ISAC.LNKB, 0x00 },
			Package (4) { 0x000fffff, 2, ^ISAC.LNKC, 0x00 },
			Package (4) { 0x000fffff, 3, ^ISAC.LNKD, 0x00 },

			/* USB controller */
			Package (4) { 0x0010ffff, 0, ^ISAC.LNKA, 0x00 },
			Package (4) { 0x0010ffff, 1, ^ISAC.LNKB, 0x00 },
			Package (4) { 0x0010ffff, 2, ^ISAC.LNKC, 0x00 },
			Package (4) { 0x0010ffff, 3, ^ISAC.LNKD, 0x00 },

			/* Audio (& LPC bridge) */
			Package (4) { 0x0011ffff, 0, ^ISAC.LNKA, 0x00 },
			Package (4) { 0x0011ffff, 1, ^ISAC.LNKB, 0x00 },
			Package (4) { 0x0011ffff, 2, ^ISAC.LNKC, 0x00 },
			Package (4) { 0x0011ffff, 3, ^ISAC.LNKD, 0x00 },

			/* Ethernet controller */
			Package (4) { 0x0012ffff, 0, ^ISAC.LNKA, 0x00 },
			Package (4) { 0x0012ffff, 1, ^ISAC.LNKB, 0x00 },
			Package (4) { 0x0012ffff, 2, ^ISAC.LNKC, 0x00 },
			Package (4) { 0x0012ffff, 3, ^ISAC.LNKD, 0x00 }
		})
	} Else {
		/* "Table 9. APIC Fixed IRQ Routing
		 * When the internal APIC is enabled, internal IRQ
		 * routing to the APIC is fixed as follows:" */
		Return (Package (20) {
			/* The graphics controller behind the AGP bridge */
			/* INTA# => IRQ16 INTB# => IRQ17 INTC# => IRQ18 INTD# => IRQ19 */
			Package (4) { 0x0001ffff, 0, 0x00, 16 },
			Package (4) { 0x0001ffff, 1, 0x00, 17 },
			Package (4) { 0x0001ffff, 2, 0x00, 18 },
			Package (4) { 0x0001ffff, 3, 0x00, 19 },

			/* IDE (Native Mode)/SATA IRQ & INTE => IRQ20 */
			Package (4) { 0x000fffff, 0, 0x00, 20 },
			Package (4) { 0x000fffff, 1, 0x00, 20 },
			Package (4) { 0x000fffff, 2, 0x00, 20 },
			Package (4) { 0x000fffff, 3, 0x00, 20 },

			/* USB IRQ (all 5 functions) and INTF => IRQ21 */
			Package (4) { 0x0010ffff, 0, 0x00, 21 },
			Package (4) { 0x0010ffff, 1, 0x00, 21 },
			Package (4) { 0x0010ffff, 2, 0x00, 21 },
			Package (4) { 0x0010ffff, 3, 0x00, 21 },

			/* AC'97 / MC'97 IRQ and INTG => IRQ22 */
			Package (4) { 0x0011ffff, 0, 0x00, 22 },
			Package (4) { 0x0011ffff, 1, 0x00, 22 },
			Package (4) { 0x0011ffff, 2, 0x00, 22 },
			Package (4) { 0x0011ffff, 3, 0x00, 22 },

			/* LAN IRQ and INTH => IRQ23 */
			Package (4) { 0x0012ffff, 0, 0x00, 23 },
			Package (4) { 0x0012ffff, 1, 0x00, 23 },
			Package (4) { 0x0012ffff, 2, 0x00, 23 },
			Package (4) { 0x0012ffff, 3, 0x00, 23 }
		})

	}
}
