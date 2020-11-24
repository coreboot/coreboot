/* SPDX-License-Identifier: GPL-2.0-only */

	/* Routing is in System Bus scope */
	Name(PR0, Package(){
		/* NB devices */
		/* Bus 0, Dev 0 - F15 Host Controller */
		Package(){0x0000FFFF, 0, INTA, 0 },
		Package(){0x0000FFFF, 1, INTB, 0 },
		Package(){0x0000FFFF, 2, INTC, 0 },
		Package(){0x0000FFFF, 3, INTD, 0 },

		/* Bus 0, Dev 1 - PCI Bridge for Internal Graphics(IGP) */
		Package(){0x0001FFFF, 0, INTB, 0 },
		Package(){0x0001FFFF, 1, INTC, 0 },

		/* Bus 0, Dev 2 - PCIe Bridge for x8 PCIe Slot (GFX0) */
		Package(){0x0002FFFF, 0, INTC, 0 },
		Package(){0x0002FFFF, 1, INTD, 0 },
		Package(){0x0002FFFF, 2, INTA, 0 },
		Package(){0x0002FFFF, 3, INTB, 0 },

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

		/* Bus 0, Dev 20 - F0:SMBus/ACPI,F1:IDE;F2:HDAudio;F3:LPC;F4:PCIBridge;F5:USB */
		Package(){0x0014FFFF, 0, INTA, 0 },
		Package(){0x0014FFFF, 1, INTB, 0 },
		Package(){0x0014FFFF, 2, INTC, 0 },
		Package(){0x0014FFFF, 3, INTD, 0 },

		/* SB devices */
		/* Bus 0, Dev 18,19,22 - USB: OHCI @ func 0
		 *                            EHCI @ func 2 */
		Package(){0x0012FFFF, 0, INTC, 0 },
		Package(){0x0012FFFF, 1, INTB, 0 },

		Package(){0x0013FFFF, 0, INTC, 0 },
		Package(){0x0013FFFF, 1, INTB, 0 },

		Package(){0x0016FFFF, 0, INTC, 0 },
		Package(){0x0016FFFF, 1, INTB, 0 },

		/* Bus 0, Dev 16 - USB: XHCI func 0, 1 */
		Package(){0x0010FFFF, 0, INTC, 0 },
		Package(){0x0010FFFF, 1, INTB, 0 },

		/* Bus 0, Dev 17 - SATA controller */
		Package(){0x0011FFFF, 0, INTD, 0 },
	})

	Name(APR0, Package(){
		/* NB devices in APIC mode */
		/* Bus 0, Dev 0 - F15 Host Controller */

		/* Bus 0, Dev 1 - PCI Bridge for Internal Graphics(IGP) */
		Package(){0x0001FFFF, 0, 0, 17 },
		Package(){0x0001FFFF, 1, 0, 18 },

		/* Bus 0, Dev 2 - PCIe Bridge for x8 PCIe Slot (GFX0) */
		Package(){0x0002FFFF, 0, 0, 18 },
		Package(){0x0002FFFF, 1, 0, 19 },
		Package(){0x0002FFFF, 2, 0, 16 },
		Package(){0x0002FFFF, 3, 0, 17 },

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

		/* Bus 0, Dev 20 - F0:SMBus/ACPI, F1:IDE; F2:HDAudio; F3:LPC; F4:PCIBridge; F5:USB */
		Package(){0x0014FFFF, 0, 0, 16 },
		Package(){0x0014FFFF, 1, 0, 17 },
		Package(){0x0014FFFF, 2, 0, 18 },
		Package(){0x0014FFFF, 3, 0, 19 },

		/* SB devices in APIC mode */
		/* Bus 0, Dev 18,19,22 - USB: OHCI @ func 0
		 *                            EHCI @ func 2 */
		Package(){0x0012FFFF, 0, 0, 18 },
		Package(){0x0012FFFF, 1, 0, 17 },

		Package(){0x0013FFFF, 0, 0, 18 },
		Package(){0x0013FFFF, 1, 0, 17 },

		Package(){0x0016FFFF, 0, 0, 18 },
		Package(){0x0016FFFF, 1, 0, 17 },

		/* Bus 0, Dev 16 - USB: XHCI func 0, 1 */
		Package(){0x0010FFFF, 0, 0, 0x12},
		Package(){0x0010FFFF, 1, 0, 0x11},

		/* Bus 0, Dev 17 - SATA controller */
		Package(){0x0011FFFF, 0, 0, 19 },
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

	Name(PS4, Package(){
		/* PCIe slot - Hooked to PCIe slot 4 */
		Package(){0x0000FFFF, 0, INTA, 0 },
		Package(){0x0000FFFF, 1, INTB, 0 },
		Package(){0x0000FFFF, 2, INTC, 0 },
		Package(){0x0000FFFF, 3, INTD, 0 },
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
		Package(){0x0000FFFF, 0, INTB, 0 },
		Package(){0x0000FFFF, 1, INTC, 0 },
		Package(){0x0000FFFF, 2, INTD, 0 },
		Package(){0x0000FFFF, 3, INTA, 0 },
	})
	Name(APS5, Package(){
		/* PCIe slot - Hooked to PCIe slot 5 */
		Package(){0x0000FFFF, 0, 0, 17 },
		Package(){0x0000FFFF, 1, 0, 18 },
		Package(){0x0000FFFF, 2, 0, 19 },
		Package(){0x0000FFFF, 3, 0, 16 },
	})
	Name(PS6, Package(){
	})
	Name(APS6, Package(){
	})
	Name(PS7, Package(){
	})
	Name(APS7, Package(){
	})
	Name(PCIB, Package(){
	})
