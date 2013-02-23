/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
		Package(){0x0001FFFF, 0, INTC, 0 },
		Package(){0x0001FFFF, 1, INTD, 0 },
		/* Bus 0, Dev 2 - */
		Package(){0x0002FFFF, 0, INTC, 0 },
		Package(){0x0002FFFF, 1, INTD, 0 },
		Package(){0x0002FFFF, 2, INTA, 0 },
		Package(){0x0002FFFF, 3, INTB, 0 },
		/* Bus 0, Dev 3 - */
		Package(){0x0003FFFF, 0, INTD, 0 },
		Package(){0x0003FFFF, 1, INTA, 0 },
		Package(){0x0003FFFF, 2, INTB, 0 },
		Package(){0x0003FFFF, 3, INTC, 0 },
		/* Bus 0, Dev 4 - PCIe Bridge for Express Card Slot */
		Package(){0x0004FFFF, 0, INTA, 0 },
		Package(){0x0004FFFF, 1, INTB, 0 },
		Package(){0x0004FFFF, 2, INTC, 0 },
		Package(){0x0004FFFF, 3, INTD, 0 },
		/* Bus 0, Dev 5 - General purpose PCIe bridge 5 */
		Package(){0x0005FFFF, 0, INTB, 0 },
		Package(){0x0005FFFF, 1, INTC, 0 },
		Package(){0x0005FFFF, 2, INTD, 0 },
		Package(){0x0005FFFF, 3, INTA, 0 },
		/* Bus 0, Dev 6 - PCIe Bridge for Ethernet Chip */
		Package(){0x0006FFFF, 0, INTC, 0 },
		Package(){0x0006FFFF, 1, INTD, 0 },
		Package(){0x0006FFFF, 2, INTA, 0 },
		Package(){0x0006FFFF, 3, INTB, 0 },
		/* Bus 0, Dev 7 - PCIe Bridge for x1 PCIe Slot */
		Package(){0x0007FFFF, 0, INTD, 0 },
		Package(){0x0007FFFF, 1, INTA, 0 },
		Package(){0x0007FFFF, 2, INTB, 0 },
		Package(){0x0007FFFF, 3, INTC, 0 },

		/* SB devices */
		/* Bus 0, Dev 20 - F0:SMBus/ACPI,F1:IDE;F2:HDAudio;F3:LPC;F4:PCIBridge;F5:USB */
		Package(){0x0014FFFF, 0, INTA, 0 },
		Package(){0x0014FFFF, 1, INTB, 0 },
		Package(){0x0014FFFF, 2, INTC, 0 },
		Package(){0x0014FFFF, 3, INTD, 0 },
		/* Bus 0, Dev 18,19,22 - USB: OHCI,EHCI */
		Package(){0x0012FFFF, 0, INTC, 0 },
		Package(){0x0012FFFF, 1, INTB, 0 },
		Package(){0x0013FFFF, 0, INTC, 0 },
		Package(){0x0013FFFF, 1, INTB, 0 },
		Package(){0x0016FFFF, 0, INTC, 0 },
		Package(){0x0016FFFF, 1, INTB, 0 },
		Package(){0x0010FFFF, 0, INTC, 0 },
		Package(){0x0010FFFF, 1, INTB, 0 },
		/* Bus 0, Dev 17 - SATA controller #2 */
		Package(){0x0011FFFF, 0, INTD, 0 },
		/* Bus 0, Dev 21 - PCIe Bridge for x1 PCIe Slot */
		Package(){0x0015FFFF, 0, INTA, 0 },
		Package(){0x0015FFFF, 1, INTB, 0 },
		Package(){0x0015FFFF, 2, INTC, 0 },
		Package(){0x0015FFFF, 3, INTD, 0 },
	})

	Name(APR0, Package(){
		/* NB devices in APIC mode */
		/* Bus 0, Dev 0 - RS780 Host Controller */
		/* Bus 0, Dev 1 - PCI Bridge for Internal Graphics */
		Package(){0x0001FFFF, 0, 0, 18 },
		Package(){0x0001FFFF, 1, 0, 19 },
		/* Bus 0, Dev 2 */
		Package(){0x0002FFFF, 0, 0, 18 },
		Package(){0x0002FFFF, 1, 0, 19 },
		Package(){0x0002FFFF, 2, 0, 16 },
		Package(){0x0002FFFF, 3, 0, 17 },
		/* Bus 0, Dev 3 */
		Package(){0x0003FFFF, 0, 0, 19 },
		Package(){0x0003FFFF, 1, 0, 16 },
		Package(){0x0003FFFF, 2, 0, 17 },
		Package(){0x0003FFFF, 3, 0, 18 },
		/* Bus 0, Dev 4 - PCIe Bridge for Express Card Slot */
		Package(){0x0004FFFF, 0, 0, 16 },
		Package(){0x0004FFFF, 1, 0, 17 },
		Package(){0x0004FFFF, 2, 0, 18 },
		Package(){0x0004FFFF, 3, 0, 19 },
		/* Bus 0, Dev 5 - General purpose PCIe bridge 5 */
		Package(){0x0005FFFF, 0, 0, 17 },
		Package(){0x0005FFFF, 1, 0, 18 },
		Package(){0x0005FFFF, 2, 0, 19 },
		Package(){0x0005FFFF, 3, 0, 16 },
		/* Bus 0, Dev 6 - General purpose PCIe bridge 6 */
		Package(){0x0006FFFF, 0, 0, 18 },
		Package(){0x0006FFFF, 1, 0, 19 },
		Package(){0x0006FFFF, 2, 0, 16 },
		Package(){0x0006FFFF, 3, 0, 17 },
		/* Bus 0, Dev 7 - PCIe Bridge for network card */
		Package(){0x0007FFFF, 0, 0, 19 },
		Package(){0x0007FFFF, 1, 0, 16 },
		Package(){0x0007FFFF, 2, 0, 17 },
		Package(){0x0007FFFF, 3, 0, 18 },

		/* SB devices in APIC mode */
		/* Bus 0, Dev 20 - F0:SMBus/ACPI, F1:IDE; F2:HDAudio; F3:LPC; F4:PCIBridge; F5:USB */
		Package(){0x0014FFFF, 0, 0, 16 },
		Package(){0x0014FFFF, 1, 0, 17 },
		Package(){0x0014FFFF, 2, 0, 18 },
		Package(){0x0014FFFF, 3, 0, 19 },
		/* Bus 0, Dev 18,19,22 - USB: OHCI,EHCI*/
		Package(){0x0012FFFF, 0, 0, 18 },
		Package(){0x0012FFFF, 1, 0, 17 },
		Package(){0x0013FFFF, 0, 0, 18 },
		Package(){0x0013FFFF, 1, 0, 17 },
		Package(){0x0016FFFF, 0, 0, 18 },
		Package(){0x0016FFFF, 1, 0, 17 },
		Package(){0x0010FFFF, 0, 0, 18 },
		Package(){0x0010FFFF, 1, 0, 17 },
		/* Bus 0, Dev 17 - SATA controller #2 */
		Package(){0x0011FFFF, 0, 0, 19 },
		/* Bus 0, Dev 21 - PCIe Bridge for x1 PCIe Slot */
		Package(){0x0015FFFF, 0, 0, 16 },
		Package(){0x0015FFFF, 1, 0, 17 },
		Package(){0x0015FFFF, 2, 0, 18 },
		Package(){0x0015FFFF, 3, 0, 19 },
	})

	Name(PS2, Package(){
		/* For Device(PBR2) PIC mode*/
		Package(){0x0000FFFF, 0, INTC, 0 },
		Package(){0x0000FFFF, 1, INTD, 0 },
		Package(){0x0000FFFF, 2, INTA, 0 },
		Package(){0x0000FFFF, 3, INTB, 0 },
	})

	Name(APS2, Package(){
		/* For Device(PBR2) APIC mode*/
		Package(){0x0000FFFF, 0, 0, 18 },
		Package(){0x0000FFFF, 1, 0, 19 },
		Package(){0x0000FFFF, 2, 0, 16 },
		Package(){0x0000FFFF, 3, 0, 17 },
	})

	Name(PS3, Package(){
		/* For Device(PBR3) PIC mode*/
		Package(){0x0000FFFF, 0, INTD, 0 },
		Package(){0x0000FFFF, 1, INTA, 0 },
		Package(){0x0000FFFF, 2, INTB, 0 },
		Package(){0x0000FFFF, 3, INTC, 0 },
	})

	Name(APS3, Package(){
		/* For Device(PBR3) APIC mode*/
		Package(){0x0000FFFF, 0, 0, 19 },
		Package(){0x0000FFFF, 1, 0, 16 },
		Package(){0x0000FFFF, 2, 0, 17 },
		Package(){0x0000FFFF, 3, 0, 18 },
	})

	Name(PS4, Package(){
		/* For Device(PBR4) PIC mode*/
		Package(){0x0000FFFF, 0, INTA, 0 },
		Package(){0x0000FFFF, 1, INTB, 0 },
		Package(){0x0000FFFF, 2, INTC, 0 },
		Package(){0x0000FFFF, 3, INTD, 0 },
	})

	Name(APS4, Package(){
		/* For Device(PBR4) APIC mode*/
		Package(){0x0000FFFF, 0, 0, 16 },
		Package(){0x0000FFFF, 1, 0, 17 },
		Package(){0x0000FFFF, 2, 0, 18 },
		Package(){0x0000FFFF, 3, 0, 19 },
	})

	Name(PS5, Package(){
		/* For Device(PBR5) PIC mode*/
		Package(){0x0000FFFF, 0, INTB, 0 },
		Package(){0x0000FFFF, 1, INTC, 0 },
		Package(){0x0000FFFF, 2, INTD, 0 },
		Package(){0x0000FFFF, 3, INTA, 0 },
	})

	Name(APS5, Package(){
		/* For Device(PBR5) APIC mode*/
		Package(){0x0000FFFF, 0, 0, 17 },
		Package(){0x0000FFFF, 1, 0, 18 },
		Package(){0x0000FFFF, 2, 0, 19 },
		Package(){0x0000FFFF, 3, 0, 16 },
	})

	Name(PS6, Package(){
		/* For Device(PBR6) PIC mode*/
		Package(){0x0000FFFF, 0, INTC, 0 },
		Package(){0x0000FFFF, 1, INTD, 0 },
		Package(){0x0000FFFF, 2, INTA, 0 },
		Package(){0x0000FFFF, 3, INTB, 0 },
	})

	Name(APS6, Package(){
		/* For Device(PBR6) APIC mode*/
		Package(){0x0000FFFF, 0, 0, 18 },
		Package(){0x0000FFFF, 1, 0, 19 },
		Package(){0x0000FFFF, 2, 0, 16 },
		Package(){0x0000FFFF, 3, 0, 17 },
	})

	Name(PS7, Package(){
		/* For Device(PBR7) PIC mode*/
		Package(){0x0000FFFF, 0, INTD, 0 },
		Package(){0x0000FFFF, 1, INTA, 0 },
		Package(){0x0000FFFF, 2, INTB, 0 },
		Package(){0x0000FFFF, 3, INTC, 0 },
	})

	Name(APS7, Package(){
		/* For Device(PBR7) APIC mode*/
		Package(){0x0000FFFF, 0, 0, 19 },
		Package(){0x0000FFFF, 1, 0, 16 },
		Package(){0x0000FFFF, 2, 0, 17 },
		Package(){0x0000FFFF, 3, 0, 18 },
	})

	Name(PE0, Package(){
		/* For Device(PE20) PIC mode*/
		Package(){0x0000FFFF, 0, INTA, 0 },
		Package(){0x0000FFFF, 1, INTB, 0 },
		Package(){0x0000FFFF, 2, INTC, 0 },
		Package(){0x0000FFFF, 3, INTD, 0 },
	})

	Name(APE0, Package(){
		/* For Device(PE20) APIC mode*/
		Package(){0x0000FFFF, 0, 0, 16 },
		Package(){0x0000FFFF, 1, 0, 17 },
		Package(){0x0000FFFF, 2, 0, 18 },
		Package(){0x0000FFFF, 3, 0, 19 },
	})

	Name(PE1, Package(){
		/* For Device(PE21) PIC mode*/
		Package(){0x0000FFFF, 0, INTB, 0 },
		Package(){0x0000FFFF, 1, INTC, 0 },
		Package(){0x0000FFFF, 2, INTD, 0 },
		Package(){0x0000FFFF, 3, INTA, 0 },
	})

	Name(APE1, Package(){
		/* For Device(PE21) APIC mode*/
		Package(){0x0000FFFF, 0, 0, 17 },
		Package(){0x0000FFFF, 1, 0, 18 },
		Package(){0x0000FFFF, 2, 0, 19 },
		Package(){0x0000FFFF, 3, 0, 16 },
	})

	Name(PE2, Package(){
		/* For Device(PE22) PIC mode*/
		Package(){0x0000FFFF, 0, INTC, 0 },
		Package(){0x0000FFFF, 1, INTD, 0 },
		Package(){0x0000FFFF, 2, INTA, 0 },
		Package(){0x0000FFFF, 3, INTB, 0 },
	})

	Name(APE2, Package(){
		/* For Device(PE22) APIC mode*/
		Package(){0x0000FFFF, 0, 0, 18 },
		Package(){0x0000FFFF, 1, 0, 19 },
		Package(){0x0000FFFF, 2, 0, 16 },
		Package(){0x0000FFFF, 3, 0, 17 },
	})

	Name(PE3, Package(){
		/* For Device(PE23) PIC mode*/
		Package(){0x0000FFFF, 0, INTD, 0 },
		Package(){0x0000FFFF, 1, INTA, 0 },
		Package(){0x0000FFFF, 2, INTB, 0 },
		Package(){0x0000FFFF, 3, INTC, 0 },
	})

	Name(APE3, Package(){
		/* For Device(PE23) APIC mode*/
		Package(){0x0000FFFF, 0, 0, 19 },
		Package(){0x0000FFFF, 1, 0, 16 },
		Package(){0x0000FFFF, 2, 0, 17 },
		Package(){0x0000FFFF, 3, 0, 18 },
	})
}
