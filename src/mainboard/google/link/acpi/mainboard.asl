/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/google/link/onboard.h>

Scope (\_SB) {
	Device (TPAD)
	{
		Name (_UID, 1)

		// Report as a Sleep Button device so Linux will
		// automatically enable it as a wake source
		Name(_HID, EisaId("PNP0C0E"))

		Name(_CRS, ResourceTemplate()
		{
			// PIRQE -> GSI20
			Interrupt (ResourceConsumer, Edge, ActiveLow)
			{
				BOARD_TRACKPAD_IRQ
			}

			// SMBUS Address 0x4b
			VendorShort (ADDR) { BOARD_TRACKPAD_I2C_ADDR }
		})
	}

	Device (TSCR)
	{
		Name (_UID, 2)

		// Report as a Sleep Button device so Linux will
		// automatically enable it as a wake source
		Name(_HID, EisaId("PNP0C0E"))

		Name(_CRS, ResourceTemplate()
		{
			// PIRQG -> GSI22
			Interrupt (ResourceConsumer, Edge, ActiveLow) {22}

			// SMBUS Address 0x4a
			VendorShort (ADDR) {0x4a}
		})
	}
}


Scope (\_SB.PCI0.GFX0)
{
	Device (GMBS){
		Name (_HID, "BOOT0001")
		Method (_STA, 0, NotSerialized)  // _STA: Status
		{
			Return (0x0F)
		}

		Method (MMIO, 0, Serialized)
		{
			Local0 = BAR0 & 0xFFFFFFFFFFFFFFF0
			Return (Local0)
		}

		Device (LVGA) {
			Name (_HID, "BOOT0002")
			Name (_UID, 0)
			Name (_DSD, Package () {
				ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
				Package () {
					Package (2) {"coolstar,bus-number", GMBUS_PIN_VGADDC}
				}
			})

			Method (_STA)
			{
				Return (0xF)
			}

			Device (ATPD)
			{
				Name (_HID, "ATML0000")
				Name (_DDN, "Atmel Touchpad")
				Name (_UID, 0)

				Name (_CRS, ResourceTemplate()
				{
					I2cSerialBus (
						BOARD_TRACKPAD_I2C_ADDR,        // SlaveAddress
						ControllerInitiated,            // SlaveMode
						100000,                         // ConnectionSpeed
						AddressingMode7Bit,             // AddressingMode
						"\\_SB.PCI0.GFX0.GMBS.LVGA",    // ResourceSource
					)

					Interrupt (ResourceConsumer, Level, ActiveLow) { BOARD_TRACKPAD_IRQ }
				})

				// Trackpad Wake is GPIO12
				Name(_PRW, Package() { BOARD_TRACKPAD_WAKE_GPIO, 0x03 } )

				Method (_STA)
				{
					Return (0xF)
				}
			}
		}

		Device (LPNL) {
			Name (_HID, "BOOT0002")
			Name (_UID, 1)
			Name (_DSD, Package () {
				ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
				Package () {
					Package (2) {"coolstar,bus-number", GMBUS_PIN_PANEL}
				}
			})

			Method (_STA)
			{
				Return (0xF)
			}

			Device (ATSA)
			{
				Name (_HID, "ATML0001")
				Name (_DDN, "Atmel TouchScreen")
				Name (_UID, 0)

				Name (_CRS, ResourceTemplate()
				{
					I2cSerialBus (
						BOARD_TOUCHSCREEN_I2C_ADDR,     // SlaveAddress
						ControllerInitiated,            // SlaveMode
						100000,                         // ConnectionSpeed
						AddressingMode7Bit,             // AddressingMode
						"\\_SB.PCI0.GFX0.GMBS.LPNL",    // ResourceSource
					)

					Interrupt (ResourceConsumer, Level, ActiveLow) { BOARD_TOUCHSCREEN_IRQ }
				})

				// Touchscreen Wake is GPIO14
				Name(_PRW, Package() { BOARD_TOUCHSCREEN_WAKE_GPIO, 0x03 } )

				Method (_STA)
				{
					Return (0xF)
				}
			}
		}
	}
}
