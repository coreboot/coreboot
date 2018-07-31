/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
 * Copyright (C) 2015 Intel Corp.
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

Scope (\_SB.PCI0.I2C5)
{
	/* Realtek Audio Codec */
	Device (RTEK)   /* Audio Codec driver I2C */
	{
		Name (_ADR, 0)
		Name (_HID, AUDIO_CODEC_HID)
		Name (_CID, AUDIO_CODEC_CID)
		Name (_DDN, AUDIO_CODEC_DDN)
		Name (_UID, 1)

		Method(_CRS, 0x0, NotSerialized)
		{
			Name(SBUF,ResourceTemplate ()
			{
				I2CSerialBus(
					AUDIO_CODEC_I2C_ADDR,	/* SlaveAddress: bus address */
					ControllerInitiated,	/* SlaveMode: default to ControllerInitiated */
					400000,			/* ConnectionSpeed: in Hz */
					AddressingMode7Bit,	/* Addressing Mode: default to 7 bit */
					"\\_SB.PCI0.I2C5",	/* ResourceSource: I2C bus controller name */
				)

			 /* Jack Detect (index 0) */
			 GpioInt (Edge, ActiveLow, ExclusiveAndWake, PullDefault,,
				  "\\_SB.GPSW") { JACK_DETECT_GPIO_INDEX }
			} )
			Return (SBUF)
		}

		Method (_STA)
		{
			Return (0xF)
		}
	}
}

Scope (\_SB.PCI0.LPEA)
{
	Name (GBUF, ResourceTemplate ()
	{
		/* Jack Detect (index 0) */
		GpioInt (Edge, ActiveLow, ExclusiveAndWake, PullDefault,,
			 "\\_SB.GPSW") { JACK_DETECT_GPIO_INDEX }
	})
}
