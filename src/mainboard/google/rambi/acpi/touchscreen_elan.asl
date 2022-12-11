/* SPDX-License-Identifier: GPL-2.0-only */

#include <variant/onboard.h>

Scope (\_SB.PCI0.I2C6)
{
	Device (ETSA)
	{
		Name (_HID, "ELAN0001")
		Name (_DDN, "ELAN Touchscreen")
		Name (_UID, 1)
		Name (ISTP, 0) /* TouchScreen */

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				BOARD_TOUCHSCREEN_I2C_ADDR,	// SlaveAddress
				ControllerInitiated,		// SlaveMode
				400000,				// ConnectionSpeed
				AddressingMode7Bit,		// AddressingMode
				"\\_SB.PCI0.I2C6",		// ResourceSource
			)
			Interrupt (ResourceConsumer, Level, ActiveLow)
			{
				BOARD_TOUCHSCREEN_IRQ
			}
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
