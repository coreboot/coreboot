/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Google Inc.
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

#include <mainboard/google/link/onboard.h>

Scope (\_SB) {
	Device (TPAD)
	{
		Name (_ADR, 0x0)
		Name (_UID, 1)

		// Report as a Sleep Button device so Linux will
		// automatically enable it as a wake source
		Name(_HID, EisaId("PNP0C0E"))

		// Trackpad Wake is GPIO12
		Name(_PRW, Package() { BOARD_TRACKPAD_WAKE_GPIO, 0x03 } )

		Name(_CRS, ResourceTemplate()
		{
			// PIRQE -> GSI20
			Interrupt (ResourceConsumer, Edge, ActiveLow)
			{
				BOARD_TRACKPAD_IRQ
			}

			// SMBUS Address 0x4b
			VendorShort (ADDR) { BOARD_TRACKPAD_I2C_ADDR }
		})
	}

	Device (TSCR)
	{
		Name (_ADR, 0x0)
		Name (_UID, 2)

		// Report as a Sleep Button device so Linux will
		// automatically enable it as a wake source
		Name(_HID, EisaId("PNP0C0E"))

		// Touchscreen Wake is GPIO14
		Name(_PRW, Package(){0x1e, 0x03})

		Name(_CRS, ResourceTemplate()
		{
			// PIRQG -> GSI22
			Interrupt (ResourceConsumer, Edge, ActiveLow) {22}

			// SMBUS Address 0x4a
			VendorShort (ADDR) {0x4a}
		})
	}
}
