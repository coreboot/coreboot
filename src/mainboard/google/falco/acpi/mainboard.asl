/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
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
 */

#include <mainboard/google/falco/onboard.h>

Scope (\_SB)
{
	Device (LID0)
	{
		Name(_HID, EisaId("PNP0C0D"))
		Method(_LID, 0)
		{
			Store (\_SB.PCI0.LPCB.EC0.LIDS, \LIDS)
			Return (\LIDS)
		}
	}

	Device (PWRB)
	{
		Name(_HID, EisaId("PNP0C0C"))
	}

	Device (TPAD)
	{
		Name (_ADR, 0x0)
		Name (_UID, 1)

		// Report as a Sleep Button device so Linux will
		// automatically enable it as a wake source
		Name (_HID, EisaId("PNP0C0E"))

		Name (_CRS, ResourceTemplate()
		{
			Interrupt (ResourceConsumer, Edge, ActiveLow)
			{
				BOARD_TRACKPAD_IRQ
			}

			VendorShort (ADDR)
			{
				BOARD_TRACKPAD_I2C_ADDR
			}
		})

		Name (_PRW, Package() { BOARD_TRACKPAD_WAKE_GPIO, 0x3 })

		Method (_DSW, 3, NotSerialized)
		{
			Store (BOARD_TRACKPAD_WAKE_GPIO, Local0)

			If (LEqual (Arg0, 1)) {
				// Enable GPIO as wake source
				\_SB.PCI0.LPCB.GWAK (Local0)
			}
		}
	}

	Device (TSCR)
	{
		Name (_ADR, 0x0)
		Name (_UID, 2)

		// Report as a Sleep Button device so Linux will
		// automatically enable it as a wake source
		Name (_HID, EisaId("PNP0C0E"))

		Name (_CRS, ResourceTemplate()
		{
			Interrupt (ResourceConsumer, Edge, ActiveLow)
			{
				BOARD_TOUCHSCREEN_IRQ
			}

			VendorShort (ADDR)
			{
				BOARD_TOUCHSCREEN_I2C_ADDR
			}
		})

		Name (_PRW, Package() { BOARD_TOUCHSCREEN_WAKE_GPIO, 0x3 })

		Method (_DSW, 3, NotSerialized)
		{
			Store (BOARD_TOUCHSCREEN_WAKE_GPIO, Local0)

			If (LEqual (Arg0, 1)) {
				// Enable GPIO as wake source
				\_SB.PCI0.LPCB.GWAK (Local0)
			}
		}
	}
}

Scope (\_SB.PCI0.I2C0)
{
	Device (CYPA)
	{
		Name (_HID, "CYPA0000")
		Name (_DDN, "Cypress Touchpad")
		Name (_UID, 1)

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				BOARD_TRACKPAD_I2C_ADDR,  // SlaveAddress
				ControllerInitiated,      // SlaveMode
				400000,                   // ConnectionSpeed
				AddressingMode7Bit,       // AddressingMode
				"\\_SB.PCI0.I2C0",        // ResourceSource
			)
			Interrupt (ResourceConsumer, Edge, ActiveLow)
			{
				BOARD_TRACKPAD_IRQ
			}
		})

		Method (_STA)
		{
			If (LEqual (\S1EN, 1)) {
				Return (0xF)
			} Else {
				Return (0x0)
			}
		}
	}
}
