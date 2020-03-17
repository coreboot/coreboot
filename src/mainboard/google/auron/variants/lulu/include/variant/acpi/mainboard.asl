/*
 * This file is part of the coreboot project.
 *
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

Scope (\_SB.PCI0.I2C0)
{
	Device (STPA)
	{
		Name (_HID, "SYNA0000")
		Name (_CID, "ACPI0C50")
		Name (_DDN, "Synaptics Touchpad")
		Name (_UID, 1)
		Name (ISTP, 1) /* Touchpad */

		Method(_CRS, 0x0, Serialized)
		{
			Name (RBUF, ResourceTemplate()
			{
				I2cSerialBus (
					BOARD_TRACKPAD_I2C_ADDR,	// SlaveAddress
					ControllerInitiated,		// SlaveMode
					400000,				// ConnectionSpeed
					AddressingMode7Bit,		// AddressingMode
					"\\_SB.PCI0.I2C0",		// ResourceSource
				)
				Interrupt (ResourceConsumer, Level, ActiveLow)
				{
					BOARD_TRACKPAD_IRQ
				}
			})
			Return(RBUF)
		}

		Method(_DSM, 0x4, NotSerialized)
		{
			If (LEqual(Arg0, ToUUID("3CDFF6F7-4267-4555-AD05-B30A3D8938DE"))) /* I2C-HID UUID */
			{
				If (LEqual(Arg2, Zero)) /* DSM Function */
				{
					/* Function 0: Query function, return based on revision */
					If (LEqual(Arg1, One)) /* Arg1 DSM Revision */
					{
						/* Revision 1: Function 0 supported */
						Return(Buffer(One) { 0x03 })
					}
				} ElseIf (LEqual(Arg2, One)) /* Function 1 : HID Function */
				{
					Return(0x0020) /* HID Descriptor Address */
				}
			}

			Return(Buffer(One) { 0x00 }) /* Not supported */
		}

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
	Device (ETSA)
	{
		Name (_HID, "ELAN0001")
		Name (_DDN, "Elan Touchscreen")
		Name (_UID, 6)
		Name (ISTP, 0) /* Touchscreen */

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				BOARD_TOUCHSCREEN_I2C_ADDR,	// SlaveAddress
				ControllerInitiated,		// SlaveMode
				400000,				// ConnectionSpeed
				AddressingMode7Bit,		// AddressingMode
				"\\_SB.PCI0.I2C1",		// ResourceSource
			)
			Interrupt (ResourceConsumer, Level, ActiveLow)
			{
				BOARD_TOUCHSCREEN_IRQ
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

		Name (_PRW, Package() { BOARD_TOUCHSCREEN_WAKE_GPIO, 0x3 })

		Method (_DSW, 3, NotSerialized)
		{
			Store (BOARD_TOUCHSCREEN_WAKE_GPIO, Local0)
			If (LEqual (Arg0, 1)) {
				// Enable GPIO as wake source
				\_SB.PCI0.LPCB.GPIO.GWAK (Local0)
			}
		}

		/* Allow device to power off in S0 */
		Name (_S0W, 4)
	}
}
