/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Nick Barker <Nick.Barker9@btinternet.com>
 * Copyright (C) 2007-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

DefinitionBlock ("dsdt.aml", "DSDT", 1, "CX700 ", "COREBOOT", 0x00000001)
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

	Scope (\) {
		Name (PICF , 0)		// Global flag indicating whether to use PIC or APIC mode
		Method ( _PIC,1)	// The OS is calling this
		{
			Store( Arg0 , PICF)
		}
	} // end of \ scope

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
				Name (CRSP, ResourceTemplate () {
					IRQ (Level, ActiveLow, Shared) {11}
				})
				Name (CRSA, ResourceTemplate () {
					Interrupt (ResourceConsumer, Level, ActiveLow, Shared) {16}
				})

				If (LNot (PICF)) {
					Return (CRSP)
				} Else {
					Return (CRSA)
				}
                	}
			/* Possible Resources - return the range of irqs
 			 * we are using for PCI - only here to keep Linux ACPI
			 * happy
			 */
			Method (_PRS, 0, NotSerialized)
			{
				Name (PRSP, ResourceTemplate () {
					IRQ (Level, ActiveLow, Shared) {3,4,6,7,10,11,12}
                    		})
				Name (PRSA, ResourceTemplate () {
					Interrupt (ResourceConsumer, Level, ActiveLow, Shared) {16,17,18,19,20,21,22,23}
                    		})

				If (LNot (PICF)) {
					Return (PRSP)
				} Else {
					Return (PRSA)
				}

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
				Name (CRSP, ResourceTemplate () {
					IRQ (Level, ActiveLow, Shared) {11}
				})
				Name (CRSA, ResourceTemplate () {
					Interrupt (ResourceConsumer, Level, ActiveLow, Shared) {17}
				})

				If (LNot (PICF)) {
					Return (CRSP)
				} Else {
					Return (CRSA)
				}
                	}
			/* Possible Resources - return the range of irqs
 			 * we are using for PCI - only here to keep Linux ACPI
			 * happy
			 */
			Method (_PRS, 0, NotSerialized)
			{
				Name (PRSP, ResourceTemplate () {
					IRQ (Level, ActiveLow, Shared) {3,4,6,7,10,11,12}
                    		})
				Name (PRSA, ResourceTemplate () {
					Interrupt (ResourceConsumer, Level, ActiveLow, Shared) {16,17,18,19,20,21,22,23}
                    		})

				If (LNot (PICF)) {
					Return (PRSP)
				} Else {
					Return (PRSA)
				}

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
				Name (CRSP, ResourceTemplate () {
					IRQ (Level, ActiveLow, Shared) {10}
				})
				Name (CRSA, ResourceTemplate () {
					Interrupt (ResourceConsumer, Level, ActiveLow, Shared) {18}
				})

				If (LNot (PICF)) {
					Return (CRSP)
				} Else {
					Return (CRSA)
				}
                	}
			/* Possible Resources - return the range of irqs
 			 * we are using for PCI - only here to keep Linux ACPI
			 * happy
			 */
			Method (_PRS, 0, NotSerialized)
			{
				Name (PRSP, ResourceTemplate () {
					IRQ (Level, ActiveLow, Shared) {3,4,6,7,10,11,12}
                    		})
				Name (PRSA, ResourceTemplate () {
					Interrupt (ResourceConsumer, Level, ActiveLow, Shared) {16,17,18,19,20,21,22,23}
                    		})

				If (LNot (PICF)) {
					Return (PRSP)
				} Else {
					Return (PRSA)
				}

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
				Name (CRSP, ResourceTemplate () {
					IRQ (Level, ActiveLow, Shared) {10}
				})
				Name (CRSA, ResourceTemplate () {
					Interrupt (ResourceConsumer, Level, ActiveLow, Shared) {19}
				})

				If (LNot (PICF)) {
					Return (CRSP)
				} Else {
					Return (CRSA)
				}
                	}
			/* Possible Resources - return the range of irqs
 			 * we are using for PCI - only here to keep Linux ACPI
			 * happy
			 */
			Method (_PRS, 0, NotSerialized)
			{
				Name (PRSP, ResourceTemplate () {
					IRQ (Level, ActiveLow, Shared) {3,4,6,7,10,11,12}
                    		})
				Name (PRSA, ResourceTemplate () {
					Interrupt (ResourceConsumer, Level, ActiveLow, Shared) {16,17,18,19,20,21,22,23}
                    		})

				If (LNot (PICF)) {
					Return (PRSP)
				} Else {
					Return (PRSA)
				}

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

		/* PCI Root Bridge */
		Device (PCI0)
		{
			Name (_HID, EisaId ("PNP0A08"))
			Name (_CID, EisaId ("PNP0A03"))
			Name (_ADR, 0x00)
			Name (_UID, 0x00)
			Name (_BBN, 0x00)

			// Mainboard specific IRQ routing
			#include "acpi/irq.asl"

			/* PCI Routing Table */
            		Method (_PRT, 0, NotSerialized)
            		{
                	    If (LNot (PICF))
                	    {
                    		Return (PICM)
                	    }
                	    Else
                	    {
                		Return (APIC)
                            }
            		}

			Device (P2PB) /* PCI to PCI bridge */
			{
				Name (_ADR, 0x00130001)

				#include "acpi/irq-p2p-bridge.asl"
				Method (_PRT, 0, NotSerialized)
				{
					If (LNot (PICF))
					{
						Return (PICM)
					}
					Else
					{
						Return (APIC)
					}
				}
				/* Status - always return ready */
				Method (_STA, 0, NotSerialized)
				{
					Return (0x0F)
				}
			}
		} // End of PCI0
	} // End of _SB
} // End of Definition Block

