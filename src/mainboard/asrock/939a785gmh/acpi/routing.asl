/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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
		/* Bus 0, Dev 0 - RS780 Host Controller */
		/* Bus 0, Dev 1 - PCI Bridge for Internal Graphics */
		/* Bus 0, Dev 2 - PCIe Bridge for x8 PCIe Slot (GFX0) */

		Package(){0x0002FFFF, 0, INTC, 0 },
		Package(){0x0002FFFF, 1, INTD, 0 },
		Package(){0x0002FFFF, 2, INTA, 0 },
		Package(){0x0002FFFF, 3, INTB, 0 },

		/* Bus 0, Funct 8 - Southbridge port (normally hidden) */

		/* SB devices */
		/* Bus 0, Dev 17 - SATA controller #2 */
		/* Bus 0, Dev 18 - SATA controller #1 */
		Package(){0x0011FFFF, 0, INTA, 0 },

		/* Bus 0, Dev 19 - USB: OHCI, dev 18,19 func 0-2, dev 20 func 5;
		 * EHCI, dev 18, 19 func 2 */
		Package(){0x0012FFFF, 0, INTA, 0 },
		Package(){0x0012FFFF, 1, INTB, 0 },
		Package(){0x0012FFFF, 2, INTC, 0 },
		Package(){0x0012FFFF, 3, INTD, 0 },

		Package(){0x0013FFFF, 0, INTC, 0 },
		Package(){0x0013FFFF, 1, INTD, 0 },
		Package(){0x0013FFFF, 2, INTA, 0 },
		Package(){0x0013FFFF, 3, INTB, 0 },

		/* Bus 0, Dev 20 - F0:SMBus/ACPI,F1:IDE;F2:HDAudio;F3:LPC;F4:PCIBridge;F5:USB */
		Package(){0x0014FFFF, 0, INTA, 0 },
		Package(){0x0014FFFF, 1, INTB, 0 },
		Package(){0x0014FFFF, 2, INTC, 0 },
		Package(){0x0014FFFF, 3, INTD, 0 },
	})

	Name(APR0, Package(){
		/* NB devices in APIC mode */
		/* Bus 0, Dev 0 - RS780 Host Controller */

		/* Bus 0, Dev 1 - PCI Bridge for Internal Graphics */
		/* Package(){0x0001FFFF, 0, 0, 18 }, */
		/* Package(){0x0001FFFF, 1, 0, 19 }, */

		/* Bus 0, Dev 2 - PCIe Bridge for x8 PCIe Slot (GFX0) */
		Package(){0x0002FFFF, 0, 0, 18 },
		Package(){0x0002FFFF, 1, 0, 19 },
		Package(){0x0002FFFF, 2, 0, 16 },
		Package(){0x0002FFFF, 3, 0, 17 },

		/* Bus 0, Dev 9 - PCIe x1 slot  */
		Package(){0x0009FFFF, 0, 0, 17 },
		Package(){0x0009FFFF, 1, 0, 18 },
		Package(){0x0009FFFF, 2, 0, 19 },
		Package(){0x0009FFFF, 3, 0, 10 },

		/* Bus 0, Dev A - PCIe internal ethernet */
		Package(){0x000AFFFF, 0, 0, 18 },
		Package(){0x000AFFFF, 1, 0, 19 },
		Package(){0x000AFFFF, 2, 0, 16 },
		Package(){0x000AFFFF, 3, 0, 17 },
		/* Bus 0, Funct 8 - Southbridge port (normally hidden) */

		/* SB devices in APIC mode */
		/* Bus 0, Dev 17 - SATA controller #2 */
		/* Bus 0, Dev 18 - SATA controller #1 */
		Package(){0x0011FFFF, 0, 0, 22 },

		/* Bus 0, Dev 19 - USB: OHCI, dev 18,19 func 0-2, dev 20 func 5;
		 * EHCI, dev 18, 19 func 2 */
		Package(){0x0012FFFF, 0, 0, 16 },
		Package(){0x0012FFFF, 1, 0, 17 },
		Package(){0x0012FFFF, 2, 0, 18 },
		Package(){0x0012FFFF, 3, 0, 19 },

		Package(){0x0013FFFF, 0, 0, 18 },
		Package(){0x0013FFFF, 1, 0, 19 },
		Package(){0x0013FFFF, 2, 0, 16 },
		Package(){0x0013FFFF, 3, 0, 17 },

		/* Bus 0, Dev 20 - F0:SMBus/ACPI, F1:IDE; F2:HDAudio; F3:LPC; F4:PCIBridge; F5:USB */
		Package(){0x0014FFFF, 0, 0, 16 },
		Package(){0x0014FFFF, 1, 0, 17 },
		Package(){0x0014FFFF, 2, 0, 18 },
		Package(){0x0014FFFF, 3, 0, 19 },
	})

	Name(PR1, Package(){
		/* Internal graphics - RS780 VGA, Bus1, Dev5 */
		Package(){0x0005FFFF, 0, INTC, 0 },
		Package(){0x0005FFFF, 1, INTD, 0 },
		Package(){0x0005FFFF, 2, INTA, 0 },
		Package(){0x0005FFFF, 3, INTB, 0 },
	})

	Name(APR1, Package(){
		/* Internal graphics - RS780 VGA, Bus1, Dev5 */
		Package(){0x0005FFFF, 0, 0, 18 },
		Package(){0x0005FFFF, 1, 0, 19 },
		Package(){0x0005FFFF, 2, 0, 16 },
		Package(){0x0005FFFF, 3, 0, 11 },
	})

	Name(PS2, Package(){
		/* The external GFX - Hooked to PCIe slot 2 */
		Package(){0x0000FFFF, 0, INTC, 0 },
		Package(){0x0000FFFF, 1, INTD, 0 },
		Package(){0x0000FFFF, 2, INTA, 0 },
		Package(){0x0000FFFF, 3, INTB, 0 },
	})

	Name(APS2, Package(){
		/* The external GFX - Hooked to PCIe slot 2 */
		Package(){0x0000FFFF, 0, 0, 18 },
		Package(){0x0000FFFF, 1, 0, 19 },
		Package(){0x0000FFFF, 2, 0, 16 },
		Package(){0x0000FFFF, 3, 0, 17 },
	})

	Name(PS9, Package(){
		/* PCIe slot - Hooked to PCIe x1 */
		Package(){0x0000FFFF, 0, INTD, 0 },
		Package(){0x0000FFFF, 1, INTA, 0 },
		Package(){0x0000FFFF, 2, INTB, 0 },
		Package(){0x0000FFFF, 3, INTC, 0 },
	})

	Name(APS9, Package(){
		/* PCIe slot - Hooked to PCIe x1 */
		Package(){0x0000FFFF, 0, 0, 17 },
		Package(){0x0000FFFF, 1, 0, 18 },
		Package(){0x0000FFFF, 2, 0, 19 },
		Package(){0x0000FFFF, 3, 0, 16 },
	})
	Name(PSa, Package(){
		/* PCIe slot - Hooked to ethernet */
		Package(){0x0000FFFF, 0, INTD, 0 },
		Package(){0x0000FFFF, 1, INTA, 0 },
		Package(){0x0000FFFF, 2, INTB, 0 },
		Package(){0x0000FFFF, 3, INTC, 0 },
	})

	Name(APSa, Package(){
		/* PCIe slot - Hooked to PCIe slot 10 */
		Package(){0x0000FFFF, 0, 0, 18 },
		Package(){0x0000FFFF, 1, 0, 19 },
		Package(){0x0000FFFF, 2, 0, 16 },
		Package(){0x0000FFFF, 3, 0, 17 },
	})

	Name(PCIB, Package(){
		/* PCI slots: slot 0, slot 1, behind Dev14, Fun4. */
		Package(){0x0005FFFF, 0, 0, 0x14 },
		Package(){0x0005FFFF, 1, 0, 0x15 },
		Package(){0x0005FFFF, 2, 0, 0x16 },
		Package(){0x0005FFFF, 3, 0, 0x17 },
		Package(){0x0006FFFF, 0, 0, 0x15 },
		Package(){0x0006FFFF, 1, 0, 0x16 },
		Package(){0x0006FFFF, 2, 0, 0x17 },
		Package(){0x0006FFFF, 3, 0, 0x14 },
	})
}
