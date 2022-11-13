/* SPDX-License-Identifier: GPL-2.0-only */

// Scope is \_SB.PCI0.LPCB

Device (SIO) {
	Name (_ADR, 0x2E)
	OperationRegion (SIOA, SystemIO, 0x2E, 0x02)
	Field (SIOA, ByteAcc, NoLock, Preserve)
	{
		SI2E, 8,
		SI2F, 8,
	}

	IndexField (SI2E, SI2F, ByteAcc, NoLock, Preserve)
	{
		Offset (0x02),
		SCNT, 8,	/* Configure Control */
		Offset (0x07),
		SLDN, 8,	/* Logical Device Number */
		Offset (0x30),
		SACT, 8,	/* Activate */
		Offset (0x60),
		IO0H, 8,	/* Base Address 0 MSB */
		IO0L, 8,	/* Base Address 0 LSB */
		Offset (0x62),
		IO1H, 8,	/* Base Address 1 MSB */
		IO1L, 8,	/* Base Address 1 LSB */
		Offset (0x70),
		IQ00, 8,	/* Interrupt Select */
	}

	Name (SFDC, 0)		/* Floppy Disk Controller */
	Name (SSP1, 1)		/* Serial Port 1 */
	Name (SENV, 4)		/* Environment Controller */
	Name (SKBC, 5)		/* Keyboard */
	Name (SKBM, 6)		/* Mouse */
	Name (SGPI, 7)		/* GPIO */
	Name (SINF, 10)		/* Consumer IR */

	Method (ENTR, 0, NotSerialized)
	{
		SI2E = 0x87
		SI2E = 0x01
		SI2E = 0x55
		SI2E = 0x55
	}

	Method (EXIT, 0, NotSerialized)
	{
		SCNT = 0x02
	}

	/* Parse activate register for an LDN */
	Method (ISEN, 1, NotSerialized)
	{
		ENTR ()
		SLDN = Arg0
		Local0 = SACT
		EXIT ()

		/* Check if it exists */
		If (Local0 == 0xFF)
		{
			Return (0x00)
		}

		/* Check if activated */
		If (Local0 == 1)
		{
			Return (0x0F)
		}
		Else
		{
			Return (0x0D)
		}
	}

	/* Enable an LDN via the activate register */
	Method (SENA, 1, NotSerialized)
	{
		ENTR ()
		SLDN = Arg0
		SACT = 1
		EXIT ()
	}

	/* Disable an LDN via the activate register */
	Method (SDIS, 1, NotSerialized)
	{
		ENTR ()
		SLDN = Arg0
		SACT = 0
		EXIT ()
	}

#ifdef SIO_ENABLE_ENVC
	Device (ENVC) {
		Name (_HID, EISAID ("PNP0C02"))
		Name (_UID, 10)

		Method (_STA, 0, NotSerialized) {
			Return (ISEN (SENV))
		}

		Name (_CRS, ResourceTemplate ()
		{
			IO (Decode16, SIO_ENVC_IO0, SIO_ENVC_IO0, 0x08, 0x08)
			IO (Decode16, SIO_ENVC_IO1, SIO_ENVC_IO1, 0x04, 0x04)
		})

		OperationRegion (ECAP, SystemIO, SIO_ENVC_IO0, 0x07)
		Field (ECAP, ByteAcc, NoLock, Preserve)
		{
			Offset (0x05),
			ECAI, 8,  // Address Index Register
			ECAD, 8,  // Address Data Register
		}

		// Registers for thermal zone implementations
		IndexField (ECAI, ECAD, ByteAcc, NoLock, Preserve)
		{
			Offset (0x29),
			TIN1, 8,  // TMPIN1 Reading
			TIN2, 8,  // TMPIN2 Reading
			TIN3, 8,  // TMPIN3 Reading
			Offset (0x6b),
			F2PS, 8,  // FAN2 PWM Setting
			Offset (0x73),
			F3PS, 8,  // FAN3 PWM Setting
		}
	}
#endif

#ifdef SIO_ENABLE_GPIO
	Device (GPIO) {
		Name (_HID, EISAID ("PNP0C02"))
		Name (_UID, 11)

		Method (_STA, 0, NotSerialized) {
			Return (0x0F)
		}

		Name (_CRS, ResourceTemplate ()
		{
			IO (Decode16, SIO_GPIO_IO0, SIO_GPIO_IO0, 0x01, 0x01)
			IO (Decode16, SIO_GPIO_IO1, SIO_GPIO_IO1, 0x08, 0x08)
		})
	}
#endif

#ifdef SIO_ENABLE_COM1
	Device (COM1) {
		Name (_HID, EISAID ("PNP0501"))
		Name (_UID, 1)

		Method (_STA, 0, NotSerialized) {
			Return (ISEN (SSP1))
		}

		Name (_CRS, ResourceTemplate ()
		{
			IO (Decode16, 0x03F8, 0x03F8, 0x08, 0x08)
			IRQNoFlags () {4}
		})
	}
#endif

#ifdef SIO_ENABLE_PS2K
	Device (PS2K)		// Keyboard
	{
		Name (_HID, EISAID("PNP0303"))
		Name (_CID, EISAID("PNP030B"))

		Method (_STA, 0, NotSerialized) {
			Return (ISEN (SKBC))
		}

		Name (_CRS, ResourceTemplate()
		{
			IO (Decode16, 0x60, 0x60, 0x01, 0x01)
			IO (Decode16, 0x64, 0x64, 0x01, 0x01)
			IRQNoFlags () {1}
		})
	}
#endif

#ifdef SIO_ENABLE_PS2M
	Device (PS2M)		// Mouse
	{
		Name (_HID, EISAID("PNP0F13"))

		Method (_STA, 0, NotSerialized) {
			Return (ISEN (SKBM))
		}

		Name (_CRS, ResourceTemplate()
		{
			IRQNoFlags () {12}
		})
	}
#endif

#ifdef SIO_ENABLE_FDC0
	Device (FDC0)		// Floppy controller
	{
		Name (_HID, EISAID ("PNP0700"))

		Method (_STA, 0, NotSerialized) {
			Return (ISEN (SFDC))
		}

		Name (_CRS, ResourceTemplate()
		{
			IO (Decode16, 0x03F0, 0x03F0, 0x01, 0x06)
			IO (Decode16, 0x03F7, 0x03F7, 0x01, 0x01)
			IRQNoFlags () {6}
			DMA (Compatibility, NotBusMaster, Transfer8) {2}
		})
	}
#endif

#ifdef SIO_ENABLE_INFR
	Device (INFR)		// Infrared controller
	{
		Name (_HID, EISAID ("PNP0510"))

		Method (_STA, 0, NotSerialized) {
			Return (ISEN (SINF))
		}

		Name (_CRS, ResourceTemplate()
		{
			IO (Decode16, SIO_INFR_IO0, SIO_INFR_IO0, 0x08, 0x08)
			IRQNoFlags () { SIO_INFR_IRQ }
		})
	}
#endif
}
