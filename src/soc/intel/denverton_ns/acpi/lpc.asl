/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 - 2009 coresystems GmbH
 * Copyright (C) 2014 - 2017 Intel Corporation.
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
 */

// Intel LPC Bus Device  - 0:1f.0

Device (LPCB)
{
	Name(_ADR, 0x001f0000)

	OperationRegion(LPC0, PCI_Config, 0x00, 0x100)
	Field (LPC0, AnyAcc, NoLock, Preserve)
	{
		Offset (0x80),	// IO Decode Ranges
		IOD0,	8,
		IOD1,	8,
	}

	#include "irqlinks.asl"

	Device(APIC)	// IO APIC
	{
		Name(_HID,EISAID("PNP0003"))
		Name(_CRS, ResourceTemplate()
		{
			Memory32Fixed(ReadOnly, 0xFEC00000, 0x1000)
		})
	}

	Device (HPET)
	{
		Name (_HID, EISAID("PNP0103"))
		Name (_CID, 0x010CD041)

		Method (_STA, 0)	// Device Status
		{
			Return (0xF)	// Enable and show device
		}

		Name(_CRS, ResourceTemplate()
		{
			Memory32Fixed(ReadOnly, DEFAULT_HPET_ADDR, 0x400)
		})
	}

	Device(PIC)	// 8259 Interrupt Controller
	{
		Name(_HID,EISAID("PNP0000"))
		Name(_CRS, ResourceTemplate()
		{
			IO (Decode16, 0x20, 0x20, 0x01, 0x02)
			IO (Decode16, 0x24, 0x24, 0x01, 0x02)
			IO (Decode16, 0x28, 0x28, 0x01, 0x02)
			IO (Decode16, 0x2c, 0x2c, 0x01, 0x02)
			IO (Decode16, 0x30, 0x30, 0x01, 0x02)
			IO (Decode16, 0x34, 0x34, 0x01, 0x02)
			IO (Decode16, 0x38, 0x38, 0x01, 0x02)
			IO (Decode16, 0x3c, 0x3c, 0x01, 0x02)
			IO (Decode16, 0xa0, 0xa0, 0x01, 0x02)
			IO (Decode16, 0xa4, 0xa4, 0x01, 0x02)
			IO (Decode16, 0xa8, 0xa8, 0x01, 0x02)
			IO (Decode16, 0xac, 0xac, 0x01, 0x02)
			IO (Decode16, 0xb0, 0xb0, 0x01, 0x02)
			IO (Decode16, 0xb4, 0xb4, 0x01, 0x02)
			IO (Decode16, 0xb8, 0xb8, 0x01, 0x02)
			IO (Decode16, 0xbc, 0xbc, 0x01, 0x02)
			IO (Decode16, 0x4d0, 0x4d0, 0x01, 0x02)
			IRQNoFlags () { 2 }
		})
	}

	Device(LDRC)	// LPC device: Resource consumption
	{
		Name (_HID, EISAID("PNP0C02"))
		Name (_UID, 2)
		Name (_CRS, ResourceTemplate()
		{
			IO (Decode16, 0x2e, 0x2e, 0x1, 0x02)		// First SuperIO
			IO (Decode16, 0x4e, 0x4e, 0x1, 0x02)		// Second SuperIO
			IO (Decode16, 0x61, 0x61, 0x1, 0x01)		// NMI Status
			IO (Decode16, 0x63, 0x63, 0x1, 0x01)		// CPU Reserved
			IO (Decode16, 0x65, 0x65, 0x1, 0x01)		// CPU Reserved
			IO (Decode16, 0x67, 0x67, 0x1, 0x01)		// CPU Reserved
			IO (Decode16, 0x70, 0x70, 0x1, 0x01)		// NMI Enable.
			IO (Decode16, 0x80, 0x80, 0x1, 0x01)		// Port 80 Post
			IO (Decode16, 0x92, 0x92, 0x1, 0x01)		// CPU Reserved
			IO (Decode16, 0xb2, 0xb2, 0x1, 0x02)		// SWSMI
			//IO (Decode16, 0x800, 0x800, 0x1, 0x10)		// ACPI I/O trap

			// BIOS ROM shadow memory range
			Memory32Fixed(ReadOnly, 0x000E0000, 0x20000)

			// BIOS flash 16MB
			Memory32Fixed(ReadOnly,0xFF000000,0x1000000)
		})
	}

	Device (RTC)	// Real Time Clock
	{
		Name (_HID, EISAID("PNP0B00"))
		Name (_CRS, ResourceTemplate()
		{
			IO (Decode16, 0x70, 0x70, 1, 8)
// Disable as Windows doesn't like it, and systems don't seem to use it.
//			IRQNoFlags() { 8 }
		})
	}

	Device (TIMR)	// Intel 8254 timer
	{
		Name(_HID, EISAID("PNP0100"))
		Name(_CRS, ResourceTemplate()
		{
			IO (Decode16, 0x40, 0x40, 0x01, 0x04)
			IO (Decode16, 0x50, 0x50, 0x10, 0x04)
			IRQNoFlags() {0}
		})
	}

	Device(IUR3) // Internal UART 1
	{
	  Name(_HID, EISAID("PNP0501"))

	  Name(_UID,1)

	  // Status Method for internal UART 1.

	  Method(_STA,0,Serialized)
	  {
		Return(0x000F)
	  }

	  // Current Resource Setting Method for internal UART 1.

	  Method(_CRS,0,Serialized)
	  {
		// Create the Buffer that stores the Resources to
		// be returned.

		Name(BUF0,ResourceTemplate()
		{
		  IO(Decode16,0x03F8,0x03F8,0x01,0x08)
		  Interrupt (ResourceConsumer, Level, ActiveLow, Shared) {16}
		})

		Return(BUF0)
	  }
	}

	Device(IUR4) // Internal UART 2
	{
	  Name(_HID, EISAID("PNP0501"))
	  Name(_UID,2)
	  // Status Method for internal UART 2.
	  Method(_STA,0,Serialized)
	  {
		Return(0x000F)
	  }
	  // Current Resource Setting Method for internal UART 2.
	  Method(_CRS,0,Serialized)
	  {
		// Create the Buffer that stores the Resources to
		// be returned.
		Name(BUF0,ResourceTemplate()
		{
		  IO(Decode16,0x02F8,0x02F8,0x01,0x08)
		  Interrupt (ResourceConsumer, Level, ActiveLow, Shared) {17}
		})
		Return(BUF0)
	  }
	}

	Device(IUR5) // Internal UART 3
	{
	  Name(_HID, EISAID("PNP0501"))
	  Name(_UID,3)
	  // Status Method for internal UART 3.
	  Method(_STA,0,Serialized)
	  {
		Return(0x000F)
	  }
	  // Current Resource Setting Method for internal UART 3.
	  Method(_CRS,0,Serialized)
	  {
		// Create the Buffer that stores the Resources to
		// be returned.
		Name(BUF0,ResourceTemplate()
		{
		  IO(Decode16,0x03E8,0x03E8,0x01,0x08)
		  Interrupt (ResourceConsumer, Level, ActiveLow, Shared) {18}
		})
		Return(BUF0)
	  }
	}
}
