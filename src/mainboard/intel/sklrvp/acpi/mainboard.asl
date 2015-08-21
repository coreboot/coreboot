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

#include <soc/gpio.h>
#include <mainboard/intel/sklrvp/onboard.h>

/*
 * LPC Trusted Platform Module
 */
Scope (\_SB.PCI0.LPCB)
{
	#include <drivers/pc80/tpm/acpi/tpm.asl>
}

/*
 * WLAN connected to Root Port 3, becomes Root Port 1 after coalesce
 */
Scope (\_SB.PCI0.RP01)
{
	Device (WLAN)
	{
		Name (_ADR, 0x00000000)

		/* GPIO10 is PCH_WLAN_WAKE_L */
		Name (GPIO, 10)

		Name (_PRW, Package() { GPIO, 3 })

	}
}

Scope (\_SB.PCI0.I2C0)
{
	Device (ETPA)
	{
		Name (_HID, "SYN2393")
		Name (_CID, "PNP0C50")
		Name (_DDN, "Synaptic Touchpad")
		Name (_UID, 3)
		Name (ISTP, 1) /* Touchpad */

		/* Fetch HidDescriptorAddress, Register offset in the
		 * I2C device at which the HID descriptor can be read
		 */
		Method (_DSM, 4, NotSerialized)
		{
			If (LEqual (Arg0, ToUUID (
				"3cdff6f7-4267-4555-ad05-b30a3d8938de")))
			{
				If (LEqual (Arg2, Zero))
				{
					If (LEqual (Arg1, One))
					{
						Return (Buffer (One)
						{
							0x03
						})
					}
					Else
					{
						Return (Buffer (One)
						{
							0x00
						})
					}
				}
				If (LEqual (Arg2, One))
				{
					Return (0x20)
				}
			}
			Else
			{
				Return (Buffer (One)
				{
					0x00
				})
			}

			Return (Zero)
		}

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				BOARD_TOUCHPAD_I2C_ADDR,	/* SlaveAddress */
				ControllerInitiated,	/* SlaveMode */
				400000,			/* ConnectionSpeed */
				AddressingMode7Bit,	/* AddressingMode */
				"\\_SB.PCI0.I2C0",	/* ResourceSource */
			)
			Interrupt (ResourceConsumer, Level, ActiveLow)
				{ BOARD_TOUCHPAD_IRQ }
		})
	}

	//-----------------------------------
	//  HD Audio I2S Codec device
	//  Realtek ALC286S       (I2SC = 2)
	//-----------------------------------
	Device (HDAC)
	{
		Name (_HID, "INT343A")
		Name (_CID, "INT343A")
		Name (_DDN, "Intel(R) Smart Sound Technology Audio Codec")
		Name (_UID, 1)

		Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
		{
			/* update Audio codec slave address in runtime */
			Name (RBUF, ResourceTemplate ()
			{
				I2cSerialBus (0x1C, ControllerInitiated, 400000,
					AddressingMode7Bit, "\\_SB.PCI0.I2C0",
					0x00, ResourceConsumer, ,)
			})

			/* update interrupt number in runtime */
			Name (SBFI, ResourceTemplate ()
			{
				Interrupt (ResourceConsumer, Level, ActiveLow,
						ExclusiveAndWake)
				{
					GPP_E22_IRQ
				}
			})
		}

		Method (_STA, 0, NotSerialized)
		{
			Return (0xF)	/* I2S Codec Enabled */
		}
	}
}

Scope (\_SB.PCI0.I2C1)
{
	Device (ATSA)
	{
		Name (_HID, "ATML3432")
		Name (_DDN, "Atmel Touchscreen")
		Name (_UID, 5)
		Name (_S0W, 4)
		Name (ISTP, 0) /* TouchScreen */
		Name (_CID, "PNP0C50")

		/* Fetch HidDescriptorAddress, Register offset in the
		 * I2C device at which the HID descriptor can be read
		 */
		Method (_DSM, 4, NotSerialized)
		{
			If (LEqual (Arg0, ToUUID (
				"3cdff6f7-4267-4555-ad05-b30a3d8938de")))
			{
				If (LEqual (Arg2, Zero))
				{
					If (LEqual (Arg1, One))
					{
						Return (Buffer (One)
						{
							0x03
						})
					}
					Else
					{
						Return (Buffer (One)
						{
							0x00
						})
					}
				}

				If (LEqual (Arg2, One))
				{
					Return (Zero)
				}
			}
			Else
			{
				Return (Buffer (One)
				{
					0x00
				})
			}

			Return (Zero)
		}

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				BOARD_TOUCHSCREEN_I2C_ADDR,	// SlaveAddress
				ControllerInitiated,		// SlaveMode
				400000,				// ConnectionSpeed
				AddressingMode7Bit,		// AddressingMode
				"\\_SB.PCI0.I2C1",		// ResourceSource
			)

			Interrupt (ResourceConsumer, Level, ActiveLow)
				{ BOARD_TOUCHSCREEN_IRQ }
		})

		Method (_STA, 0, NotSerialized)
		{
			Return (0xF)
		}
	}
}
