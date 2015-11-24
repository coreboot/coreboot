/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2013 Vladimir Serbinenko
 * Copyright (c) 2015 Raptor Engineering
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

	/* SuperIO control port */
	Name (SPIO, 0x2E)

	/* SuperIO control map */
	OperationRegion (SPIM, SystemIO, SPIO, 0x02)
		Field (SPIM, ByteAcc, NoLock, Preserve) {
		SIOI, 8,
		SIOD, 8
	}

	/* SuperIO control registers */
	IndexField (SIOI, SIOD, ByteAcc, NoLock, Preserve) {
		Offset (0x2A),
		CR2A, 8,		/* Pin function selection */
	}

	Device (PS2K)		// Keyboard
	{
		Name(_HID, EISAID("PNP0303"))
		Name(_CID, EISAID("PNP030B"))

		Name(_CRS, ResourceTemplate()
		{
			IO (Decode16, 0x60, 0x60, 0x01, 0x01)
			IO (Decode16, 0x64, 0x64, 0x01, 0x01)
			IRQ (Edge, ActiveHigh, Exclusive) { 0x01 } // IRQ 1
		})

		Method (_STA, 0)
		{
			Return (0xf)
		}
	}

	Device (PS2M)		// Mouse
	{
		Name(_HID, EISAID("PNP0F13"))
		Name(_CRS, ResourceTemplate()
		{
			IRQ (Edge, ActiveHigh, Exclusive) { 0x0c } // IRQ 12
		})

		Method(_STA, 0)
		{
			/* Access SuperIO ACPI device */
			Store(0x87, SIOI)
			Store(0x87, SIOI)

			/* Read Pin56 function select */
			And(CR2A, 0x2, Local0)

			/* Restore default SuperIO access */
			Store(0xAA, SIOI)

			if (LEqual(Local0, 0x0)) {
				/* Mouse function selected */
				Return (0xf)
			}
			Return (0x0)
		}
	}
