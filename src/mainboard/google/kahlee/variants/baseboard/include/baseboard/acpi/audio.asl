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
#include <soc/iomap.h>

/* Grunt specific I2S machine driver */
Device (I2S)
{
	Name (_ADR, 1)
	Name (_HID, "AMD7219")
	Name (_CID, "AMD7219")

	Method (_CRS, 0x0, NotSerialized) {
		Name (RBUF, ResourceTemplate () {
			// Memory resource is for MISC FCH register set.
			// It is needed for enabling the clock.
			Memory32Fixed(ReadWrite, MISC_MMIO_BASE, 0x100)
		})

		Return (RBUF)
	}
}
