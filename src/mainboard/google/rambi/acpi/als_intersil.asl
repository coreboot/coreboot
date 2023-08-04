/* SPDX-License-Identifier: GPL-2.0-only */

#include <variant/onboard.h>

Scope (\_SB.PCI0.I2C5)
{
	Device (ALSI)
	{
		Name (_HID, EisaId ("LSD2918"))
		Name (_DDN, "Intersil 29018 Ambient Light Sensor")
		Name (_UID, 1)
		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				0x44,                     // SlaveAddress
				ControllerInitiated,      // SlaveMode
				400000,                   // ConnectionSpeed
				AddressingMode7Bit,       // AddressingMode
				"\\_SB.I2C5",             // ResourceSource
			)
			Interrupt (ResourceConsumer, Edge, ActiveLow)
			{
				BOARD_ALS_IRQ
			}
		})
		Method (_STA)
		{
			If (\S5EN == 1) {
				Return (0xF)
			} Else {
				Return (0x0)
			}
		}
	}
}
