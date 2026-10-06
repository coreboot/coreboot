/* SPDX-License-Identifier: GPL-2.0-only */

// Intel Serial IO Devices in ACPI Mode

// Serial IO Device BAR0 and BAR1 is 4KB
#define SIO_BAR_LEN 0x1000

// Serial IO DMA request lines and channels, empty if the device has no DMA
#define SIO_DMA(tx_line, tx_chan, rx_line, rx_chan)		\
	FixedDMA (tx_line, tx_chan, Width32Bit) /* Tx */	\
	FixedDMA (rx_line, rx_chan, Width32Bit) /* Rx */

#if CONFIG(SOUTHBRIDGE_INTEL_WILDCATPOINT)
#define SIO_DMA_I2C0
#define SIO_DMA_I2C1
#define SIO_DMA_SPI0	SIO_DMA (2, 2, 3, 3)
#define SIO_DMA_SPI1	SIO_DMA (0, 0, 1, 1)
#define SIO_DMA_UAR0	SIO_DMA (4, 4, 5, 5)
#define SIO_DMA_UAR1	SIO_DMA (6, 6, 7, 7)
#else
#define SIO_DMA_I2C0	SIO_DMA (0x18, 4, 0x19, 5)
#define SIO_DMA_I2C1	SIO_DMA (0x1A, 6, 0x1B, 7)
#define SIO_DMA_SPI0
#define SIO_DMA_SPI1	SIO_DMA (0x10, 0, 0x11, 1)
#define SIO_DMA_UAR0	SIO_DMA (0x16, 2, 0x17, 3)
#define SIO_DMA_UAR1
#endif

// TODO: Replace this with some HAVE_DEVICE_NVS Kconfig
#if !CONFIG(SOUTHBRIDGE_INTEL_WILDCATPOINT)
// This is defined in SSDT2 which is generated at boot based
// on whether or not the device is enabled in ACPI mode.
External (\S0EN)
External (\S1EN)
External (\S2EN)
External (\S3EN)
External (\S4EN)
External (\S5EN)
External (\S6EN)
External (\S7EN)
#endif

// Put SerialIO device in D0 state
// Arg0 - Ref to offset 0x84 of device's PCI config space
Method (LPD0, 1, Serialized)
{
	Arg0 = DeRefOf (Arg0) & 0xFFFFFFFC
	Local0 = DeRefOf (Arg0) // Read back after writing

	// Use Local0 to avoid iasl warning: Method Local is set but never used
	Local0 &= Ones
}

// Put SerialIO device in D3 state
// Arg0 - Ref to offset 0x84 of device's PCI config space
Method (LPD3, 1, Serialized)
{
	Arg0 = DeRefOf (Arg0) | 0x3
	Local0 = DeRefOf (Arg0) // Read back after writing

	// Use Local0 to avoid iasl warning: Method Local is set but never used
	Local0 &= Ones
}

// Serial IO Resource Consumption for BAR1
Device (SIOR)
{
	Name (_HID, EISAID ("PNP0C02"))
	Name (_UID, 4)

	Name (RBUF, ResourceTemplate ()
	{
		// Serial IO BAR1 (PCI config space) resources
		Memory32Fixed (ReadWrite, 0x00000000, 0x00000000, B1D0) // SDMA
		Memory32Fixed (ReadWrite, 0x00000000, 0x00000000, B1D1) // I2C0
		Memory32Fixed (ReadWrite, 0x00000000, 0x00000000, B1D2) // I2C1
		Memory32Fixed (ReadWrite, 0x00000000, 0x00000000, B1D3) // SPI0
		Memory32Fixed (ReadWrite, 0x00000000, 0x00000000, B1D4) // SPI1
		Memory32Fixed (ReadWrite, 0x00000000, 0x00000000, B1D5) // UART0
		Memory32Fixed (ReadWrite, 0x00000000, 0x00000000, B1D6) // UART1
		Memory32Fixed (ReadWrite, 0x00000000, 0x00000000, B1D7) // SDIO
	})

	// Update BAR1 address and length if set in NVS
	Method (_CRS, 0, NotSerialized)
	{
		// SDMA
		If (\S0B1 != 0) {
			CreateDwordField (^RBUF, ^B1D0._BAS, B0AD)
			CreateDwordField (^RBUF, ^B1D0._LEN, B0LN)
			B0AD = \S0B1
			B0LN = SIO_BAR_LEN
		}

		// I2C0
		If (\S1B1 != 0) {
			CreateDwordField (^RBUF, ^B1D1._BAS, B1AD)
			CreateDwordField (^RBUF, ^B1D1._LEN, B1LN)
			B1AD = \S1B1
			B1LN = SIO_BAR_LEN
		}

		// I2C1
		If (\S2B1 != 0) {
			CreateDwordField (^RBUF, ^B1D2._BAS, B2AD)
			CreateDwordField (^RBUF, ^B1D2._LEN, B2LN)
			B2AD = \S2B1
			B2LN = SIO_BAR_LEN
		}

		// SPI0
		If (\S3B1 != 0) {
			CreateDwordField (^RBUF, ^B1D3._BAS, B3AD)
			CreateDwordField (^RBUF, ^B1D3._LEN, B3LN)
			B3AD = \S3B1
			B3LN = SIO_BAR_LEN
		}

		// SPI1
		If (\S4B1 != 0) {
			CreateDwordField (^RBUF, ^B1D4._BAS, B4AD)
			CreateDwordField (^RBUF, ^B1D4._LEN, B4LN)
			B4AD = \S4B1
			B4LN = SIO_BAR_LEN
		}

		// UART0
		If (\S5B1 != 0) {
			CreateDwordField (^RBUF, ^B1D5._BAS, B5AD)
			CreateDwordField (^RBUF, ^B1D5._LEN, B5LN)
			B5AD = \S5B1
			B5LN = SIO_BAR_LEN
		}

		// UART1
		If (\S6B1 != 0) {
			CreateDwordField (^RBUF, ^B1D6._BAS, B6AD)
			CreateDwordField (^RBUF, ^B1D6._LEN, B6LN)
			B6AD = \S6B1
			B6LN = SIO_BAR_LEN
		}

		// SDIO
		If (\S7B1 != 0) {
			CreateDwordField (^RBUF, ^B1D7._BAS, B7AD)
			CreateDwordField (^RBUF, ^B1D7._LEN, B7LN)
			B7AD = \S7B1
			B7LN = SIO_BAR_LEN
		}

		Return (RBUF)
	}
}

Device (SDMA)
{
	// Serial IO DMA Controller
	Name (_HID, "INTL9C60")
	Name (_UID, 1)

	// BAR0 is assigned during PCI enumeration and saved into NVS
	Name (RBUF, ResourceTemplate ()
	{
		Memory32Fixed (ReadWrite, 0x00000000, 0x00000000, BAR0)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , ) {6}
	})

	Method (_CRS, 0, NotSerialized)
	{
		// Update BAR0 address and length if set in NVS
		If (\S0B0 != 0) {
			CreateDwordField (^RBUF, ^BAR0._BAS, B0AD)
			CreateDwordField (^RBUF, ^BAR0._LEN, B0LN)
			B0AD = \S0B0
			B0LN = SIO_BAR_LEN
		}

		Return (RBUF)
	}

	Method (_STA, 0, NotSerialized)
	{
		If (\S0EN == 0) {
			Return (0x0)
		} Else {
			Return (0xF)
		}
	}
}

Device (I2C0)
{
	// Serial IO I2C0 Controller
	Method (_HID)
	{
		If (\ISWP ()) {
			// WildcatPoint
			Return ("INT3432")
		}
		// LynxPoint-LP
		Return ("INT33C2")
	}
	Name (_CID, "INT33C2")
	Name (_UID, 1)

	Name (SSCN, Package () { 432, 507, 30 })
	Name (FMCN, Package () { 72, 160, 30 })

	// BAR0 is assigned during PCI enumeration and saved into NVS
	Name (RBUF, ResourceTemplate ()
	{
		Memory32Fixed (ReadWrite, 0x00000000, 0x00000000, BAR0)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , ) {7}
	})

	// DMA channels are only used if Serial IO DMA controller is enabled
	Name (DBUF, ResourceTemplate ()
	{
		SIO_DMA_I2C0
	})

	Method (_CRS, 0, NotSerialized)
	{
		// Update BAR0 address and length if set in NVS
		If (\S1B0 != 0) {
			CreateDwordField (^RBUF, ^BAR0._BAS, B0AD)
			CreateDwordField (^RBUF, ^BAR0._LEN, B0LN)
			B0AD = \S1B0
			B0LN = SIO_BAR_LEN
		}

		// Check if Serial IO DMA Controller is enabled
		If (\_SB.PCI0.SDMA._STA != 0) {
			Return (ConcatenateResTemplate (RBUF, DBUF))
		} Else {
			Return (RBUF)
		}
	}

	Method (_STA, 0, NotSerialized)
	{
		If (\S1EN == 0) {
			Return (0x0)
		} Else {
			Return (0xF)
		}
	}

	OperationRegion (SPRT, SystemMemory, \S1B1 + 0x84, 4)
	Field (SPRT, DWordAcc, NoLock, Preserve)
	{
		SPCS, 32
	}

	Method (_PS0, 0, Serialized)
	{
		^^LPD0 (RefOf (SPCS))
	}

	Method (_PS3, 0, Serialized)
	{
		^^LPD3 (RefOf (SPCS))
	}
}

Device (I2C1)
{
	// Serial IO I2C1 Controller
	Method (_HID)
	{
		If (\ISWP ()) {
			// WildcatPoint
			Return ("INT3433")
		}
		// LynxPoint-LP
		Return ("INT33C3")
	}
	Name (_CID, "INT33C3")
	Name (_UID, 1)

	Name (SSCN, Package () { 432, 507, 30 })
	Name (FMCN, Package () { 72, 160, 30 })

	// BAR0 is assigned during PCI enumeration and saved into NVS
	Name (RBUF, ResourceTemplate ()
	{
		Memory32Fixed (ReadWrite, 0x00000000, 0x00000000, BAR0)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , ) {7}
	})

	// DMA channels are only used if Serial IO DMA controller is enabled
	Name (DBUF, ResourceTemplate ()
	{
		SIO_DMA_I2C1
	})

	Method (_CRS, 0, NotSerialized)
	{
		// Update BAR0 address and length if set in NVS
		If (\S2B0 != 0) {
			CreateDwordField (^RBUF, ^BAR0._BAS, B0AD)
			CreateDwordField (^RBUF, ^BAR0._LEN, B0LN)
			B0AD = \S2B0
			B0LN = SIO_BAR_LEN
		}

		// Check if Serial IO DMA Controller is enabled
		If (\_SB.PCI0.SDMA._STA != 0) {
			Return (ConcatenateResTemplate (RBUF, DBUF))
		} Else {
			Return (RBUF)
		}
	}

	Method (_STA, 0, NotSerialized)
	{
		If (\S2EN == 0) {
			Return (0x0)
		} Else {
			Return (0xF)
		}
	}

	OperationRegion (SPRT, SystemMemory, \S2B1 + 0x84, 4)
	Field (SPRT, DWordAcc, NoLock, Preserve)
	{
		SPCS, 32
	}

	Method (_PS0, 0, Serialized)
	{
		^^LPD0 (RefOf (SPCS))
	}

	Method (_PS3, 0, Serialized)
	{
		^^LPD3 (RefOf (SPCS))
	}
}

Device (SPI0)
{
	// Serial IO SPI0 Controller
	Method (_HID)
	{
		If (\ISWP ()) {
			// WildcatPoint
			Return ("INT3430")
		}
		// LynxPoint-LP
		Return ("INT33C0")
	}
	Name (_CID, "INT33C0")
	Name (_UID, 1)

	// BAR0 is assigned during PCI enumeration and saved into NVS
	Name (RBUF, ResourceTemplate ()
	{
		Memory32Fixed (ReadWrite, 0x00000000, 0x00000000, BAR0)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , ) {7}
	})

	// DMA channels are only used if Serial IO DMA controller is enabled
	Name (DBUF, ResourceTemplate ()
	{
		SIO_DMA_SPI0
	})

	Method (_CRS, 0, NotSerialized)
	{
		// Update BAR0 address and length if set in NVS
		If (\S3B0 != 0) {
			CreateDwordField (^RBUF, ^BAR0._BAS, B0AD)
			CreateDwordField (^RBUF, ^BAR0._LEN, B0LN)
			B0AD = \S3B0
			B0LN = SIO_BAR_LEN
		}

		// Check if Serial IO DMA Controller is enabled
		If (\_SB.PCI0.SDMA._STA != 0) {
			Return (ConcatenateResTemplate (RBUF, DBUF))
		} Else {
			Return (RBUF)
		}
	}

	Method (_STA, 0, NotSerialized)
	{
		If (\S3EN == 0) {
			Return (0x0)
		} Else {
			Return (0xF)
		}
	}

	OperationRegion (SPRT, SystemMemory, \S3B1 + 0x84, 4)
	Field (SPRT, DWordAcc, NoLock, Preserve)
	{
		SPCS, 32
	}

	Method (_PS0, 0, Serialized)
	{
		^^LPD0 (RefOf (SPCS))
	}

	Method (_PS3, 0, Serialized)
	{
		^^LPD3 (RefOf (SPCS))
	}
}

Device (SPI1)
{
	// Serial IO SPI1 Controller
	Method (_HID)
	{
		If (\ISWP ()) {
			// WildcatPoint
			Return ("INT3431")
		}
		// LynxPoint-LP
		Return ("INT33C1")
	}
	Name (_CID, "INT33C1")
	Name (_UID, 1)

	// BAR0 is assigned during PCI enumeration and saved into NVS
	Name (RBUF, ResourceTemplate ()
	{
		Memory32Fixed (ReadWrite, 0x00000000, 0x00000000, BAR0)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , ) {7}
	})

	// DMA channels are only used if Serial IO DMA controller is enabled
	Name (DBUF, ResourceTemplate ()
	{
		SIO_DMA_SPI1
	})

	Method (_CRS, 0, NotSerialized)
	{
		// Update BAR0 address and length if set in NVS
		If (\S4B0 != 0) {
			CreateDwordField (^RBUF, ^BAR0._BAS, B0AD)
			CreateDwordField (^RBUF, ^BAR0._LEN, B0LN)
			B0AD = \S4B0
			B0LN = SIO_BAR_LEN
		}

		// Check if Serial IO DMA Controller is enabled
		If (\_SB.PCI0.SDMA._STA != 0) {
			Return (ConcatenateResTemplate (RBUF, DBUF))
		} Else {
			Return (RBUF)
		}
	}

	Method (_STA, 0, NotSerialized)
	{
		If (\S4EN == 0) {
			Return (0x0)
		} Else {
			Return (0xF)
		}
	}

	OperationRegion (SPRT, SystemMemory, \S4B1 + 0x84, 4)
	Field (SPRT, DWordAcc, NoLock, Preserve)
	{
		SPCS, 32
	}

	Method (_PS0, 0, Serialized)
	{
		^^LPD0 (RefOf (SPCS))
	}

	Method (_PS3, 0, Serialized)
	{
		^^LPD3 (RefOf (SPCS))
	}
}

Device (UAR0)
{
	// Serial IO UART0 Controller
	Method (_HID)
	{
		If (\ISWP ()) {
			// WildcatPoint
			Return ("INT3434")
		}
		// LynxPoint-LP
		Return ("INT33C4")
	}
	Name (_CID, "INT33C4")
	Name (_UID, 1)

	// BAR0 is assigned during PCI enumeration and saved into NVS
	Name (RBUF, ResourceTemplate ()
	{
		Memory32Fixed (ReadWrite, 0x00000000, 0x00000000, BAR0)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , ) {13}
	})

	// DMA channels are only used if Serial IO DMA controller is enabled
	Name (DBUF, ResourceTemplate ()
	{
		SIO_DMA_UAR0
	})

	Method (_CRS, 0, NotSerialized)
	{
		// Update BAR0 address and length if set in NVS
		If (\S5B0 != 0) {
			CreateDwordField (^RBUF, ^BAR0._BAS, B0AD)
			CreateDwordField (^RBUF, ^BAR0._LEN, B0LN)
			B0AD = \S5B0
			B0LN = SIO_BAR_LEN
		}

		// Check if Serial IO DMA Controller is enabled
		If (\_SB.PCI0.SDMA._STA != 0) {
			Return (ConcatenateResTemplate (RBUF, DBUF))
		} Else {
			Return (RBUF)
		}
	}

	Method (_STA, 0, NotSerialized)
	{
		If (\S5EN == 0) {
			Return (0x0)
		} Else {
			Return (0xF)
		}
	}

	OperationRegion (SPRT, SystemMemory, \S5B1 + 0x84, 4)
	Field (SPRT, DWordAcc, NoLock, Preserve)
	{
		SPCS, 32
	}

	Method (_PS0, 0, Serialized)
	{
		^^LPD0 (RefOf (SPCS))
	}

	Method (_PS3, 0, Serialized)
	{
		^^LPD3 (RefOf (SPCS))
	}
}

Device (UAR1)
{
	// Serial IO UART1 Controller
	Method (_HID)
	{
		If (\ISWP ()) {
			// WildcatPoint
			Return ("INT3435")
		}
		// LynxPoint-LP
		Return ("INT33C5")
	}
	Name (_CID, "INT33C5")
	Name (_UID, 1)

	// BAR0 is assigned during PCI enumeration and saved into NVS
	Name (RBUF, ResourceTemplate ()
	{
		Memory32Fixed (ReadWrite, 0x00000000, 0x00000000, BAR0)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , ) {13}
	})

	// DMA channels are only used if Serial IO DMA controller is enabled
	Name (DBUF, ResourceTemplate ()
	{
		SIO_DMA_UAR1
	})

	Method (_CRS, 0, NotSerialized)
	{
		// Update BAR0 address and length if set in NVS
		If (\S6B0 != 0) {
			CreateDwordField (^RBUF, ^BAR0._BAS, B0AD)
			CreateDwordField (^RBUF, ^BAR0._LEN, B0LN)
			B0AD = \S6B0
			B0LN = SIO_BAR_LEN
		}

		// Check if Serial IO DMA Controller is enabled
		If (\_SB.PCI0.SDMA._STA != 0) {
			Return (ConcatenateResTemplate (RBUF, DBUF))
		} Else {
			Return (RBUF)
		}
	}

	Method (_STA, 0, NotSerialized)
	{
		If (\S6EN == 0) {
			Return (0x0)
		} Else {
			Return (0xF)
		}
	}

	OperationRegion (SPRT, SystemMemory, \S6B1 + 0x84, 4)
	Field (SPRT, DWordAcc, NoLock, Preserve)
	{
		SPCS, 32
	}

	Method (_PS0, 0, Serialized)
	{
		^^LPD0 (RefOf (SPCS))
	}

	Method (_PS3, 0, Serialized)
	{
		^^LPD3 (RefOf (SPCS))
	}
}

Device (SDIO)
{
	// Serial IO SDIO Controller
	Method (_HID)
	{
		If (\ISWP ()) {
			// WildcatPoint
			Return ("INT3436")
		}
		// LynxPoint-LP
		Return ("INT33C6")
	}
	Name (_CID, "PNP0D40")
	Name (_UID, 1)

	// BAR0 is assigned during PCI enumeration and saved into NVS
	Name (RBUF, ResourceTemplate ()
	{
		Memory32Fixed (ReadWrite, 0x00000000, 0x00000000, BAR0)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , ) {5}
	})

	Method (_CRS, 0, NotSerialized)
	{
		// Update BAR0 address and length if set in NVS
		If (\S7B0 != 0) {
			CreateDwordField (^RBUF, ^BAR0._BAS, B0AD)
			CreateDwordField (^RBUF, ^BAR0._LEN, B0LN)
			B0AD = \S7B0
			B0LN = SIO_BAR_LEN
		}

		Return (RBUF)
	}

	Method (_STA, 0, NotSerialized)
	{
		If (\S7EN == 0) {
			Return (0x0)
		} Else {
			Return (0xF)
		}
	}
}
