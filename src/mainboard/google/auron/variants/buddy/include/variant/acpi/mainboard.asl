/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0.I2C0)
{
	Device (RTEK)
	{
		Name (_HID, "10EC5650")
		Name (_CID, "INTCCFFD")
		Name (_DDN, "RTEK Codec Controller ")
		Name (_UID, 1)

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				0x1A,                     // SlaveAddress
				ControllerInitiated,      // SlaveMode
				400000,                   // ConnectionSpeed
				AddressingMode7Bit,       // AddressingMode
				"\\_SB.PCI0.I2C0",        // ResourceSource
			)

			Interrupt (ResourceConsumer, Edge, ActiveLow){ 37 }
		})

		Method (_STA)
		{
			If (\S1EN == 1) {
				Return (0xF)
			} Else {
				Return (0x0)
			}
		}
	}
}

Scope (\_SB.PCI0.I2C1)
{
	Device (ETSA)
	{
		Name (_HID, "ELAN0001")
		Name (_DDN, "Elan Touchscreen")
		Name (_UID, 6)
		Name (ISTP, 0) /* Touchscreen */

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				0x10,                     // SlaveAddress
				ControllerInitiated,      // SlaveMode
				400000,                   // ConnectionSpeed
				AddressingMode7Bit,       // AddressingMode
				"\\_SB.PCI0.I2C1",        // ResourceSource
			)
			Interrupt (ResourceConsumer, Level, ActiveLow)
			{
				BOARD_TOUCHSCREEN_IRQ
			}
		})

		Method (_STA)
		{
			If (\S2EN == 1) {
				Return (0xF)
			} Else {
				Return (0x0)
			}
		}

		Name (_PRW, Package() { BOARD_TOUCHSCREEN_WAKE_GPIO, 0x3 })

		Method (_DSW, 3, NotSerialized)
		{
			Local0 = BOARD_TOUCHSCREEN_WAKE_GPIO
			If (Arg0 == 1) {
				// Enable GPIO as wake source
				\_SB.PCI0.LPCB.GPIO.GWAK (Local0)
			}
		}

		/* Allow device to power off in S0 */
		Name (_S0W, 4)
	}
}

/*
 * LAN connected to Root Port 3, becomes Root Port 1 after coalesce
 */
Scope (\_SB.PCI0.RP01)
{
	Device (ETH0)
	{
		Name (_ADR, 0x00000000)
		Name (_PRW, Package() { BUDDY_NIC_WAKE_GPIO, 3 })

		Method (_DSW, 3, NotSerialized)
		{
			Local0 = BUDDY_NIC_WAKE_GPIO

			If (Arg0 == 1) {
				// Enable GPIO as wake source
				\_SB.PCI0.LPCB.GPIO.GWAK (Local0)
			}
		}
	}
}
