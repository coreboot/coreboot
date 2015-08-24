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
 * Foundation, Inc.
 */

#include "../gpio.h"

#define BOARD_TRACKPAD_IRQ		0x33
#define BOARD_TOUCHSCREEN_IRQ		0x1f

#define BOARD_TRACKPAD_I2C_ADDR		0x15
#define BOARD_TOUCHSCREEN_I2C_ADDR	0x10
#define BOARD_LEFT_SSM4567_I2C_ADDR	0x34
#define BOARD_RIGHT_SSM4567_I2C_ADDR	0x35
#define BOARD_AUDIO_CODEC_I2C_ADDR	0x1A

Scope (\_SB)
{
	Device (LID0)
	{
		Name (_HID, EisaId("PNP0C0D"))
		Method (_LID, 0)
		{
			Return (\_SB.PCI0.LPCB.EC0.LIDS)
		}

		Name (_PRW, Package () { GPE_EC_WAKE, 5 })
	}

	Device (PWRB)
	{
		Name (_HID, EisaId("PNP0C0C"))
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
	Device (ETSA)
	{
		Name (_HID, "ELAN0001")
		Name (_DDN, "ELAN Touchscreen")
		Name (_UID, 5)
		Name (ISTP, 0) /* TouchScreen */
		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				BOARD_TOUCHSCREEN_I2C_ADDR,
				ControllerInitiated,
				400000,
				AddressingMode7Bit,
				"\\_SB.PCI0.I2C0",
			)
			Interrupt (ResourceConsumer, Edge, ActiveLow) {
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
		/* Allow device to power off in S0 */
		Name (_S0W, 4)
		Name (ISTP, 1) /* TouchPad */
		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				BOARD_TRACKPAD_I2C_ADDR,
				ControllerInitiated,
				400000,
				AddressingMode7Bit,
				"\\_SB.PCI0.I2C1",
			)
			Interrupt (ResourceConsumer, Edge, ActiveLow)
			{
				BOARD_TRACKPAD_IRQ
			}
		})
		Method (_STA)
		{
			Return (0xF)
		}
	}
}

Scope (\_SB.PCI0.I2C4)
{
	// LEFT SSM4567 I2c ADDR 0x34
	Device (LSPK)
	{
		Name (_HID, "INT343B")
		Name (_CID, "INT343B")
		Name (_DDN, "Intel(R) Smart Sound Technology Audio Codec")
		Name (_UID, 1)

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				BOARD_LEFT_SSM4567_I2C_ADDR,
				ControllerInitiated,
				400000,
				AddressingMode7Bit,
				"\\_SB.PCI0.I2C4",
			)
		})

		Method (_STA, 0, NotSerialized)
		{
			Return (0xF) // I2S Codec ADI LEFT SSM4567 Enabled
		}
	} // Device (LSPK)

	// RIGHT SSM4567 I2C ADDR 0x35
	Device (RSPK)
	{
		Name (_HID, "INT343B")
		Name (_CID, "INT343B")
		Name (_DDN, "Intel(R) Smart Sound Technology Audio Codec")
		Name (_UID, 2)

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				BOARD_RIGHT_SSM4567_I2C_ADDR,
				ControllerInitiated,
				400000,
				AddressingMode7Bit,
				"\\_SB.PCI0.I2C4",
			)
		})

		Method (_STA, 0, NotSerialized)
		{
			Return (0xF) // I2S Codec ADI RIGHT SSM4567 Enabled
		}
	} // Device (RSPK)

	// Nuvoton NAU88L25 (I2SC = 2)
	Device (HDAC)
	{
		Name (_HID, "10508825")
		Name (_CID, "10508825")
		Name (_DDN, "Intel(R) Smart Sound Technology Audio Codec")
		Name (_UID, 1)

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				BOARD_AUDIO_CODEC_I2C_ADDR,
				ControllerInitiated,
				400000,
				AddressingMode7Bit,
				"\\_SB.PCI0.I2C4",
			)
		})

		Method (_STA, 0, NotSerialized)
		{
			Return (0xF) // I2S Codec  Enabled
		}
	} // Device (HDAC)
}


