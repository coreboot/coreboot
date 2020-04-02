/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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

		/* Bus 0, Dev 2 - PCIe Bridge for x16 slot */
		Package(){0x0002FFFF, 0, INTC, 0 },
		Package(){0x0002FFFF, 1, INTD, 0 },
		Package(){0x0002FFFF, 2, INTA, 0 },
		Package(){0x0002FFFF, 3, INTB, 0 },

		/* Bus 0, Dev 3 - PCIe graphics port 1 bridge */

		/* Bus 0, Dev 4 - PCIe Bridge for 4x slot */
		Package(){0x0004FFFF, 0, INTA, 0 },
		Package(){0x0004FFFF, 1, INTB, 0 },
		Package(){0x0004FFFF, 2, INTC, 0 },
		Package(){0x0004FFFF, 3, INTD, 0 },

		/* Bus 0, Dev 5 - General purpose PCIe bridge 5 */
		/* Bus 0, Dev 6 - PCIe Bridge for Ethernet Chip */
		/* Bus 0, Dev 7 - PCIe Bridge for x1 PCIe Slot */
		/* Bus 0, Dev 8 - Southbridge port (normally hidden) */

#if CONFIG(BOARD_ASUS_F2A85_M_PRO)
		Package(){0x000FFFFF, 0, INTA, 0 },
		Package(){0x000FFFFF, 1, INTB, 0 },
		Package(){0x000FFFFF, 2, INTC, 0 },
		Package(){0x000FFFFF, 3, INTD, 0 },
#endif /* CONFIG_BOARD_ASUS_F2A85_M_PRO */

		/* Bus 0, Dev 20 - F0:SMBus/ACPI,F1:IDE;F2:HDAudio;F3:LPC;F4:PCIBridge;F5:USB */
		Package(){0x0014FFFF, 0, INTA, 0 },
		Package(){0x0014FFFF, 1, INTB, 0 },
		Package(){0x0014FFFF, 2, INTC, 0 },
		Package(){0x0014FFFF, 3, INTD, 0 },

		/* Bus 0, Dev 18,19,22 - USB: OHCI @ func 0
		 *                            EHCI @ func 2 */
		Package(){0x0012FFFF, 0, INTC, 0 },
		Package(){0x0012FFFF, 1, INTB, 0 },

		Package(){0x0013FFFF, 0, INTC, 0 },
		Package(){0x0013FFFF, 1, INTB, 0 },

		Package(){0x0016FFFF, 0, INTC, 0 },
		Package(){0x0016FFFF, 1, INTB, 0 },

		/* SB devices */
		/* Bus 0, Dev 16 - USB: XHCI func 0, 1 */
		Package(){0x0010FFFF, 0, INTC, 0 },
		Package(){0x0010FFFF, 1, INTB, 0 },

		/* Bus 0, Dev 17 - SATA controller */
		Package(){0x0011FFFF, 0, INTD, 0 },

		/* Bus 0, Dev 21 PCIe Bridge */
		Package(){0x0015FFFF, 0, INTA, 0 },
		Package(){0x0015FFFF, 1, INTB, 0 },
		Package(){0x0015FFFF, 2, INTC, 0 },
		Package(){0x0015FFFF, 3, INTD, 0 },
	})

	Name(APR0, Package(){
		/* NB devices in APIC mode */
		/* Bus 0, Dev 0 - F15 Host Controller */
		Package(){0x0000FFFF, 0, 0, 16 },
		Package(){0x0000FFFF, 1, 0, 17 },
		Package(){0x0000FFFF, 2, 0, 18 },
		Package(){0x0000FFFF, 3, 0, 19 },

		/* Bus 0, Dev 1 - PCI Bridge for Internal Graphics(IGP) */
		Package(){0x0001FFFF, 0, 0, 17 },
		Package(){0x0001FFFF, 1, 0, 18 },

		/* Bus 0, Dev 2 - PCIe Bridge for x16 PCIe Slot */
		Package(){0x0002FFFF, 0, 0, 18 },
		Package(){0x0002FFFF, 1, 0, 19 },
		Package(){0x0002FFFF, 2, 0, 16 },
		Package(){0x0002FFFF, 3, 0, 17 },

		/* Bus 0, Dev 3 - PCIe graphics port 1 bridge */

		/* Bus 0, Dev 4 - PCIe Bridge for x4 PCIe Slot black */
		Package(){0x0004FFFF, 0, 0, 16 },
		Package(){0x0004FFFF, 1, 0, 17 },
		Package(){0x0004FFFF, 2, 0, 18 },
		Package(){0x0004FFFF, 3, 0, 19 },

		/* Bus 0, Dev 5 - General purpose PCIe bridge 5 */
		/* Bus 0, Dev 6 - General purpose PCIe bridge 6 */
		/* Bus 0, Dev 7 - PCIe Bridge for network card */
		/* Bus 0, Dev 8 - Southbridge port (normally hidden) */

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

		/* Bus 0, Dev 21 PCIE Bridge */
		Package(){0x0015FFFF, 0, 0, 17 },
		Package(){0x0015FFFF, 1, 0, 18 },
		Package(){0x0015FFFF, 2, 0, 19 },
		Package(){0x0015FFFF, 3, 0, 16 },
	})

	Name(PS2, Package(){
		/* The external GFX - Hooked to PCIe slot 2 */
		Package(){0x0000FFFF, 0, INTD, 0 },
		Package(){0x0000FFFF, 1, INTA, 0 },
		Package(){0x0000FFFF, 2, INTB, 0 },
		Package(){0x0000FFFF, 3, INTC, 0 },
	})
	Name(APS2, Package(){
		/* The external GFX - Hooked to PCIe slot 2 */
		Package(){0x0000FFFF, 0, 0, 18 },
		Package(){0x0000FFFF, 1, 0, 19 },
		Package(){0x0000FFFF, 2, 0, 16 },
		Package(){0x0000FFFF, 3, 0, 17 },
	})

	/* black slot */
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
		/* PCIe slot - Hooked to PCIe slot 6 */
		Package(){0x0000FFFF, 0, INTC, 0 },
		Package(){0x0000FFFF, 1, INTD, 0 },
		Package(){0x0000FFFF, 2, INTA, 0 },
		Package(){0x0000FFFF, 3, INTB, 0 },
	})
	Name(APS6, Package(){
		/* PCIe slot - Hooked to PCIe slot 6 */
		Package(){0x0000FFFF, 0, 0, 18 },
		Package(){0x0000FFFF, 1, 0, 19 },
		Package(){0x0000FFFF, 2, 0, 16 },
		Package(){0x0000FFFF, 3, 0, 17 },
	})

	Name(PS7, Package(){
		/* The onboard Ethernet chip - Hooked to PCIe slot 7 */
		Package(){0x0000FFFF, 0, INTD, 0 },
		Package(){0x0000FFFF, 1, INTA, 0 },
		Package(){0x0000FFFF, 2, INTB, 0 },
		Package(){0x0000FFFF, 3, INTC, 0 },
	})

	Name(APS7, Package(){
		/* The onboard Ethernet chip - Hooked to PCIe slot 7 */
		Package(){0x0000FFFF, 0, 0, 19 },
		Package(){0x0000FFFF, 1, 0, 16 },
		Package(){0x0000FFFF, 2, 0, 17 },
		Package(){0x0000FFFF, 3, 0, 18 },
	})

	Name(PBR0, Package(){
		/* PCIx1 on SB */
		Package(){0x0000FFFF, 0, INTA, 0 },
		Package(){0x0000FFFF, 1, INTB, 0 },
		Package(){0x0000FFFF, 2, INTC, 0 },
		Package(){0x0000FFFF, 3, INTD, 0 },
	})
	Name(ABR0, Package(){
		/* PCIx1 on SB */
		Package(){0x0000FFFF, 0, 0, 0x10 },
		Package(){0x0000FFFF, 1, 0, 0x11 },
		Package(){0x0000FFFF, 2, 0, 0x12 },
		Package(){0x0000FFFF, 3, 0, 0x13 },
	})

	Name(PBR1, Package(){
		/* Onboard network */
		Package(){0x0000FFFF, 0, INTB, 0 },
		Package(){0x0000FFFF, 1, INTC, 0 },
		Package(){0x0000FFFF, 2, INTD, 0 },
		Package(){0x0000FFFF, 3, INTA, 0 },
	})
	Name(ABR1, Package(){
		/* Onboard network */
		Package(){0x0000FFFF, 0, 0, 0x11 },
		Package(){0x0000FFFF, 1, 0, 0x12 },
		Package(){0x0000FFFF, 2, 0, 0x13 },
		Package(){0x0000FFFF, 3, 0, 0x10 },
	})

	/* SB PCI Bridge  */
	Name(PCIB, Package(){
		/* PCI slots: slot 0 behind Dev14, Fun4. */
		Package(){0x0005FFFF, 0, 0, 0x14 },
		Package(){0x0005FFFF, 1, 0, 0x15 },
		Package(){0x0005FFFF, 2, 0, 0x16 },
		Package(){0x0005FFFF, 3, 0, 0x17 },
	})
