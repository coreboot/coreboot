/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0.I2C6)
{
	Device (ETPA)
	{
		Name (_HID, "ELAN0000")
		Name (_DDN, "Elan Touchpad")
		Name (_UID, 3)
		Name (ISTP, 1) /* Touchpad */

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				0x15,                     /* SlaveAddress */
				ControllerInitiated,      /* SlaveMode */
				400000,                   /* ConnectionSpeed */
				AddressingMode7Bit,       /* AddressingMode */
				"\\_SB.PCI0.I2C6",        /* ResourceSource */
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

		Name (_PRW, Package() { BOARD_TRACKPAD_WAKE_GPIO, 0x3 })

		/* Allow device to power off in S0 */
		Name (_S0W, 4)
	}
}
