/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
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

Scope (\_SB)
{
#ifdef BOARD_TRACKPAD_IRQ
	/* Wake device for touchpad */
	Device (TPAD)
	{
		Name (_HID, EisaId ("PNP0C0E"))
		Name (_UID, 1)
		Name (_PRW, Package() { BOARD_TRACKPAD_WAKE_GPIO, 0x3 })

		Name (RBUF, ResourceTemplate()
		{
			Interrupt (ResourceConsumer, Level, ActiveLow)
			{
				BOARD_TRACKPAD_IRQ
			}
		})

		Method (_CRS)
		{
			/* Only return interrupt if I2C1 is PCI mode */
			If (LEqual (\S1EN, 0)) {
				Return (^RBUF)
			}

			/* Return empty resource template otherwise */
			Return (ResourceTemplate() {})
		}
	}
#endif
#ifdef BOARD_TOUCHSCREEN_IRQ
	/* Wake device for touchscreen */
	Device (TSCR)
	{
		Name (_HID, EisaId ("PNP0C0E"))
		Name (_UID, 2)
		Name (_PRW, Package() { BOARD_TOUCHSCREEN_WAKE_GPIO, 0x3 })

		Name (RBUF, ResourceTemplate()
		{
			Interrupt (ResourceConsumer, Level, ActiveLow)
			{
				BOARD_TOUCHSCREEN_IRQ
			}
		})

		Method (_CRS)
		{
			/* Return interrupt if I2C6 is PCI mode */
			If (LEqual (\S6EN, 0)) {
				Return (^RBUF)
			}

			/* Return empty resource template otherwise */
			Return (ResourceTemplate() {})
		}
	}
#endif
}

Scope (\_SB.PCI0.I2C2)
{
	Device (CODC)
	{
		/*
		 * TODO(dlaurie): Need official HID.
		 *
		 * The current HID is created from the Maxim Integrated
		 * PCI Vendor ID 193Ch and a shortened device identifier.
		 */
		Name (_HID, "193C9890")
		Name (_DDN, "Maxim 98090 Codec")
		Name (_UID, 1)

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				0x10,                     // SlaveAddress
				ControllerInitiated,      // SlaveMode
				400000,                   // ConnectionSpeed
				AddressingMode7Bit,       // AddressingMode
				"\\_SB.PCI0.I2C2",        // ResourceSource
			)
			Interrupt (ResourceConsumer, Level, ActiveLow)
			{
				BOARD_CODEC_IRQ
			}
		})

		Method (_STA)
		{
			If (LEqual (\S2EN, 1)) {
				Return (0xF)
			} Else {
				Return (0x0)
			}
		}
	}
}

Scope (\_SB.PCI0.LPEA)
{
	Name (GBUF, ResourceTemplate ()
	{
		/* Jack Detect (index 0) */
		GpioInt (Level, ActiveHigh, Exclusive, PullNone,,
			 "\\_SB.GPSC") { 14 }

		/* Mic Detect (index 1) */
		GpioInt (Level, ActiveHigh, Exclusive, PullNone,,
			 "\\_SB.GPSC") { 15 }
	})
}

#include <variant/acpi/mainboard.asl>

/* USB port entries */
#include <variant/acpi/usb.asl>
