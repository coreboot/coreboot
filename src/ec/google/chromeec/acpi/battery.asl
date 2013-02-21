/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The Chromium OS Authors. All rights reserved.
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

// Scope (EC0)

Device (BAT0)
{
	Name (_HID, EISAID ("PNP0C0A"))
	Name (_UID, 1)
	Name (_PCL, Package () { \_SB })

	Name (PBIF, Package () {
		0x00000001,  // 0x00: Power Unit: mAh
		0xFFFFFFFF,  // 0x01: Design Capacity
		0xFFFFFFFF,  // 0x02: Last Full Charge Capacity
		0x00000001,  // 0x03: Battery Technology: Rechargeable
		0xFFFFFFFF,  // 0x04: Design Voltage
		0x00000003,  // 0x05: Design Capacity of Warning
		0xFFFFFFFF,  // 0x06: Design Capacity of Low
		0x00000001,  // 0x07: Capacity Granularity 1
		0x00000001,  // 0x08: Capacity Granularity 2
		"",          // 0x09: Model Number
		"",          // 0x0a: Serial Number
		"LION",      // 0x0b: Battery Type
		""           // 0x0c: OEM Information
	})

	Name (PBIX, Package () {
		0x00000000,  // 0x00: Revision
		0x00000001,  // 0x01: Power Unit: mAh
		0xFFFFFFFF,  // 0x02: Design Capacity
		0xFFFFFFFF,  // 0x03: Last Full Charge Capacity
		0x00000001,  // 0x04: Battery Technology: Rechargeable
		0xFFFFFFFF,  // 0x05: Design Voltage
		0x00000003,  // 0x06: Design Capacity of Warning
		0xFFFFFFFF,  // 0x07: Design Capacity of Low
		0x00000000,  // 0x08: Cycle Count
		0x00018000,  // 0x09: Measurement Accuracy (98.3%?)
		0x000001F4,  // 0x0a: Max Sampling Time (500ms)
		0x0000000a,  // 0x0b: Min Sampling Time (10ms)
		0xFFFFFFFF,  // 0x0c: Max Averaging Interval
		0xFFFFFFFF,  // 0x0d: Min Averaging Interval
		0x00000001,  // 0x0e: Capacity Granularity 1
		0x00000001,  // 0x0f: Capacity Granularity 2
		"",          // 0x10 Model Number
		"",          // 0x11: Serial Number
		"LION",      // 0x12: Battery Type
		""           // 0x13: OEM Information
	})

	Name (PBST, Package () {
		0x00000000,  // 0x00: Battery State
		0xFFFFFFFF,  // 0x01: Battery Present Rate
		0xFFFFFFFF,  // 0x02: Battery Remaining Capacity
		0xFFFFFFFF,  // 0x03: Battery Present Voltage
	})
	Name (BSTP, Zero)

	// Workaround for full battery status, enabled by default
	Name (BFWK, One)

	// Method to enable full battery workaround
	Method (BFWE)
	{
		Store (One, BFWK)
	}

	// Method to disable full battery workaround
	Method (BFWD)
	{
		Store (Zero, BFWK)
	}

	Method (_STA, 0, Serialized)
	{
		If (BTEX) {
			Return (0x1F)
		} Else {
			Return (0x0F)
		}
	}

	Method (_BIF, 0, Serialized)
	{
		// Last Full Charge Capacity
		Store (BTDF, Index (PBIF, 2))

		// Design Voltage
		Store (BTDV, Index (PBIF, 4))

		// Design Capacity
		Store (BTDA, Local0)
		Store (Local0, Index (PBIF, 1))

		// Design Capacity of Warning
		Divide (Multiply (Local0, DWRN), 100, Local1, Local2)
		Store (Local2, Index (PBIF, 5))

		// Design Capacity of Low
		Divide (Multiply (Local0, DLOW), 100, Local1, Local2)
		Store (Local2, Index (PBIF, 6))

		// Get battery info from mainboard
		Store (ToString(BMOD), Index (PBIF, 9))
		Store (ToString(BSER), Index (PBIF, 10))
		Store (ToString(BMFG), Index (PBIF, 12))

		Return (PBIF)
	}

	// Extended Battery info method is disabled for now due to
	// a bug in the Linux kernel: http://crosbug.com/28747
	Method (XBIX, 0, Serialized)
	{
		// Last Full Charge Capacity
		Store (BTDF, Index (PBIX, 3))

		// Design Voltage
		Store (BTDV, Index (PBIX, 5))

		// Design Capacity
		Store (BTDA, Local0)
		Store (Local0, Index (PBIX, 2))

		// Design Capacity of Warning
		Divide (Multiply (Local0, DWRN), 100, Local1, Local2)
		Store (Local2, Index (PBIX, 6))

		// Design Capacity of Low
		Divide (Multiply (Local0, DLOW), 100, Local1, Local2)
		Store (Local2, Index (PBIX, 7))

		// Cycle Count
		Store (BTCC, Index (PBIX, 8))

		// Get battery info from mainboard
		Store (ToString(BMOD), Index (PBIX, 16))
		Store (ToString(BSER), Index (PBIX, 17))
		Store (ToString(BMFG), Index (PBIX, 19))

		Return (PBIX)
	}

	Method (_BST, 0, Serialized)
	{
		//
		// 0: BATTERY STATE
		//
		// bit 0 = discharging
		// bit 1 = charging
		// bit 2 = critical level
		//
		Store (Zero, Local1)

		// Check if AC is present
		If (ACEX) {
			If (BFCG) {
				Store (0x02, Local1)
			} ElseIf (BFDC) {
				Store (0x01, Local1)
			}
		} Else {
			// Always discharging when on battery power
			Store (0x01, Local1)
		}

		// Check for critical battery level
		If (BFCR) {
			Or (Local1, 0x04, Local1)
		}
		Store (Local1, Index (PBST, 0))

		// Notify if battery state has changed since last time
		If (LNotEqual (Local1, BSTP)) {
			Store (Local1, BSTP)
			Notify (BAT0, 0x80)
		}

		//
		// 1: BATTERY PRESENT RATE
		//
		Store (BTPR, Index (PBST, 1))

		//
		// 2: BATTERY REMAINING CAPACITY
		//
		Store (BTRA, Local1)
		If (LAnd (BFWK, LAnd (ACEX, LNot (LAnd (BFDC, BFCG))))) {
			// On AC power and battery is neither charging
			// nor discharging.  Linux expects a full battery
			// to report same capacity as last full charge.
			// https://bugzilla.kernel.org/show_bug.cgi?id=12632
			Store (BTDF, Local2)

			// See if within ~3% of full
			ShiftRight (Local2, 5, Local3)
			If (LAnd (LGreater (Local1, Subtract (Local2, Local3)),
			          LLess (Local1, Add (Local2, Local3))))
			{
				Store (Local2, Local1)
			}
		}
		Store (Local1, Index (PBST, 2))

		//
		// 3: BATTERY PRESENT VOLTAGE
		//
		Store (BTVO, Index (PBST, 3))

		Return (PBST)
	}
}
