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

#include <mainboard/google/butterfly/onboard.h>

Scope (\_SB) {
	Device (LID0)
	{
		Name (_HID, EisaId("PNP0C0D"))
		Method (_LID, 0)
		{
			Store (\_SB.PCI0.LPCB.EC0.LIDF, \LIDS)
			Return (\LIDS)
		}
	}

	Device (TPAD)
	{
		Name (_ADR, 0x0)
		Name (_UID, 1)

		// Report as a Sleep Button device so Linux will
		// automatically enable it as a wake source
		Name(_HID, EisaId("PNP0C0E"))

		// Trackpad Wake is GPIO11, wake from S3
		Name(_PRW, Package() { BOARD_TRACKPAD_WAKE_GPIO, 0x03 })

		Name(_CRS, ResourceTemplate()
		{
			// PIRQG -> GSI22
			Interrupt (ResourceConsumer, EDGE, ActiveLow)
			{
				BOARD_TRACKPAD_IRQ
			}

			// SMBUS Address 0x67
			VendorShort (ADDR) { BOARD_TRACKPAD_I2C_ADDR }
		})
	}

}

// Battery information
Name (BATV, "GOOGLE")
