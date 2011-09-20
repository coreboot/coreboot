/*
 * Minimalist ACPI DSDT table for EPIA-M / MII
 * (C) Copyright 2004 Nick Barker <Nick.Barker9@btinternet.com>
 *
 *
 */

DefinitionBlock ("DSDT.aml", "DSDT", 1, "CORE  ", "COREBOOT", 1)
{
	/*
	 * Define the main processor
	 */
	Scope (\_PR)
	{
		Processor (\_PR.CPU0, 0x00, 0x00000410, 0x06) {}
	}

	/* For now only define 2 power states:
	 *  - S0 which is fully on
	 *  - S5 which is soft off
	 * any others would involve declaring the wake up methods
	 */
	Name (\_S0, Package () {0x00, 0x00, 0x00, 0x00 })
	Name (\_S5, Package () {0x02, 0x02, 0x00, 0x00 })

	/* Root of the bus hierarchy */
	Scope (\_SB)
    	{
		/* Define how interrupt Link A is plumbed in */
		Device (LNKA)
		{
			Name (_HID, EisaId ("PNP0C0F"))
			Name (_UID, 0x01)
			/* Status - always return ready */
			Method (_STA, 0, NotSerialized)
			{
				Return (0x0B)
 			}
			/* Current Resources - return irq set up in BIOS */
			Method (_CRS, 0, NotSerialized)
			{
				Name (BUFF, ResourceTemplate ()
				{
					IRQ (Level, ActiveLow, Shared) {5}
				})
				Return (BUFF)
                	}
			/* Possible Resources - return the range of irqs
 			 * we are using for PCI - only here to keep Linux ACPI
			 * happy
			 */
			Method (_PRS, 0, NotSerialized)
			{
				Name (BUFF, ResourceTemplate ()
                    		{
					IRQ (Level, ActiveLow, Shared) {5,9,10}
                    		})
                    		Return (BUFF)
                	}
			/* Set Resources - dummy function to keep Linux ACPI happy
                         * Linux is more than happy not to tinker with irq
			 * assignments as long as the CRS and STA functions
			 * return good values
			 */
			Method (_SRS, 1, NotSerialized ) {}
			/* Disable - dummy function to keep Linux ACPI happy */
			Method (_DIS, 0, NotSerialized ) {}

		} // End of LNKA

		/* Define how interrupt Link B is plumbed in */
		Device (LNKB)
		{
			Name (_HID, EisaId ("PNP0C0F"))
			Name (_UID, 0x02)
			/* Status - always return ready */
			Method (_STA, 0, NotSerialized)
			{
				Return (0x0B)
 			}
			/* Current Resources - return irq set up in BIOS */
			Method (_CRS, 0, NotSerialized)
			{
				Name (BUFF, ResourceTemplate ()
				{
					IRQ (Level, ActiveLow, Shared) {9}
				})
				Return (BUFF)
                	}
			/* Possible Resources - return the range of irqs
			 * we are using for PCI - only here to keep Linux ACPI
			 * happy
			 */
			Method (_PRS, 0, NotSerialized)
			{
				Name (BUFF, ResourceTemplate ()
                    		{
					IRQ (Level, ActiveLow, Shared) {5,9,10}
                    		})
                    		Return (BUFF)
                	}
			/* Set Resources - dummy function to keep Linux ACPI happy
                         * Linux is more than happy not to tinker with irq
			 * assignments as long as the CRS and STA functions
			 * return good values
			 */
			Method (_SRS, 1, NotSerialized ) {}
			/* Disable - dummy function to keep Linux ACPI happy */
			Method (_DIS, 0, NotSerialized ) {}

		} // End of LNKB

		/* Define how interrupt Link C is plumbed in */
		Device (LNKC)
		{
			Name (_HID, EisaId ("PNP0C0F"))
			Name (_UID, 0x03)
			/* Status - always return ready */
			Method (_STA, 0, NotSerialized)
			{
				Return (0x0B)
 			}
			/* Current Resources - return irq set up in BIOS */
			Method (_CRS, 0, NotSerialized)
			{
				Name (BUFF, ResourceTemplate ()
				{
					IRQ (Level, ActiveLow, Shared) {9}
				})
				Return (BUFF)
                	}
			/* Possible Resources - return the range of irqs
			 * we are using for PCI - only here to keep Linux ACPI
			 * happy
			 */
			Method (_PRS, 0, NotSerialized)
			{
				Name (BUFF, ResourceTemplate ()
                    		{
					IRQ (Level, ActiveLow, Shared) {5,9,10}
                    		})
                    		Return (BUFF)
                	}
			/* Set Resources - dummy function to keep Linux ACPI happy
                         * Linux is more than happy not to tinker with irq
			 * assignments as long as the CRS and STA functions
			 * return good values
			 */
			Method (_SRS, 1, NotSerialized ) {}
			/* Disable - dummy function to keep Linux ACPI happy */
			Method (_DIS, 0, NotSerialized ) {}

		} // End of LNKC

		/* Define how interrupt Link D is plumbed in */
		Device (LNKD)
		{
			Name (_HID, EisaId ("PNP0C0F"))
			Name (_UID, 0x04)
			/* Status - always return ready */
			Method (_STA, 0, NotSerialized)
			{
				Return (0x0B)
 			}
			/* Current Resources - return irq set up in BIOS */
			Method (_CRS, 0, NotSerialized)
			{
				Name (BUFF, ResourceTemplate ()
				{
					IRQ (Level, ActiveLow, Shared) {5}
				})
				Return (BUFF)
                	}
			/* Possible Resources - return the range of irqs
			 * we are using for PCI - only here to keep Linux ACPI
			 * happy
			 */
			Method (_PRS, 0, NotSerialized)
			{
				Name (BUFF, ResourceTemplate ()
                    		{
					IRQ (Level, ActiveLow, Shared) {5,9,10}
                    		})
                    		Return (BUFF)
                	}
			/* Set Resources - dummy function to keep Linux ACPI happy
                         * Linux is more than happy not to tinker with irq
			 * assignments as long as the CRS and STA functions
			 * return good values
			 */
			Method (_SRS, 1, NotSerialized ) {}
			/* Disable - dummy function to keep Linux ACPI happy */
			Method (_DIS, 0, NotSerialized ) {}

		} // End of LNKD


		/* top PCI device */
		Device (PCI0)
		{
			Name (_HID, EisaId ("PNP0A03"))
			Name (_ADR, 0x00)
			Name (_UID, 0x00)
			Name (_BBN, 0x00)

			/* PCI Routing Table */
			Name (_PRT, Package () {
				/* Epia-MII 6000e cardbus: */
				Package () {0x000AFFFF, 0x00, LNKA, 0x00}, // Cardbus Link A
				Package () {0x000AFFFF, 0x01, LNKB, 0x00}, // Cardbus Link B
				Package () {0x000AFFFF, 0x02, LNKC, 0x00}, // Cardbus Link C
				Package () {0x000AFFFF, 0x03, LNKD, 0x00}, // Cardbus Link D

				Package () {0x000DFFFF, 0x00, LNKB, 0x00}, // Firewire Link B
				Package () {0x000DFFFF, 0x01, LNKC, 0x00}, // Firewire Link C
				Package () {0x000DFFFF, 0x02, LNKD, 0x00}, // Firewire Linc D
				Package () {0x000DFFFF, 0x03, LNKA, 0x00}, // Firewire Link A

				Package () {0x0010FFFF, 0x00, LNKA, 0x00}, // USB Link A
				Package () {0x0010FFFF, 0x01, LNKB, 0x00}, // USB Link B
				Package () {0x0010FFFF, 0x02, LNKC, 0x00}, // USB Link C
				Package () {0x0010FFFF, 0x03, LNKD, 0x00}, // USB Link D

				Package () {0x0011FFFF, 0x00, LNKA, 0x00}, // vt8623 Link A
				Package () {0x0011FFFF, 0x01, LNKB, 0x00}, // vt8623 Link B
				Package () {0x0011FFFF, 0x02, LNKC, 0x00}, // vt8623 Link C
				Package () {0x0011FFFF, 0x03, LNKD, 0x00}, // vt8623 Link D

				Package () {0x0012FFFF, 0x00, LNKA, 0x00}, // LAN Link A
				Package () {0x0012FFFF, 0x01, LNKB, 0x00}, // LAN Link B
				Package () {0x0012FFFF, 0x02, LNKC, 0x00}, // LAN Link C
				Package () {0x0012FFFF, 0x03, LNKD, 0x00}, // LAN Link D

				Package () {0x0013FFFF, 0x00, LNKA, 0x00}, // Riser slot LinkA
				Package () {0x0013FFFF, 0x01, LNKB, 0x00}, // Riser slot LinkB
				Package () {0x0013FFFF, 0x02, LNKC, 0x00}, // Riser slot LinkC
				Package () {0x0013FFFF, 0x03, LNKD, 0x00}, // Riser slot LinkD

				Package () {0x0014FFFF, 0x00, LNKB, 0x00}, // Slot 1, Link B
				Package () {0x0014FFFF, 0x01, LNKC, 0x00}, // Slot 1, Link C
				Package () {0x0014FFFF, 0x02, LNKD, 0x00}, // Slot 1, Link D
				Package () {0x0014FFFF, 0x03, LNKA, 0x00}, // Slot 1, Link A

				Package () {0x0001FFFF, 0x00, LNKA, 0x00}, // VGA Link A
				Package () {0x0001FFFF, 0x01, LNKB, 0x00}, // VGA Link B
				Package () {0x0001FFFF, 0x02, LNKC, 0x00}, // VGA Link C
				Package () {0x0001FFFF, 0x03, LNKD, 0x00} // VGA Link D

            		})


		} // End of PCI0

	} // End of _SB

} // End of Definition Block

