/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0.I2C6)
{
	Device (ATPA)
	{
		Name (_HID, "ATML0000")
		Name (_DDN, "Atmel Touchpad")
		Name (_UID, 2)
		Name (ISTP, 1) /* Touchpad */
		Name (_PRW, Package() { BOARD_TRACKPAD_WAKE_GPIO, 0x3 })

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				0x4a,				// SlaveAddress
				ControllerInitiated,		// SlaveMode
				400000,				// ConnectionSpeed
				AddressingMode7Bit,		// AddressingMode
				"\\_SB.PCI0.I2C6",		// ResourceSource
			)
			GpioInt (Level, ActiveLow, SharedAndWake, PullDefault,,
				"\\_SB.GPNC") { BOARD_TRACKPAD_GPIO_INDEX }
		})

		Method (_STA)
		{
			If (\S6EN == 1) {
				Return (0xF)
			} Else {
				Return (0x0)
			}
		}

		/* Allow device to power off in S0 */
		Name (_S0W, 4)
	}
}
