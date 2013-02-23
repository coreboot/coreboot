/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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
 */

/*
DefinitionBlock ("DSDT.AML","DSDT",0x01,"XXXXXX","XXXXXXXX",0x00010001
		)
	{
		#include "routing.asl"
	}
*/

/* Routing is in System Bus scope */
Scope(\_SB) {
	Name(PR0, Package(){
		/* NB devices */
		/* Bus 0, Dev 0 - SR5650 HT */
		Package() { 0xFFFF, Zero, INTA, Zero },

		/* Bus 0, Dev 1 - CLKCONFIG */

		/* Bus 0, Dev 2 - PCIe Bridge for x16 PCIe Slot */
		Package() {0x0002FFFF, 0, INTE, 0 },

		/* Bus 0, Dev 3 - PCIe graphics port 1 bridge */

		/* Bus 0, Dev 4 - PCIe Bridge for Express Card Slot */
		Package() {0x0004FFFF, 0, INTE, 0 },

		/* Bus 0, Dev 5 - General purpose PCIe bridge 5 */

		/* Bus 0, Dev 6 - PCIe Bridge for Ethernet Chip */

		/* Bus 0, Dev 7 - PCIe Bridge for x1 PCIe Slot */

		/* Bus 0, Dev 8 - Southbridge port (normally hidden) */

		/* Bus 0, Dev 9 - PCIe Bridge */

		/* Bus 0, Dev a - PCIe Bridge */

		/* Bus 0, Dev b - PCIe Bridge */
		Package() {0x000BFFFF, 0, INTG, 0 },

		/* Bus 0, Dev c - PCIe Bridge */
		Package() {0x000CFFFF, 0, INTG, 0 },

		/* Bus 0, Dev d - PCIe Bridge for Intel 82576 Giga NIC*/
		Package() {0x000DFFFF, 0, INTG, 0 },

		/* SB devices */
		/* Bus 0, Dev 17 - SATA controller */
		Package() {0x0011FFFF, 0, INTG, 0 },

		/* Bus 0, Dev 19 - USB: OHCI, dev 18,19 func 0-2, dev 20 func 5;
		 * EHCI, dev 18, 19 func 2 */
		Package() {0x0012FFFF, 0, INTA, 0 },
		Package() {0x0012FFFF, 1, INTB, 0 },
		Package() {0x0012FFFF, 2, INTC, 0 },
		Package() {0x0012FFFF, 3, INTD, 0 },

		Package() {0x0013FFFF, 0, INTC, 0 },
		Package() {0x0013FFFF, 1, INTD, 0 },
		Package() {0x0013FFFF, 2, INTA, 0 },
		Package() {0x0013FFFF, 2, INTB, 0 },

		/* Bus 0, Dev 20 - F0:SMBus/ACPI,F1:IDE;F2:HDAudio;F3:LPC;F4:PCIBridge;F5:USB */
		Package(){0x0014FFFF, 0, INTA, 0 },
		Package(){0x0014FFFF, 1, INTB, 0 },
		Package(){0x0014FFFF, 2, INTC, 0 },
		Package(){0x0014FFFF, 3, INTD, 0 },
	})

	Name(APR0, Package(){
		/* NB devices in APIC mode */
		/* Bus 0, Dev 0 - SR5650 HT */
		Package() { 0xFFFF, Zero, Zero, 55 },

		/* Bus 0, Dev 1 - CLKCONFIG */

		/* Bus 0, Dev 2 - PCIe Bridge for x16 PCIe Slot (GFX0) */
		Package() {0x0002FFFF, 0, 0, 0x34 },

		/* Bus 0, Dev 3 - PCIe graphics port 1 bridge */

		/* Bus 0, Dev 4 - PCIe Bridge for Express Card Slot */
		Package() {0x0004FFFF, 0, 0, 0x34 },

		/* Bus 0, Dev 5 - General purpose PCIe bridge 5 */

		/* Bus 0, Dev 6 - General purpose PCIe bridge 6 */

		/* Bus 0, Dev 7 - PCIe Bridge */

		/* Bus 0, Dev 8 - Southbridge port (normally hidden) */

		/* Bus 0, Dev 9 - PCIe Bridge */

		/* Bus 0, Dev A - PCIe Bridge */

		/* Bus 0, Dev B - PCIe Bridge */
		Package() {0x000BFFFF, 0, 0, 0x36 },

		/* Bus 0, Dev C - PCIe Bridge */
		Package() {0x000CFFFF, 0, 0, 0x36 },

		/* Bus 0, Dev D - PCIe Bridge For Intel 82576 Giga NIC*/
		Package() {0x000DFFFF, 0, 0, 0x36 },

		/* SB devices in APIC mode */
		/* Bus 0, Dev 17 - SATA controller */
		Package() {0x0011FFFF, 0, 0, 0x16 },

		/* Bus 0, Dev 19 - USB: OHCI, dev 18,19 func 0-2, dev 20 func 5;
		 * EHCI, dev 18, 19 func 2 */
		Package( ){0x0012FFFF, 0, 0, 16 },
		Package() {0x0012FFFF, 1, 0, 17 },
		Package() {0x0012FFFF, 2, 0, 18 },
		Package() {0x0012FFFF, 3, 0, 19 },

		Package() {0x0013FFFF, 0, 0, 18 },
		Package() {0x0013FFFF, 1, 0, 19 },
		Package() {0x0013FFFF, 2, 0, 16 },
		Package() {0x0013FFFF, 3, 0, 17 },

		/* Bus 0, Dev 20 - F0:SMBus/ACPI, F1:IDE; F2:HDAudio; F3:LPC; F4:PCIBridge; F5:USB */
		Package() {0x0014FFFF, 0, 0, 16 },
		Package() {0x0014FFFF, 1, 0, 17 },
		Package() {0x0014FFFF, 2, 0, 18 },
		Package() {0x0014FFFF, 3, 0, 19 },
	})

	Name(PS2, Package(){
		/* The external GFX - Hooked to PCIe slot 4 */
		Package() {0x0000FFFF, 0, INTC, 0 },
		Package() {0x0000FFFF, 1, INTD, 0 },
		Package() {0x0000FFFF, 2, INTA, 0 },
		Package() {0x0000FFFF, 3, INTB, 0 },
	})
	Name(APS2, Package(){
		/* The external GFX - Hooked to PCIe slot 4 */
		Package(){0x0000FFFF, 0, 0, 0x18 },
		Package(){0x0000FFFF, 1, 0, 0x19 },
		Package(){0x0000FFFF, 2, 0, 0x1A },
		Package(){0x0000FFFF, 3, 0, 0x1B },
	})

	Name(PS4, Package(){
		/* PCIe slot - Hooked to PCIe slot 4 */
		Package(){0x0000FFFF, 0, INTA, 0 },
		Package(){0x0000FFFF, 1, INTB, 0 },
		Package(){0x0000FFFF, 2, INTC, 0 },
		Package(){0x0000FFFF, 3, INTD, 0 },
	})
	Name(APS4, Package(){
		/* PCIe slot - Hooked to PCIe slot 4 */
		Package(){0x0000FFFF, 0, 0, 0x2C },
		Package(){0x0000FFFF, 1, 0, 0x2D },
		Package(){0x0000FFFF, 2, 0, 0x2E },
		Package(){0x0000FFFF, 3, 0, 0x2F },
	})

	Name(PSb, Package(){
		/* PCIe slot - Hooked to PCIe slot 11 */
		Package(){0x0000FFFF, 0, INTD, 0 },
		Package(){0x0000FFFF, 1, INTA, 0 },
		Package(){0x0000FFFF, 2, INTB, 0 },
		Package(){0x0000FFFF, 3, INTC, 0 },
	})
	Name(APSb, Package(){
		/* PCIe slot - Hooked to PCIe */
		Package(){0x0000FFFF, 0, 0, 0x20 },
		Package(){0x0000FFFF, 1, 0, 0x21 },
		Package(){0x0000FFFF, 2, 0, 0x22 },
		Package(){0x0000FFFF, 3, 0, 0x23 },
	})

	Name(PSc, Package(){
		/* PCIe slot - Hooked to PCIe slot 12 */
		Package(){0x0000FFFF, 0, INTA, 0 },
		Package(){0x0000FFFF, 1, INTB, 0 },
		Package(){0x0000FFFF, 2, INTC, 0 },
		Package(){0x0000FFFF, 3, INTD, 0 },
	})
	Name(APSc, Package(){
		/* PCIe slot - Hooked to PCIe */
		Package(){0x0000FFFF, 0, 0, 0x24 },
		Package(){0x0000FFFF, 1, 0, 0x25 },
		Package(){0x0000FFFF, 2, 0, 0x26 },
		Package(){0x0000FFFF, 3, 0, 0x27 },
	})

	Name(PSd, Package(){
		/* PCIe slot - Hooked to PCIe slot 13 */
		Package(){0x0000FFFF, 0, INTB, 0 },
		Package(){0x0000FFFF, 1, INTC, 0 },
		Package(){0x0000FFFF, 2, INTD, 0 },
		Package(){0x0000FFFF, 3, INTA, 0 },
	})
	Name(APSd, Package(){
		/* PCIe slot - Hooked to PCIe */
		Package(){0x0000FFFF, 0, 0, 0x28 },
		Package(){0x0000FFFF, 1, 0, 0x29 },
		Package(){0x0000FFFF, 2, 0, 0x2A },
		Package(){0x0000FFFF, 3, 0, 0x2B },
	})
}
