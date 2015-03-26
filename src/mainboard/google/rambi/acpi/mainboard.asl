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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <mainboard/google/rambi/onboard.h>

Scope (\_SB)
{
	Device (LID0)
	{
		Name (_HID, EisaId ("PNP0C0D"))
		Name (_PRW, Package() { BOARD_PCH_WAKE_GPIO, 0x5 })
		Method (_LID, 0)
		{
			Store (\_SB.PCI0.LPCB.EC0.LIDS, \LIDS)
			Return (\LIDS)
		}
	}

	Device (PWRB)
	{
		Name (_HID, EisaId ("PNP0C0C"))
		Name (_UID, 1)
	}

	/* Wake device for touchpad */
	Device (TPAD)
	{
		Name (_HID, EisaId ("PNP0C0E"))
		Name (_UID, 1)
		Name (_PRW, Package() { BOARD_TRACKPAD_WAKE_GPIO, 0x3 })

		Name (RBUF, ResourceTemplate()
		{
			Interrupt (ResourceConsumer, Edge, ActiveLow)
			{
				BOARD_TRACKPAD_IRQ
			}
		})

		Method (_CRS)
		{
			/* Only return interrupt if I2C1 is PCI mode */
			If (LEqual (\S1EN, 0)) {
				Return (^RBUF)
			}
		}
	}

	/* Wake device for touchscreen */
	Device (TSCR)
	{
		Name (_HID, EisaId ("PNP0C0E"))
		Name (_UID, 2)
		Name (_PRW, Package() { BOARD_TOUCHSCREEN_WAKE_GPIO, 0x3 })

		Name (RBUF, ResourceTemplate()
		{
			Interrupt (ResourceConsumer, Edge, ActiveLow)
			{
				BOARD_TOUCHSCREEN_IRQ
			}
		})

		Method (_CRS)
		{
			/* Only return interrupt if I2C6 is PCI mode */
			If (LEqual (\S6EN, 0)) {
				Return (^RBUF)
			}
		}
	}
}

Scope (\_SB.I2C1)
{
	Device (ATPB)
	{
		Name (_HID, "ATML0000")
		Name (_DDN, "Atmel Touchpad Bootloader")
		Name (_UID, 1)
		Name (ISTP, 1) /* Touchpad */

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				0x25,                     // SlaveAddress
				ControllerInitiated,      // SlaveMode
				400000,                   // ConnectionSpeed
				AddressingMode7Bit,       // AddressingMode
				"\\_SB.I2C1",             // ResourceSource
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

		/* Allow device to power off in S0 */
		Name (_S0W, 4)
	}

	Device (ATPA)
	{
		Name (_HID, "ATML0000")
		Name (_DDN, "Atmel Touchpad")
		Name (_UID, 2)
		Name (ISTP, 1) /* Touchpad */

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				0x4b,                     // SlaveAddress
				ControllerInitiated,      // SlaveMode
				400000,                   // ConnectionSpeed
				AddressingMode7Bit,       // AddressingMode
				"\\_SB.I2C1",             // ResourceSource
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

		/* Allow device to power off in S0 */
		Name (_S0W, 4)
	}

	Device (ETPA)
	{
		Name (_HID, "ELAN0000")
		Name (_DDN, "Elan Touchpad")
		Name (_UID, 3)
		Name (ISTP, 1) /* Touchpad */

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				0x15,                     // SlaveAddress
				ControllerInitiated,      // SlaveMode
				400000,                   // ConnectionSpeed
				AddressingMode7Bit,       // AddressingMode
				"\\_SB.I2C1",             // ResourceSource
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

		/* Allow device to power off in S0 */
		Name (_S0W, 4)
	}
}

Scope (\_SB.I2C2)
{
	Device (CODC)
	{
		/*
		 * TODO(dlaurie): Need official HID.
		 *
		 * The current HID is created from the Maxim Integrated
		 * PCI Vendor ID 193Ch and a shortened device identifier.
		 */
		Name (_HID, "193C9890")
		Name (_DDN, "Maxim 98090 Codec")
		Name (_UID, 1)

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				0x10,                     // SlaveAddress
				ControllerInitiated,      // SlaveMode
				400000,                   // ConnectionSpeed
				AddressingMode7Bit,       // AddressingMode
				"\\_SB.I2C2",             // ResourceSource
			)
			Interrupt (ResourceConsumer, Edge, ActiveLow)
			{
				BOARD_CODEC_IRQ
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

Scope (\_SB.I2C5)
{
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
		Name (_UID, 1)

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				0x44,                     // SlaveAddress
				ControllerInitiated,      // SlaveMode
				400000,                   // ConnectionSpeed
				AddressingMode7Bit,       // AddressingMode
				"\\_SB.I2C5",             // ResourceSource
			)
			Interrupt (ResourceConsumer, Edge, ActiveLow)
			{
				BOARD_ALS_IRQ
			}
		})

		Method (_STA)
		{
			If (LEqual (\S5EN, 1)) {
				Return (0xF)
			} Else {
				Return (0x0)
			}
		}
	}
}

Scope (\_SB.I2C6)
{
	Device (ATSB)
	{
		Name (_HID, "ATML0001")
		Name (_DDN, "Atmel Touchscreen Bootloader")
		Name (_UID, 4)
		Name (ISTP, 0) /* TouchScreen */

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				0x26,                     // SlaveAddress
				ControllerInitiated,      // SlaveMode
				400000,                   // ConnectionSpeed
				AddressingMode7Bit,       // AddressingMode
				"\\_SB.I2C6",             // ResourceSource
			)
			Interrupt (ResourceConsumer, Edge, ActiveLow)
			{
				BOARD_TOUCHSCREEN_IRQ
			}
		})

		Method (_STA)
		{
			If (LEqual (\S6EN, 1)) {
				Return (0xF)
			} Else {
				Return (0x0)
			}
		}

		/* Allow device to power off in S0 */
		Name (_S0W, 4)
	}

	Device (ATSA)
	{
		Name (_HID, "ATML0001")
		Name (_DDN, "Atmel Touchscreen")
		Name (_UID, 5)
		Name (ISTP, 0) /* TouchScreen */

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				0x4a,                     // SlaveAddress
				ControllerInitiated,      // SlaveMode
				400000,                   // ConnectionSpeed
				AddressingMode7Bit,       // AddressingMode
				"\\_SB.I2C6",             // ResourceSource
			)
			Interrupt (ResourceConsumer, Edge, ActiveLow)
			{
				BOARD_TOUCHSCREEN_IRQ
			}
		})

		Method (_STA)
		{
			If (LEqual (\S6EN, 1)) {
				Return (0xF)
			} Else {
				Return (0x0)
			}
		}

		/* Allow device to power off in S0 */
		Name (_S0W, 4)
	}
}

Scope (\_SB.LPEA)
{
	Name (GBUF, ResourceTemplate ()
	{
		/* Jack Detect (index 0) */
		GpioInt (Edge, ActiveHigh, Exclusive, PullNone,,
			 "\\_SB.GPSC") { 14 }

		/* Mic Detect (index 1) */
		GpioInt (Edge, ActiveHigh, Exclusive, PullNone,,
			 "\\_SB.GPSC") { 15 }
	})
}
