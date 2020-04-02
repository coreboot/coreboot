/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/* LynxPoint-H */

Scope (\_SB.PCI0.LPCB)
{
	// GWAK: Setup GPIO as ACPI GPE for Wake
	// Arg0: GPIO Number
	Method (GWAK, 1, NotSerialized)
	{
		// Local0 = GPIO Base Address
		Store (And (GPBS, Not(0x1)), Local0)

		// Local1 = BANK, Local2 = OFFSET
		Divide (Arg0, 32, Local2, Local1)

		//
		// Set OWNER to ACPI
		//

		// Local3 = GPIOBASE + GPIO_OWN(BANK)
		Store (Add (Local0, Multiply (Local1, 0x4)), Local3)

		// GPIO_OWN(BANK)
		OperationRegion (IOWN, SystemIO, Local3, 4)
		Field (IOWN, AnyAcc, NoLock, Preserve) {
			GOWN, 32,
		}

		// GPIO_OWN[GPIO] = 0 (ACPI)
		Store (And (GOWN, Not (ShiftLeft (0x1, Local2))), GOWN)

		//
		// Set ROUTE to SCI
		//

		// Local3 = GPIOBASE + GPIO_ROUTE(BANK)
		Store (Add (Add (Local0, 0x30), Multiply (Local1, 0x4)), Local3)

		// GPIO_ROUTE(BANK)
		OperationRegion (IROU, SystemIO, Local3, 4)
		Field (IROU, AnyAcc, NoLock, Preserve) {
			GROU, 32,
		}

		// GPIO_ROUTE[GPIO] = 0 (SCI)
		Store (And (GROU, Not (ShiftLeft (0x1, Local2))), GROU)

		//
		// Set GPnCONFIG to GPIO|INPUT|INVERT
		//

		// Local3 = GPIOBASE + GPnCONFIG0(GPIO)
		Store (Add (Add (Local0, 0x100), Multiply (Arg0, 0x8)), Local3)

		// GPnCONFIG(GPIO)
		OperationRegion (GPNC, SystemIO, Local3, 8)
		Field (GPNC, AnyAcc, NoLock, Preserve) {
			GMOD, 1,  // MODE:   0=NATIVE 1=GPIO
			, 1,
			GIOS, 1,  // IO_SEL: 0=OUTPUT 1=INPUT
			GINV, 1,  // INVERT: 0=NORMAL 1=INVERT
			GLES, 1,  // LxEB:   0=EDGE 1=LEVEL
			, 24,
			ILVL, 1,  // INPUT:  0=LOW 1=HIGH
			OLVL, 1,  // OUTPUT: 0=LOW 1=HIGH
			GPWP, 2,  // PULLUP: 00=NONE 01=DOWN 10=UP 11=INVALID
			ISEN, 1,  // SENSE:  0=ENABLE 1=DISABLE
		}

		Store (0x1, GMOD) // GPIO
		Store (0x1, GIOS) // INPUT
		Store (0x1, GINV) // INVERT
	}
}
