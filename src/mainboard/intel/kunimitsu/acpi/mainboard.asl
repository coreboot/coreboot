/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#define KUNIMITSU_TRACKPAD_IRQ		0x33
#define KUNIMITSU_TOUCH_IRQ		0x1f

#define BOARD_TRACKPAD_NAME		"trackpad"
#define BOARD_TRACKPAD_IRQ		KUNIMITSU_TRACKPAD_IRQ
#define BOARD_TRACKPAD_I2C_BUS		1
#define BOARD_TRACKPAD_I2C_ADDR	0x15

#define BOARD_TOUCHSCREEN_NAME		"touchscreen"
#define BOARD_TOUCHSCREEN_IRQ		KUNIMITSU_TOUCH_IRQ
#define BOARD_TOUCHSCREEN_I2C_BUS	0
#define BOARD_TOUCHSCREEN_I2C_ADDR	0x4b

Scope (\_SB)
{
	Device (LID0)
	{
		Name (_HID, EisaId("PNP0C0D"))
		Method (_LID, 0)
		{
			Return (\_SB.PCI0.LPCB.EC0.LIDS)
		}
	}

	Device (PWRB)
	{
		Name(_HID, EisaId("PNP0C0C"))
	}
}
/*
 * LPC Trusted Platform Module
 */
Scope (\_SB.PCI0.LPCB)
{
	#include <drivers/pc80/tpm/acpi/tpm.asl>
}
Scope (\_SB.PCI0.I2C0)
{
	Device (ATSB)
	{
		Name (_HID, "ATML0001")
		Name (_DDN, "Atmel Touchscreen Bootloader")
		Name (_UID, 4)
		Name (_S0W, 4)
		Name (ISTP, 0) /* TouchScreen */
		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				0x27,                     // SlaveAddress
				ControllerInitiated,      // SlaveMode
				400000,                   // ConnectionSpeed
				AddressingMode7Bit,       // AddressingMode
				"\\_SB.PCI0.I2C0",        // ResourceSource
			)
			Interrupt (ResourceConsumer, Edge, ActiveLow) { 28 }
		})
		Method (_STA)
		{
			Return (0xF)
		}
	}
	Device (ATSA)
	{
		Name (_HID, "ATML0001")
		Name (_DDN, "Atmel Touchscreen")
		Name (_UID, 5)
		Name (_S0W, 4)
		Name (ISTP, 0) /* TouchScreen */
		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				BOARD_TOUCHSCREEN_I2C_ADDR, // SlaveAddress
				ControllerInitiated,        // SlaveMode
				400000,                     // ConnectionSpeed
				AddressingMode7Bit,         // AddressingMode
				"\\_SB.PCI0.I2C0",          // ResourceSource
			)
			Interrupt (ResourceConsumer, Edge, ActiveLow)
			{
				BOARD_TOUCHSCREEN_IRQ
			}
		})
		Method (_STA, 0, NotSerialized)
		{
			Return (0xF)
		}
	}
}
Scope (\_SB.PCI0.I2C1)
{
	Device (ELAN)
	{
		Name (_HID, "ELAN0000")
		Name (_DDN, "Elan Touchpad")
		Name (_UID, 3)
		Name (_S0W, 4)
		Name (ISTP, 1) /* TouchPad */
		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				BOARD_TRACKPAD_I2C_ADDR,	/* SlaveAddress */
				ControllerInitiated,		/* SlaveMode */
				400000,				/* ConnectionSpeed */
				AddressingMode7Bit,		/* AddressingMode */
				"\\_SB.PCI0.I2C1",		/* ResourceSource */
			)
			Interrupt (ResourceConsumer, Edge, ActiveLow)
			{
				KUNIMITSU_TRACKPAD_IRQ
			}
		})
		Method (_STA)
		{
			Return (0xF)
		}
	}
}
