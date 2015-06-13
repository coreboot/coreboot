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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

Scope (\_SB.PCI0.SBUS)
{
	Device (GPIO)
	{
		Name (_HID, "PRP0001")

		Name (_CRS, ResourceTemplate () {
			/* ACPI_MMIO_BASE + gpio offset */
			Memory32Fixed(ReadWrite, 0xFED80100, 0x0000100)
		})

		Name (_DSD, Package () {
			ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package () {"compatible", Package () {"gpio-sb8xx"}},
			}
		})
	}
}
