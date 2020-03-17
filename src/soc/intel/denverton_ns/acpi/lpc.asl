/*
 * This file is part of the coreboot project.
 *
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
#include <soc/intel/common/block/acpi/acpi/lpc.asl>

Scope (\_SB.PCI0.LPCB)
{
	#include "irqlinks.asl"

	OperationRegion(LPC0, PCI_Config, 0x00, 0x100)
	Field (LPC0, AnyAcc, NoLock, Preserve)
	{
		Offset (0x80),	// IO Decode Ranges
		IOD0,	8,
		IOD1,	8,
	}

	Device(APIC)	// IO APIC
	{
		Name(_HID,EISAID("PNP0003"))
		Name(_CRS, ResourceTemplate()
		{
			Memory32Fixed(ReadOnly, 0xFEC00000, 0x1000)
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
