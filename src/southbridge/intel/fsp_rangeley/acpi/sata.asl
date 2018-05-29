/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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

// Intel SATA Controller 0:17.0

// Note: Some BIOSes put the S-ATA code into an SSDT to make it easily
// pluggable

Device (SATA)
{
	Name (_ADR, 0x00170000)

	Device (PRID)
	{
		Name (_ADR, 0)

		// Get Timing Mode
		Method (_GTM, 0, Serialized)
		{
			Name(PBUF, Buffer(20) {
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0x00,0x00,0x00,0x00 })

			CreateDwordField (PBUF,  0, PIO0)
			CreateDwordField (PBUF,  4, DMA0)
			CreateDwordField (PBUF,  8, PIO1)
			CreateDwordField (PBUF, 12, DMA1)
			CreateDwordField (PBUF, 16, FLAG)

			// TODO fill return structure

			Return (PBUF)
		}

		// Set Timing Mode
		Method (_STM, 3)
		{
			CreateDwordField (Arg0,  0, PIO0)
			CreateDwordField (Arg0,  4, DMA0)
			CreateDwordField (Arg0,  8, PIO1)
			CreateDwordField (Arg0, 12, DMA1)
			CreateDwordField (Arg0, 16, FLAG)

			// TODO: Do the deed
		}

		Device (DSK0)
		{
			Name (_ADR, 0)
			// TODO: _RMV ?
			// TODO: _GTF ?
		}

		Device (DSK1)
		{
			Name (_ADR, 1)

			// TODO: _RMV ?
			// TODO: _GTF ?
		}

	}
}
