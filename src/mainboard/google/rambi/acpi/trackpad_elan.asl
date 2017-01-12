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

Scope (\_SB.I2C1)
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
				BOARD_TRACKPAD_I2C_ADDR,	// SlaveAddress
				ControllerInitiated,		// SlaveMode
				400000,				// ConnectionSpeed
				AddressingMode7Bit,		// AddressingMode
				"\\_SB.I2C1",			// ResourceSource
			)
			Interrupt (ResourceConsumer, Level, ActiveLow)
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
