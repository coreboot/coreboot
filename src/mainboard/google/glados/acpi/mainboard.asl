/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Inc.
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

#define BOARD_TRACKPAD_I2C_ADDR		0x15
#define BOARD_TRACKPAD_IRQ		0x33

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

	/* Keyboard Backlight interface via EC */
	Device (KBLT) {
		Name (_HID, "GOOG0002")
		Name (_UID, 1)

		/* Read current backlight value */
		Method (KBQC, 0)
		{
			Return (\_SB.PCI0.LPCB.EC0.KBLV)
		}

		/* Write new backlight value */
		Method (KBCM, 1)
		{
			Store (Arg0, \_SB.PCI0.LPCB.EC0.KBLV)
		}
	}
}

/*
 * LPC Trusted Platform Module
 */
Scope (\_SB.PCI0.LPCB)
{
	#include <drivers/pc80/tpm/acpi/tpm.asl>
}

/* Trackpad */
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
				BOARD_TRACKPAD_IRQ
			}
		})
		Method (_STA)
		{
			Return (0xF)
		}
	}
}
