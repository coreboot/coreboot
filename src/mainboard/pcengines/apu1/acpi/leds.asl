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
	Device (LEDS)
	{
		Name (_HID, "PRP0001")

		Name (_CRS, ResourceTemplate () {
			GpioIo (Exclusive, PullUp, 0, 0, IoRestrictionOutputOnly,
				"\\_SB.PCI0.SBUS.GPIO", 0, ResourceConsumer) {189, 190, 191}
		})

		Name (_DSD, Package () {
			ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package () {"compatible", Package () {"gpio-leds"}},
			}
		})

		Device (LED1)
		{
			Name (_HID, "PRP0001")
			Name (_DSD, Package () {
				ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
				Package () {
					/*
					 * From Linux Documentation/leds/leds-class.txt:
					 * LED Device Naming
					 * Is currently of the form:
					 * "devicename:colour:function"
					 */
					Package () {"label", "apu1:green:led1"},
					Package () {"gpios", Package () {^^LEDS, 0, 0, 1 /* low-active */}},
					Package () {"default-state", "keep"},
				} }) }

		Device (LED2)
		{
			Name (_HID, "PRP0001")
			Name (_DSD, Package () {
				ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
				Package () {
					Package () {"label", "apu1:green:led2"},
					Package () {"gpios", Package () {^^LEDS, 0, 1, 1 /* low-active */}},
					Package () {"default-state", "keep"},
				}
			})
		}

		Device (LED3)
		{
			Name (_HID, "PRP0001")
			Name (_DSD, Package () {
				ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
				Package () {
					Package () {"label", "apu1:green:led3"},
					Package () {"gpios", Package () {^^LEDS, 0, 2, 1 /* low-active */}},
					Package () {"default-state", "keep"},
				}
			})
		}
	}
}
