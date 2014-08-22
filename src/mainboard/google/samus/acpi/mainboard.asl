/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

Scope (\_SB)
{
	Device (LID0)
	{
		Name (_HID, EisaId("PNP0C0D"))
		Method (_LID, 0)
		{
			Return (\_SB.PCI0.LPCB.EC0.LIDS)
		}

		// EC wake is GPIO27 which is a special DeepSX wake pin
		Name (_PRW, Package(){ 0x70, 5 }) // GP27_EN
	}

	Device (PWRB)
	{
		Name(_HID, EisaId("PNP0C0C"))
	}

	Device (TPAD)
	{
		Name (_HID, EisaId("PNP0C0E"))
		Name (_UID, 1)
		Name (_PRW, Package() { 13, 0x3 }) // GPIO13
	}

	Device (TSCR)
	{
		Name (_HID, EisaId("PNP0C0E"))
		Name (_UID, 2)
		Name (_PRW, Package() { 14, 0x3 }) // GPIO14
	}

	// Keyboard Backlight interface via EC
	Device (KBLT) {
		Name (_HID, "GOOG0002")
		Name (_UID, 1)

		// Read current backlight value
		Method (KBQC, 0)
		{
			Return (\_SB.PCI0.LPCB.EC0.KBLV)
		}

		// Write new backlight value
		Method (KBCM, 1)
		{
			Store (Arg0, \_SB.PCI0.LPCB.EC0.KBLV)
		}
	}
}

/*
 * WLAN connected to Root Port 3, becomes Root Port 1 after coalesce
 */
Scope (\_SB.PCI0.RP01)
{
	Device (WLAN)
	{
		Name (_ADR, 0x00000000)

		/* GPIO10 is PCH_WLAN_WAKE_L */
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

Scope (\_SB.PCI0.I2C0)
{
	Device (ATPB)
	{
		Name (_HID, "ATML0000")
		Name (_DDN, "Atmel Touchpad Bootloader")
		Name (_UID, 1)
		Name (_S0W, 4)
		Name (ISTP, 1) /* Touchpad */

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				0x26,                     // SlaveAddress
				ControllerInitiated,      // SlaveMode
				400000,                   // ConnectionSpeed
				AddressingMode7Bit,       // AddressingMode
				"\\_SB.PCI0.I2C0",        // ResourceSource
			)

			// GPIO13 is PIRQL
			Interrupt (ResourceConsumer, Edge, ActiveLow) { 27 }
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

	Device (ATPA)
	{
		Name (_HID, "ATML0000")
		Name (_DDN, "Atmel Touchpad")
		Name (_UID, 2)
		Name (_S0W, 4)
		Name (ISTP, 1) /* Touchpad */

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				0x4a,                     // SlaveAddress
				ControllerInitiated,      // SlaveMode
				400000,                   // ConnectionSpeed
				AddressingMode7Bit,       // AddressingMode
				"\\_SB.PCI0.I2C0",        // ResourceSource
			)

			// GPIO13 is PIRQL
			Interrupt (ResourceConsumer, Edge, ActiveLow) { 27 }
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

	Device (CODC)
	{
		/*
		 * TODO(kane): Need official HID.
		 *
		 */
		Name (_HID, "RT5677CE")
		Name (_DDN, "RT5667 Codec")
		Name (_UID, 1)
		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				0x2c,                     // SlaveAddress
				ControllerInitiated,      // SlaveMode
				400000,                   // ConnectionSpeed
				AddressingMode7Bit,       // AddressingMode
				"\\_SB.PCI0.I2C0",        // ResourceSource
			)
			Interrupt (ResourceConsumer, Edge, ActiveLow){ 30 }
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

	Device (HOTW)
	{
		Name (_HID, "PNP0A05")
		Name (_DDN, "Hotword Wake")
		Name (_UID, 1)
		Name (GPIO, 46) /* HOTWORD_DET_L_3V3 */

		Name (_PRW, Package() { GPIO, 3 })

		Method (_DSW, 3, NotSerialized)
		{
			If (LEqual (Arg0, 1)) {
				// Enable GPIO as wake source
				\_SB.PCI0.LPCB.GPIO.GWAK (^GPIO)
			}
		}

		Method (_STA)
		{
			Return (0xF)
		}
	}
}

Scope (\_SB.PCI0.I2C1)
{
	Device (ATSB)
	{
		Name (_HID, "ATML0001")
		Name (_DDN, "Atmel Touchscreen Bootloader")
		Name (_UID, 4)
		Name (_S0W, 4)
		Name (ISTP, 0) /* TouchScreen */

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				0x25,                     // SlaveAddress
				ControllerInitiated,      // SlaveMode
				400000,                   // ConnectionSpeed
				AddressingMode7Bit,       // AddressingMode
				"\\_SB.PCI0.I2C1",        // ResourceSource
			)

			// GPIO14 is PIRQM
			Interrupt (ResourceConsumer, Edge, ActiveLow) { 28 }
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
				0x4b,                     // SlaveAddress
				ControllerInitiated,      // SlaveMode
				400000,                   // ConnectionSpeed
				AddressingMode7Bit,       // AddressingMode
				"\\_SB.PCI0.I2C1",        // ResourceSource
			)

			// GPIO14 is PIRQM
			Interrupt (ResourceConsumer, Edge, ActiveLow) { 28 }
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
