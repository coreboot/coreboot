/*
 * This file is part of the coreboot project.
 *
 * Based on the example of Mika Westerberg: https://lwn.net/Articles/612062/
 *
 * Copyright (C) 2015 Tobias Diedrich, Mika Westerberg
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

Scope (\_SB.PCI0.SBUS)
{
	Device (BTNS)
	{
		Name (_HID, "PRP0001")

		Name (_CRS, ResourceTemplate () {
			GpioIo (Exclusive, PullUp, 0, 0, IoRestrictionInputOnly,
				"\\_SB.PCI0.SBUS.GPIO", 0, ResourceConsumer) {187}
		})

		Name (_DSD, Package () {
			ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package () {"compatible", Package () {"gpio-keys-polled"}},
				Package () {"poll-interval", 100},
				Package () {"autorepeat", 1}
			}
		})

		Device (BTN1)
		{
			Name (_HID, "PRP0001")
			Name (_DSD, Package () {
				ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
				Package () {
					/* BTN_1 is 0x101 in linux/input.h */
					Package () {"linux,code", 257},
					Package () {"linux,input-type", 1},
					/* labeled S1 on the board, MODESW in the gpio header files */
					Package () {"label", "switch1"},
					Package () {"gpios", Package () {^^BTNS, 0, 0, 1 /* low-active */}},
				}
			})
		}
	}
}
