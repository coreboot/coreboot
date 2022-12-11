/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0.I2C1)
{
	Device (MTSA)
	{
		Name (_HID, "MLFS0000")
		Name (_DDN, "Melfas Touchscreen ")
		Name (_UID, 5)
		Name (ISTP, 0) /* TouchScreen */

		Method(_CRS, 0x0, NotSerialized)
		{
			Name (BUF0, ResourceTemplate ()
			{
				I2cSerialBus(
					0x34,                     /* SlaveAddress */
					ControllerInitiated,      /* SlaveMode */
					400000,                   /* ConnectionSpeed */
					AddressingMode7Bit,       /* AddressingMode */
					"\\_SB.PCI0.I2C1",        /* ResourceSource */
				)
				GpioInt (Level, ActiveLow, SharedAndWake, PullDefault,,
					"\\_SB.GPNC") { BOARD_TOUCH_GPIO_INDEX }
			})
			Return (BUF0)
		}

		Method (_STA)
		{
			If (\S1EN == 1) {
				Return (0xF)
			} Else {
				Return (0x0)
			}
		}

		Name (_PRW, Package() { BOARD_TOUCHSCREEN_WAKE_GPIO, 0x3 })

		/* Allow device to power off in S0 */
		Name (_S0W, 4)
	}
}
