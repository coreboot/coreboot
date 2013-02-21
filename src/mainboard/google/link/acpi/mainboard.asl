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

		// EC_LID_OUT is GPIO15
		Name(_PRW, Package(){0x1f, 0x05})
	}

	Device (PWRB)
	{
		Name(_HID, EisaId("PNP0C0C"))
	}

	Device (TPAD)
	{
		Name (_ADR, 0x0)
		Name (_UID, 1)

		// Report as a Sleep Button device so Linux will
		// automatically enable it as a wake source
		Name(_HID, EisaId("PNP0C0E"))

		// Trackpad Wake is GPIO12
		Name(_PRW, Package(){0x1c, 0x03})

		Name(_CRS, ResourceTemplate()
		{
			// PIRQE -> GSI20
			Interrupt (ResourceConsumer, Edge, ActiveLow) {20}

			// SMBUS Address 0x4b
			VendorShort (ADDR) {0x4b}
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

	// Keyboard Backlight interface via EC
	Device (KBLT) {
		Name (_HID, EisaId("GGL0002"))
		Name (_UID, 1)
		Name (_ADR, 0)

		// Read current backlight value
		Method (KBQC, 0)
		{
			Return (\_SB.PCI0.LPCB.EC0.KBLV)
		}

		// Write new backlight value
		Method (KBCM, 1)
		{
			Store (Arg0, \_SB.PCI0.LPCB.EC0.KBLV)
		}
	}
}
