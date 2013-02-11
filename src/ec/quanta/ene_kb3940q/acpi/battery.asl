/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011-2012 The Chromium OS Authors. All rights reserved.
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

/*
 * The mainboard must define strings in the root scope to
 * report device-specific battery information to the OS.
 *
 *  BATV: Vendor
 */

// Scope (EC0)

Device (BATX)
{
	Name (_HID, EISAID ("PNP0C0A"))
	Name (_UID, 1)
	Name (_PCL, Package () { \_SB })

	Name (PBIF, Package () {
		0x00000001,  // 0 Power Unit: mAh
		0xFFFFFFFF,  // 1 Design Capacity
		0xFFFFFFFF,  // 2 Last Full Charge Capacity
		0x00000001,  // 3 Battery Technology: Rechargeable
		0xFFFFFFFF,  // 4 Design Voltage
		0x000000FA,  // 5 Design Capacity of Warning
		0x00000096,  // 6 Design Capacity of Low
		0x0000000A,  // 7 Capacity Granularity 1
		0x00000019,  // 8 Capacity Granularity 2
		"",          // 9 Model Number
		"",          // 10 Serial Number
		"",          // 11 Battery Type
		""           // 12 OEM Information
	})

	Name (PBST, Package () {
		0x00000000,  // Battery State
		0xFFFFFFFF,  // Battery Present Rate
		0xFFFFFFFF,  // Battery Remaining Capacity
		0xFFFFFFFF,  // Battery Present Voltage
	})

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

	// Device insertion/removal control method that returns a device’s status.
	// Power resource object that evaluates to the current on or off state of
	// the Power Resource.
	Method (_STA, 0, Serialized)
	{
		If (BTIN) {
			Return (0x1F)
		} Else {
			Return (0x0F)
		}
	}

	Method (_BIF, 0, Serialized)
	{
		// Update fields from EC
		Store (BDC0, Index (PBIF, 1)) // Batt Design Capacity
		Store (BFC0, Index (PBIF, 2)) // Batt Last Full Charge Capacity
		Store (BDV0, Index (PBIF, 4)) // Batt Design Voltage
		Divide(BFC0, 0x64, Local0, Local1)
		Multiply(Local1, 0x0A, Local0)
		Store(Local0, Index(PBIF, 5))
		Multiply(Local1, 0x05, Local0)
		Store (Local0, Index (PBIF, 6))
		Store (ToString(BATD), Index (PBIF, 9)) // Model Number
		Store (ToDecimalString(BSN0), Index (PBIF, 10)) // Serial Number
		Store (ToString(BCHM), Index (PBIF, 11)) // Battery Type
		Store (\BATV, Index (PBIF, 12)) // OEM information

		Return (PBIF)
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

		// Get battery state from EC
		Store (BST0, Local0)
		Store (Local0, Index (PBST, 0))

		//
		// 1: BATTERY PRESENT RATE/CURRENT
		//
		Store (BPC0, Local1)
		If (LAnd (Local1, 0x8000)) {
			Xor (Local1, 0xFFFF, Local1)
			Increment (Local1)
		}
		Store (Local1, Index (PBST, 1))

		//
		// 2: BATTERY REMAINING CAPACITY
		//
		Store (BRC0, Local1)

		If (LAnd (BFWK, LAnd (ADPT, LNot (Local0)))) {
			// On AC power and battery is neither charging
			// nor discharging.  Linux expects a full battery
			// to report same capacity as last full charge.
			// https://bugzilla.kernel.org/show_bug.cgi?id=12632
			Store (BFC0, Local2)

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
		Store (BPV0, Index (PBST, 3))

		Return (PBST)
	}
}
