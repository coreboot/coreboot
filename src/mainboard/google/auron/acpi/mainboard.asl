/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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

#include <mainboard/google/auron/onboard.h>

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


		// There is no GPIO for LID, the EC pulses WAKE# pin instead.
		// There is no GPE for WAKE#, so fake it with PCI_EXP_WAKE
		Name (_PRW, Package(){ 0x69, 5 }) // PCI_EXP
	}

	Device (PWRB)
	{
		Name(_HID, EisaId("PNP0C0C"))
	}
}

/*
 * LPC Trusted Platform Module
 */
Scope (\_SB.PCI0.LPCB)
	{
	#include <drivers/pc80/tpm/acpi/tpm.asl>
}

Scope (\_SB.PCI0.I2C0)
{
	Device (ETPA)
	{
		Name (_HID, "ELAN0000")
		Name (_DDN, "Elan Touchpad")
		Name (_UID, 1)
		Name (ISTP, 1) /* Touchpad */

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				0x15,                     // SlaveAddress
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

		Name (_PRW, Package() { BOARD_TRACKPAD_WAKE_GPIO, 0x3 })

		Method (_DSW, 3, NotSerialized)
		{
			Store (BOARD_TRACKPAD_WAKE_GPIO, Local0)
			If (LEqual (Arg0, 1)) {
				// Enable GPIO as wake source
				\_SB.PCI0.LPCB.GPIO.GWAK (Local0)
			}
		}

		/* Allow device to power off in S0 */
		Name (_S0W, 4)
	}

	Device (CTPA)
	{
		Name (_HID, "CYAP0000")
		Name (_DDN, "Cypress Touchpad")
		Name (_UID, 3)
		Name (ISTP, 1) /* Touchpad */

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				0x67,                     // SlaveAddress
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

		Name (_PRW, Package() { BOARD_TRACKPAD_WAKE_GPIO, 0x3 })

		Method (_DSW, 3, NotSerialized)
		{
			Store (BOARD_TRACKPAD_WAKE_GPIO, Local0)
			If (LEqual (Arg0, 1)) {
				// Enable GPIO as wake source
				\_SB.PCI0.LPCB.GPIO.GWAK (Local0)
			}
		}

		/* Allow device to power off in S0 */
		Name (_S0W, 4)
	}

	Device (CTPB)
	{
		Name (_HID, "CYAP0001")
		Name (_DDN, "Cypress Touchpad")
		Name (_UID, 3)
		Name (ISTP, 1) /* Touchpad */

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				0x24,                     // SlaveAddress
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

		Name (_PRW, Package() { BOARD_TRACKPAD_WAKE_GPIO, 0x3 })

		Method (_DSW, 3, NotSerialized)
		{
			Store (BOARD_TRACKPAD_WAKE_GPIO, Local0)
			If (LEqual (Arg0, 1)) {
				// Enable GPIO as wake source
				\_SB.PCI0.LPCB.GPIO.GWAK (Local0)
			}
		}

		/* Allow device to power off in S0 */
		Name (_S0W, 4)
	}
}
Scope (\_SB.PCI0.I2C1)
{
	Device (ATSA)
	{
		Name (_HID, "ATML0001")
		Name (_DDN, "Atmel Touchscreen")
		Name (_UID, 5)
		Name (_S0W, 4)
		Name (ISTP, 0) /* TouchScreen */

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				0x4a,                     // SlaveAddress
				ControllerInitiated,      // SlaveMode
				400000,                   // ConnectionSpeed
				AddressingMode7Bit,       // AddressingMode
				"\\_SB.PCI0.I2C1",        // ResourceSource
			)

			// GPIO54 (ball L3) is PIRQW: PIRQL_GSI + PIRQL - PIRQW = PIRQW_GSI
			// 27 + 3 - 14 = 38
			Interrupt (ResourceConsumer, Edge, ActiveLow) { 38 }
		})

		Name (_PRW, Package() { BOARD_TOUCHSCREEN_WAKE_GPIO, 0x3 })

		Method (_DSW, 3, NotSerialized)
		{
			Store (BOARD_TOUCHSCREEN_WAKE_GPIO, Local0)
			If (LEqual (Arg0, 1)) {
				// Enable GPIO as wake source
				\_SB.PCI0.LPCB.GPIO.GWAK (Local0)
			}
		}

		Method (_STA)
		{
			If (LEqual (\S2EN, 1)) {
				Return (0xF)
			} Else {
				Return (0x0)
			}
		}
	}

	Device (ALSI)
	{
		/*
		 * TODO(dlaurie): Need official HID.
		 *
		 * The current HID is created from the Intersil PNP
		 * Vendor ID "LSD" and a shortened device identifier.
		 */
		Name (_HID, EisaId ("LSD2918"))
		Name (_DDN, "Intersil 29018 Ambient Light Sensor")
		Name (_UID, 6)

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				0x44,                     // SlaveAddress
				ControllerInitiated,      // SlaveMode
				400000,                   // ConnectionSpeed
				AddressingMode7Bit,       // AddressingMode
				"\\_SB.I2C1",             // ResourceSource
			)

			// On Auron/Peppy board, IRQ is hooked to GPIO 51.
			// Based on table 5-36, this is PIRQT. Then based on
			// table 5-12, this is IRQ #35.
			Interrupt (ResourceConsumer, Edge, ActiveLow)
			{
				35
			}
		})

		Method (_STA)
		{
			If (LEqual (\S2EN, 1)) {
				Return (0xF)
			} Else {
				Return (0x0)
			}
		}
	}
}

Scope (\_SB.PCI0.RP01)
{
	Device (WLAN)
	{
		Name (_ADR, 0x00000000)

		/* GPIO10 is WLAN_WAKE_L_Q */
		Name (GPIO, 10)

		Name (_PRW, Package() { GPIO, 3 })

		Method (_DSW, 3, NotSerialized)
		{
			If (LEqual (Arg0, 1)) {
				// Enable GPIO as wake source
				\_SB.PCI0.LPCB.GPIO.GWAK (^GPIO)
			}
		}
	}
}
