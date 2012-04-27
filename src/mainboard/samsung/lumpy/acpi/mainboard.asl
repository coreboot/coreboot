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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

Scope (\_SB) {
	Device (LID0)
	{
		Name(_HID, EisaId("PNP0C0D"))
		Method(_LID, 0)
		{
			Store (\_SB.PCI0.LPCB.EC0.LIDS, \LIDS)
			Return (\LIDS)
		}

		// WAKE SCI# from EC is GPIO14
		Name(_PRW, Package(){0x1e, 0x05})
	}

	Device (PWRB)
	{
		Name(_HID, EisaId("PNP0C0C"))
	}

	Device (TPAD)
	{
		// Report as a Sleep Button device so
		// Linux will automatically enable for wake
		Name(_HID, EisaId("PNP0C0E"))

		// Trackpad Wake is GPIO11
		Name(_PRW, Package(){0x1b, 0x03})

		Name(_CRS, ResourceTemplate()
		{
			// PIRQF -> GSI21
			Interrupt (ResourceConsumer, Edge, ActiveLow) {21}

			// SMBUS Address 0x67
			VendorShort (ADDR) {0x67}
		})
	}

	Device (LITE)
	{
		// Generic ACPI Device Container
		Name(_HID, EisaId("PNP0A05"))

		Name(_CRS, ResourceTemplate()
		{
			// PIRQE -> GSI20
			Interrupt (ResourceConsumer, Edge, ActiveLow) {20}

			// SMBUS Address 0x44
			VendorShort (ADDR) {0x44}
		})
	}
}

// Battery information
Name (BATV, "SAMSUNG")
Name (BATM, "LUMPY")
Name (BATS, "BATTERY")
