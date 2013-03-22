/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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

// Intel LynxPoint Serial IO Devices in ACPI Mode

// Serial IO Device BAR0 and BAR1 is 4KB
#define SIO_BAR_LEN 0x1000

// Serial IO Resource Consumption for BAR1
Device (SIOR)
{
	Name (_HID, EISAID("PNP0C02"))
	Name (_UID, 4)

	Name (RBUF, ResourceTemplate()
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
		If (LNotEqual (\S0B1, Zero)) {
			CreateDwordField (^RBUF, ^B1D0._BAS, B0AD)
			CreateDwordField (^RBUF, ^B1D0._LEN, B0LN)
			Store (\S0B1, B0AD)
			Store (SIO_BAR_LEN, B0LN)
		}

		// I2C0
		If (LNotEqual (\S1B1, Zero)) {
			CreateDwordField (^RBUF, ^B1D1._BAS, B1AD)
			CreateDwordField (^RBUF, ^B1D1._LEN, B1LN)
			Store (\S1B1, B1AD)
			Store (SIO_BAR_LEN, B1LN)
		}

		// I2C1
		If (LNotEqual (\S2B1, Zero)) {
			CreateDwordField (^RBUF, ^B1D2._BAS, B2AD)
			CreateDwordField (^RBUF, ^B1D2._LEN, B2LN)
			Store (\S2B1, B2AD)
			Store (SIO_BAR_LEN, B2LN)
		}

		// SPI0
		If (LNotEqual (\S3B1, Zero)) {
			CreateDwordField (^RBUF, ^B1D3._BAS, B3AD)
			CreateDwordField (^RBUF, ^B1D3._LEN, B3LN)
			Store (\S3B1, B3AD)
			Store (SIO_BAR_LEN, B3LN)
		}

		// SPI1
		If (LNotEqual (\S4B1, Zero)) {
			CreateDwordField (^RBUF, ^B1D4._BAS, B4AD)
			CreateDwordField (^RBUF, ^B1D4._LEN, B4LN)
			Store (\S4B1, B4AD)
			Store (SIO_BAR_LEN, B4LN)
		}

		// UART0
		If (LNotEqual (\S5B1, Zero)) {
			CreateDwordField (^RBUF, ^B1D5._BAS, B5AD)
			CreateDwordField (^RBUF, ^B1D5._LEN, B5LN)
			Store (\S5B1, B5AD)
			Store (SIO_BAR_LEN, B5LN)
		}

		// UART1
		If (LNotEqual (\S6B1, Zero)) {
			CreateDwordField (^RBUF, ^B1D6._BAS, B6AD)
			CreateDwordField (^RBUF, ^B1D6._LEN, B6LN)
			Store (\S6B1, B6AD)
			Store (SIO_BAR_LEN, B6LN)
		}

		// SDIO
		If (LNotEqual (\S7B1, Zero)) {
			CreateDwordField (^RBUF, ^B1D7._BAS, B7AD)
			CreateDwordField (^RBUF, ^B1D7._LEN, B7LN)
			Store (\S7B1, B7AD)
			Store (SIO_BAR_LEN, B7LN)
		}

		Return (RBUF)
	}
}

Device (SDMA)
{
	// Serial IO DMA Controller
	Name (_HID, "INTL9C60")
	Name (_UID, 1)
	Name (_ADR, 0x00150000)

	// BAR0 is assigned during PCI enumeration and saved into NVS
	Name (RBUF, ResourceTemplate ()
	{
		Memory32Fixed (ReadWrite, 0x00000000, 0x00000000, BAR0)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , ) {7}
	})

	Method (_CRS, 0, NotSerialized)
	{
		// Update BAR0 address and length if set in NVS
		If (LNotEqual (\S0B0, Zero)) {
			CreateDwordField (^RBUF, ^BAR0._BAS, B0AD)
			CreateDwordField (^RBUF, ^BAR0._LEN, B0LN)
			Store (\S0B0, B0AD)
			Store (SIO_BAR_LEN, B0LN)
		}

		Return (RBUF)
	}

	Method (_STA, 0, NotSerialized)
	{
		If (LEqual (\S0B0, 0)) {
			Return (0x0)
		} Else {
			Return (0xF)
		}
	}
}

Device (I2C0)
{
	// Serial IO I2C0 Controller
	Name (_HID, "INT33C2")
	Name (_CID, "INT33C2")
	Name (_UID, 1)
	Name (_ADR, 0x00150001)

	// BAR0 is assigned during PCI enumeration and saved into NVS
	Name (RBUF, ResourceTemplate ()
	{
		Memory32Fixed (ReadWrite, 0x00000000, 0x00000000, BAR0)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , ) {7}
	})

	// DMA channels are only used if Serial IO DMA controller is enabled
	Name (DBUF, ResourceTemplate ()
	{
		// TODO: Need to update IASL to support FixedDMA
		//FixedDMA (0x18, 4, Width32Bit, DMA1) // Tx
		//FixedDMA (0x19, 5, Width32Bit, DMA2) // Rx
	})

	Method (_CRS, 0, NotSerialized)
	{
		// Update BAR0 address and length if set in NVS
		If (LNotEqual (\S1B0, Zero)) {
			CreateDwordField (^RBUF, ^BAR0._BAS, B0AD)
			CreateDwordField (^RBUF, ^BAR0._LEN, B0LN)
			Store (\S1B0, B0AD)
			Store (SIO_BAR_LEN, B0LN)
		}

		// Check if Serial IO DMA Controller is enabled
		If (LNotEqual (\_SB.PCI0.SDMA._STA, Zero)) {
			Return (ConcatenateResTemplate (RBUF, DBUF))
		} Else {
			Return (RBUF)
		}
	}

	Method (_STA, 0, NotSerialized)
	{
		If (LEqual (\S1B0, 0)) {
			Return (0x0)
		} Else {
			Return (0xF)
		}
	}
}

Device (I2C1)
{
	// Serial IO I2C1 Controller
	Name (_HID, "INT33C3")
	Name (_CID, "INT33C3")
	Name (_UID, 1)
	Name (_ADR, 0x00150002)

	// BAR0 is assigned during PCI enumeration and saved into NVS
	Name (RBUF, ResourceTemplate ()
	{
		Memory32Fixed (ReadWrite, 0x00000000, 0x00000000, BAR0)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , ) {7}
	})

	// DMA channels are only used if Serial IO DMA controller is enabled
	Name (DBUF, ResourceTemplate ()
	{
		// TODO: Need to update IASL to support FixedDMA
		//FixedDMA (0x1A, 6, Width32Bit, DMA1) // Tx
		//FixedDMA (0x1B, 7, Width32Bit, DMA2) // Rx
	})

	Method (_CRS, 0, NotSerialized)
	{
		// Update BAR0 address and length if set in NVS
		If (LNotEqual (\S2B0, Zero)) {
			CreateDwordField (^RBUF, ^BAR0._BAS, B0AD)
			CreateDwordField (^RBUF, ^BAR0._LEN, B0LN)
			Store (\S2B0, B0AD)
			Store (SIO_BAR_LEN, B0LN)
		}

		// Check if Serial IO DMA Controller is enabled
		If (LNotEqual (\_SB.PCI0.SDMA._STA, Zero)) {
			Return (ConcatenateResTemplate (RBUF, DBUF))
		} Else {
			Return (RBUF)
		}
	}

	Method (_STA, 0, NotSerialized)
	{
		If (LEqual (\S2B0, 0)) {
			Return (0x0)
		} Else {
			Return (0xF)
		}
	}
}

Device (SPI0)
{
	// Serial IO SPI0 Controller
	Name (_HID, "INT33C0")
	Name (_CID, "INT33C0")
	Name (_UID, 1)
	Name (_ADR, 0x00150003)

	// BAR0 is assigned during PCI enumeration and saved into NVS
	Name (RBUF, ResourceTemplate ()
	{
		Memory32Fixed (ReadWrite, 0x00000000, 0x00000000, BAR0)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , ) {7}
	})

	Method (_CRS, 0, NotSerialized)
	{
		// Update BAR0 address and length if set in NVS
		If (LNotEqual (\S3B0, Zero)) {
			CreateDwordField (^RBUF, ^BAR0._BAS, B0AD)
			CreateDwordField (^RBUF, ^BAR0._LEN, B0LN)
			Store (\S3B0, B0AD)
			Store (SIO_BAR_LEN, B0LN)
		}

		Return (RBUF)
	}

	Method (_STA, 0, NotSerialized)
	{
		If (LEqual (\S3B0, 0)) {
			Return (0x0)
		} Else {
			Return (0xF)
		}
	}
}

Device (SPI1)
{
	// Serial IO SPI1 Controller
	Name (_HID, "INT33C1")
	Name (_CID, "INT33C1")
	Name (_UID, 1)
	Name (_ADR, 0x00150004)

	// BAR0 is assigned during PCI enumeration and saved into NVS
	Name (RBUF, ResourceTemplate ()
	{
		Memory32Fixed (ReadWrite, 0x00000000, 0x00000000, BAR0)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , ) {7}
	})

	// DMA channels are only used if Serial IO DMA controller is enabled
	Name (DBUF, ResourceTemplate ()
	{
		// TODO: Need to update IASL to support FixedDMA
		//FixedDMA (0x10, 0, Width32Bit, DMA1) // Tx
		//FixedDMA (0x11, 1, Width32Bit, DMA2) // Rx
	})

	Method (_CRS, 0, NotSerialized)
	{
		// Update BAR0 address and length if set in NVS
		If (LNotEqual (\S4B0, Zero)) {
			CreateDwordField (^RBUF, ^BAR0._BAS, B0AD)
			CreateDwordField (^RBUF, ^BAR0._LEN, B0LN)
			Store (\S4B0, B0AD)
			Store (SIO_BAR_LEN, B0LN)
		}

		// Check if Serial IO DMA Controller is enabled
		If (LNotEqual (\_SB.PCI0.SDMA._STA, Zero)) {
			Return (ConcatenateResTemplate (RBUF, DBUF))
		} Else {
			Return (RBUF)
		}
	}

	Method (_STA, 0, NotSerialized)
	{
		If (LEqual (\S4B0, 0)) {
			Return (0x0)
		} Else {
			Return (0xF)
		}
	}
}

Device (UAR0)
{
	// Serial IO UART0 Controller
	Name (_HID, "INT33C4")
	Name (_CID, "INT33C4")
	Name (_UID, 1)
	Name (_ADR, 0x00150005)

	// BAR0 is assigned during PCI enumeration and saved into NVS
	Name (RBUF, ResourceTemplate ()
	{
		Memory32Fixed (ReadWrite, 0x00000000, 0x00000000, BAR0)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , ) {13}
	})

	// DMA channels are only used if Serial IO DMA controller is enabled
	Name (DBUF, ResourceTemplate ()
	{
		// TODO: Need to update IASL to support FixedDMA
		//FixedDMA (0x16, 2, Width32Bit, DMA1) // Tx
		//FixedDMA (0x17, 3, Width32Bit, DMA2) // Rx
	})

	Method (_CRS, 0, NotSerialized)
	{
		// Update BAR0 address and length if set in NVS
		If (LNotEqual (\S5B0, Zero)) {
			CreateDwordField (^RBUF, ^BAR0._BAS, B0AD)
			CreateDwordField (^RBUF, ^BAR0._LEN, B0LN)
			Store (\S5B0, B0AD)
			Store (SIO_BAR_LEN, B0LN)
		}

		// Check if Serial IO DMA Controller is enabled
		If (LNotEqual (\_SB.PCI0.SDMA._STA, Zero)) {
			Return (ConcatenateResTemplate (RBUF, DBUF))
		} Else {
			Return (RBUF)
		}
	}

	Method (_STA, 0, NotSerialized)
	{
		If (LEqual (\S5B0, 0)) {
			Return (0x0)
		} Else {
			Return (0xF)
		}
	}
}

Device (UAR1)
{
	// Serial IO UART1 Controller
	Name (_HID, "INT33C5")
	Name (_CID, "INT33C5")
	Name (_UID, 1)
	Name (_ADR, 0x00150006)

	// BAR0 is assigned during PCI enumeration and saved into NVS
	Name (RBUF, ResourceTemplate ()
	{
		Memory32Fixed (ReadWrite, 0x00000000, 0x00000000, BAR0)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , ) {13}
	})

	Method (_CRS, 0, NotSerialized)
	{
		// Update BAR0 address and length if set in NVS
		If (LNotEqual (\S6B0, Zero)) {
			CreateDwordField (^RBUF, ^BAR0._BAS, B0AD)
			CreateDwordField (^RBUF, ^BAR0._LEN, B0LN)
			Store (\S6B0, B0AD)
			Store (SIO_BAR_LEN, B0LN)
		}

		Return (RBUF)
	}

	Method (_STA, 0, NotSerialized)
	{
		If (LEqual (\S6B0, 0)) {
			Return (0x0)
		} Else {
			Return (0xF)
		}
	}
}

Device (SDIO)
{
	// Serial IO SDIO Controller
	Name (_HID, "INT33C6")
	Name (_CID, "PNP0D40")
	Name (_UID, 1)
	Name (_ADR, 0x00170000)

	// BAR0 is assigned during PCI enumeration and saved into NVS
	Name (RBUF, ResourceTemplate ()
	{
		Memory32Fixed (ReadWrite, 0x00000000, 0x00000000, BAR0)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , ) {5}
	})

	Method (_CRS, 0, NotSerialized)
	{
		// Update BAR0 address and length if set in NVS
		If (LNotEqual (\S7B0, Zero)) {
			CreateDwordField (^RBUF, ^BAR0._BAS, B0AD)
			CreateDwordField (^RBUF, ^BAR0._LEN, B0LN)
			Store (\S7B0, B0AD)
			Store (SIO_BAR_LEN, B0LN)
		}

		Return (RBUF)
	}

	Method (_STA, 0, NotSerialized)
	{
		If (LEqual (\S7B0, 0)) {
			Return (0x0)
		} Else {
			Return (0xF)
		}
	}
}
