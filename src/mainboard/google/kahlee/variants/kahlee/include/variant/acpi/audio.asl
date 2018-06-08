/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* Realtek Audio Codec */
Device (RTEK)   /* Audio Codec driver I2CS*/
{
	Name (_ADR, 0)
	Name (_HID, "10EC5650")
	Name (_CID, "10EC5650")
	Name (_DDN, "RTEK Codec Controller ")
	Name (_UID, 1)

	Device (I2S) /* I2S machine driver for RT5650 */
	{
		Name (_ADR, 1)
		Name (_HID, "AMDI1002")
		Name (_CID, "AMDI1002")
	}

	Method (_CRS, 0x0, Serialized)
	{
		Name (SBUF, ResourceTemplate ()
		{
			I2CSerialBus(
			0x1A,   /* SlaveAddress: bus address */
			ControllerInitiated,    /* SlaveMode: default to ControllerInitiated */
			400000,                 /* ConnectionSpeed: in Hz */
			AddressingMode7Bit,     /* Addressing Mode: default to 7 bit */
			"\\_SB.I2CA",           /* ResourceSource: I2C bus controller name */
			)

			/* Jack Detect AGPIO90 */
			GpioInt (Edge, ActiveLow, ExclusiveAndWake, PullNone,,
				"\\_SB.GPIO") { 90 }
		})
		Return (SBUF)
	}

	Method (_STA)
	{
		Return (0xF)
	}
}
