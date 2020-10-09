/* SPDX-License-Identifier: GPL-2.0-only */

Device (GPIO)
{
	// GPIO Controller
	Method (_HID)
	{
		If (\ISWP ()) {
			// WildcatPoint
			Return ("INT3437")
		}

		// LynxPoint-LP
		Return ("INT33C7")
	}
	Name (_UID, 1)

	Name (RBUF, ResourceTemplate ()
	{
		DWordIo (ResourceProducer,
			MinFixed,    // IsMinFixed
			MaxFixed,    // IsMaxFixed
			PosDecode,   // Decode
			EntireRange, // ISARanges
			0x00000000,  // AddressGranularity
			0x00000000,  // AddressMinimum
			0x00000000,  // AddressMaximum
			0x00000000,  // AddressTranslation
			0x00000000,  // RangeLength
			,            // ResourceSourceIndex
			,            // ResourceSource
			BAR0)
		// Disabled due to IRQ storm: http://crosbug.com/p/29548
		//Interrupt (ResourceConsumer,
		//	Level, ActiveHigh, Shared, , , ) {14}
	})

	Method (_CRS, 0, NotSerialized)
	{
		CreateDwordField (^RBUF, ^BAR0._MIN, BMIN)
		CreateDwordField (^RBUF, ^BAR0._MAX, BMAX)
		CreateDwordField (^RBUF, ^BAR0._LEN, BLEN)

		BLEN = GPIO_BASE_SIZE
		BMIN = GPIO_BASE_ADDRESS
		BMAX = GPIO_BASE_ADDRESS + GPIO_BASE_SIZE - 1

		Return (RBUF)
	}

	Method (_STA, 0, NotSerialized)
	{
		Return (0xF)
	}

	// GWAK: Setup GPIO as ACPI GPE for Wake
	// Arg0: GPIO Number
	Method (GWAK, 1, Serialized)
	{
		// Local0 = GPIO Base Address
		Local0 = GPBS & ~1

		// Local1 = BANK, Local2 = OFFSET
		Local2 = Arg0 % 32
		Local1 = Arg0 / 32

		//
		// Set OWNER to ACPI
		//

		// Local3 = GPIOBASE + GPIO_OWN(BANK)
		Local3 = Local0 + (Local1 * 4)

		// GPIO_OWN(BANK)
		OperationRegion (IOWN, SystemIO, Local3, 4)
		Field (IOWN, AnyAcc, NoLock, Preserve) {
			GOWN, 32,
		}

		// GPIO_OWN[GPIO] = 0 (ACPI)
		GOWN = GOWN & ~(1 << Local2)

		//
		// Set ROUTE to SCI
		//

		// Local3 = GPIOBASE + GPIO_ROUTE(BANK)
		Local3 = Local0 + 0x30  + (Local1 * 4)

		// GPIO_ROUTE(BANK)
		OperationRegion (IROU, SystemIO, Local3, 4)
		Field (IROU, AnyAcc, NoLock, Preserve) {
			GROU, 32,
		}

		// GPIO_ROUTE[GPIO] = 0 (SCI)
		GROU = GROU & ~(1 << Local2)

		//
		// Set GPnCONFIG to GPIO|INPUT|INVERT
		//

		// Local3 = GPIOBASE + GPnCONFIG0(GPIO)
		Local3 = Local0 + 0x100 + (Arg0 * 8)

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

		GMOD = 1 // GPIO
		GIOS = 1 // INPUT
		GINV = 1 // INVERT
	}
}
