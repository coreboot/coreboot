/*
 * This file is part of the coreboot project.
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

#include <variant/onboard.h>

Scope (\_SB.I2C6)
{
	Device (ATSA)
	{
		Name (_HID, "ATML0001")
		Name (_CID, EisaId ("PNP0C0E"))
		Name (_DDN, "Atmel Touchscreen")
		Name (_UID, 5)
		Name (ISTP, 0) /* TouchScreen */

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				BOARD_TOUCHSCREEN_I2C_ADDR,	// SlaveAddress
				ControllerInitiated,		// SlaveMode
				400000,				// ConnectionSpeed
				AddressingMode7Bit,		// AddressingMode
				"\\_SB.I2C6",			// ResourceSource
			)
			Interrupt (ResourceConsumer, Level, ActiveLow)
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

		Name (_PRW, Package() { BOARD_TOUCHSCREEN_WAKE_GPIO, 0x3 })
	}
}
