/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 * Copyright (C) 2010 Siemens AG, Inc.
 * (Written by Josef Kellermann <joseph.kellermann@heitec.de> for Siemens AG, Inc.)
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

/* Routing is in System Bus scope */
Scope(\_SB)
{
	Name(PR0, Package(){
		/* NB devices */
		/* SB devices */
		/* Bus 0, Dev 18 - SATA controller #1 */
		Package(){0x0012FFFF, 1, \_SB.PCI0.LPC0.INTG, 0 },

		/* Bus 0, Dev 19 - USB: OHCI, funct 0-4; EHCI, funct 5 */
		Package(){0x0013FFFF, 0, \_SB.PCI0.LPC0.INTA, 0 },
		Package(){0x0013FFFF, 1, \_SB.PCI0.LPC0.INTB, 0 },
		Package(){0x0013FFFF, 2, \_SB.PCI0.LPC0.INTC, 0 },
		Package(){0x0013FFFF, 3, \_SB.PCI0.LPC0.INTD, 0 },

		/* Bus 0, Dev 20 - F0:SMBus/ACPI,F1:IDE;F2:HDAudio;F3:LPC;F4:PCIBridge;F5:AC97 Audio;F6:AC97 Modem */
		Package(){0x0014FFFF, 0, \_SB.PCI0.LPC0.INTA, 0 },
		Package(){0x0014FFFF, 1, \_SB.PCI0.LPC0.INTB, 0 },
		Package(){0x0014FFFF, 2, \_SB.PCI0.LPC0.INTC, 0 },
		Package(){0x0014FFFF, 3, \_SB.PCI0.LPC0.INTD, 0 },
	})

	Name(APR0, Package(){
		/* NB devices in APIC mode */
		/* Bus 0, Dev 0 - RS690 Host Controller */
		/* SB devices in APIC mode */
		/* Bus 0, Dev 18 - SATA controller #1 */
		Package(){0x0012FFFF, 0, 0, 22 },

		/* Bus 0, Dev 19 - USB: OHCI, funct 0-4; EHCI, funct 5 */
		Package(){0x0013FFFF, 0, 0, 16 },
		Package(){0x0013FFFF, 1, 0, 17 },
		Package(){0x0013FFFF, 2, 0, 18 },
		Package(){0x0013FFFF, 3, 0, 19 },

		/* Bus 0, Dev 20 - F0:SMBus/ACPI, F1:IDE; F2:HDAudio; F3:LPC; F4:PCIBridge; F5:AC97 Audio; F6:AC97 Modem */
		Package(){0x0014FFFF, 0, 0, 16 },
		Package(){0x0014FFFF, 1, 0, 17 },
		Package(){0x0014FFFF, 2, 0, 18 },
		Package(){0x0014FFFF, 3, 0, 19 },
	})

	Name(PR1, Package(){
		/* Internal graphics - RS690 VGA, Bus1, Dev5 */
		Package(){0x0005FFFF, 0, \_SB.PCI0.LPC0.INTA, 0 },
		Package(){0x0005FFFF, 1, \_SB.PCI0.LPC0.INTB, 0 },
		Package(){0x0005FFFF, 2, \_SB.PCI0.LPC0.INTC, 0 },
		Package(){0x0005FFFF, 3, \_SB.PCI0.LPC0.INTD, 0 },
	})

	Name(APR1, Package(){
		/* Internal graphics - RS690 VGA, Bus1, Dev5 */
		Package(){0x0005FFFF, 0, 0, 18 },
		Package(){0x0005FFFF, 1, 0, 19 },
	})

	Name(PS2, Package(){
		/* The external GFX - Hooked to PCIe slot 2 */
		Package(){0x0000FFFF, 0, \_SB.PCI0.LPC0.INTC, 0 },
		Package(){0x0000FFFF, 1, \_SB.PCI0.LPC0.INTD, 0 },
		Package(){0x0000FFFF, 2, \_SB.PCI0.LPC0.INTA, 0 },
		Package(){0x0000FFFF, 3, \_SB.PCI0.LPC0.INTB, 0 },
	})

	Name(APS2, Package(){
		/* The external GFX - Hooked to PCIe slot 2 */
		Package(){0x0000FFFF, 0, 0, 18 },
		Package(){0x0000FFFF, 1, 0, 19 },
		Package(){0x0000FFFF, 2, 0, 16 },
		Package(){0x0000FFFF, 3, 0, 17 },
	})

	Name(PS4, Package(){
		/* PCIe slot - Hooked to PCIe slot 4 */
		Package(){0x0000FFFF, 0, \_SB.PCI0.LPC0.INTA, 0 },
		Package(){0x0000FFFF, 1, \_SB.PCI0.LPC0.INTB, 0 },
		Package(){0x0000FFFF, 2, \_SB.PCI0.LPC0.INTC, 0 },
		Package(){0x0000FFFF, 3, \_SB.PCI0.LPC0.INTD, 0 },
	})

	Name(APS4, Package(){
		/* PCIe slot - Hooked to PCIe slot 4 */
		Package(){0x0000FFFF, 0, 0, 16 },
		Package(){0x0000FFFF, 1, 0, 17 },
		Package(){0x0000FFFF, 2, 0, 18 },
		Package(){0x0000FFFF, 3, 0, 19 },
	})

	Name(PS5, Package(){
		/* PCIe slot - Hooked to PCIe slot 5 */
		Package(){0x0000FFFF, 0, \_SB.PCI0.LPC0.INTB, 0 },
		Package(){0x0000FFFF, 1, \_SB.PCI0.LPC0.INTC, 0 },
		Package(){0x0000FFFF, 2, \_SB.PCI0.LPC0.INTD, 0 },
		Package(){0x0000FFFF, 3, \_SB.PCI0.LPC0.INTA, 0 },
	})

	Name(APS5, Package(){
		/* PCIe slot - Hooked to PCIe slot 5 */
		Package(){0x0000FFFF, 0, 0, 17 },
		Package(){0x0000FFFF, 1, 0, 18 },
		Package(){0x0000FFFF, 2, 0, 19 },
		Package(){0x0000FFFF, 3, 0, 16 },
	})

	Name(PS6, Package(){
		/* PCIe slot - Hooked to PCIe slot 6 */
		Package(){0x0000FFFF, 0, \_SB.PCI0.LPC0.INTC, 0 },
		Package(){0x0000FFFF, 1, \_SB.PCI0.LPC0.INTD, 0 },
		Package(){0x0000FFFF, 2, \_SB.PCI0.LPC0.INTA, 0 },
		Package(){0x0000FFFF, 3, \_SB.PCI0.LPC0.INTB, 0 },
	})

	Name(APS6, Package(){
		/* PCIe slot - Hooked to PCIe slot 6 */
		Package(){0x0000FFFF, 0, 0, 18 },
		Package(){0x0000FFFF, 1, 0, 19 },
		Package(){0x0000FFFF, 2, 0, 16 },
		Package(){0x0000FFFF, 3, 0, 17 },
	})

	Name(PS7, Package(){
		/* PCIe slot - Hooked to PCIe slot 7 */
		Package(){0x0000FFFF, 0, \_SB.PCI0.LPC0.INTD, 0 },
		Package(){0x0000FFFF, 1, \_SB.PCI0.LPC0.INTA, 0 },
		Package(){0x0000FFFF, 2, \_SB.PCI0.LPC0.INTB, 0 },
		Package(){0x0000FFFF, 3, \_SB.PCI0.LPC0.INTC, 0 },
	})

	Name(APS7, Package(){
		/* PCIe slot - Hooked to PCIe slot 7 */
		Package(){0x0000FFFF, 0, 0, 19 },
		Package(){0x0000FFFF, 1, 0, 16 },
		Package(){0x0000FFFF, 2, 0, 17 },
		Package(){0x0000FFFF, 3, 0, 18 },
	})

	Name(PCIB, Package(){
		/* PCI slots: slot 1 behind Dev14, Fun4. */
		Package(){0x005FFFF, 0, \_SB.PCI0.LPC0.INTF, 0 }, // Phoenix does it
		Package(){0x005FFFF, 1, \_SB.PCI0.LPC0.INTG, 0 }, // Phoenix does it
		Package(){0x004FFFF, 0, \_SB.PCI0.LPC0.INTE, 0 },
		Package(){0x004FFFF, 1, \_SB.PCI0.LPC0.INTF, 0 },
		Package(){0x004FFFF, 2, \_SB.PCI0.LPC0.INTG, 0 },
		Package(){0x004FFFF, 3, \_SB.PCI0.LPC0.INTH, 0 },
	})

	Name(AP2P, Package(){
		/* PCI slots: slot 0 behind Dev14, Fun4. */
		Package(){0x0005FFFF, 0, 0, 21 }, // Phoenix does it
		Package(){0x0005FFFF, 1, 0, 22 }, // Phoenix does it
		Package(){0x0004FFFF, 0, 0, 20 },
		Package(){0x0004FFFF, 1, 0, 21 },
		Package(){0x0004FFFF, 2, 0, 22 },
		Package(){0x0004FFFF, 3, 0, 23 },
	})

}
