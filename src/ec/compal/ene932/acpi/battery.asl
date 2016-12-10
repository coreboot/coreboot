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
 */

// Scope (EC0)

Device (BATX)
{
	Name (_HID, EISAID ("PNP0C0A"))
	Name (_UID, 1)
	Name (_PCL, Package () { \_SB })

	Name (PBIF, Package () {
		0x00000001,  // Power Unit: mAh
		0xFFFFFFFF,  // Design Capacity
		0xFFFFFFFF,  // Last Full Charge Capacity
		0x00000001,  // Battery Technology: Rechargeable
		0xFFFFFFFF,  // Design Voltage
		0x00000003,  // Design Capacity of Warning
		0xFFFFFFFF,  // Design Capacity of Low
		0x00000001,  // Capacity Granularity 1
		0x00000001,  // Capacity Granularity 2
		"",          // Model Number
		"",          // Serial Number
		"LION",      // Battery Type
		""           // OEM Information
	})

	Name (PBST, Package () {
		0x00000000,  // Battery State
		0xFFFFFFFF,  // Battery Present Rate
		0xFFFFFFFF,  // Battery Remaining Capacity
		0xFFFFFFFF,  // Battery Present Voltage
	})
	Name (BTNM, Zero)  // Battery number

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
		If (BOL0) {
			Return (0x1F)
		} Else {
			Return (0x0F)
		}
	}

	Method (_BIF, 0, Serialized)
	{
		// Update fields from EC
		Store (BAM0, Index (PBIF, 0))
		Store (BDC0, Index (PBIF, 1))
		Store (BFC0, Index (PBIF, 2))
		Store (BDV0, Index (PBIF, 4))
		Divide(BFC0, 0x64, , Local1)
		Multiply(Local1, 0x0A, Local1)
		Store(Local1, Index(PBIF, 5))
		Divide(BFC0, 0x64, , Local1)
		Multiply(Local1, 0x03, Local1)
		Store (Local1, Index (PBIF, 6))

		Store (ToString(Concatenate(BATD, 0x00)), Index (PBIF, 9))  // model
		Store (ToHexString(BSN0), Index (PBIF, 10))  // serial
		Store (ToString(BMFN), Index (PBIF, 12))  // vendor

		Store(BDN0, BTNM)	// Save the battery number

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

		// Get battery state from EC and save it for the charging workaround
		Store (BST0, Local0)
		Store (Local0, Index (PBST, 0))

		//
		// 1: BATTERY PRESENT RATE/CURRENT
		//

		Store (BAC0, Local1)
		Subtract(0xFFFF, Local1, Local1)
		Store (Local1, Index (PBST, 1))

		//
		// 2: BATTERY REMAINING CAPACITY
		//
		Multiply(BFC0, GAU0, Local1)
		Divide(Local1, 0x64, Local2, Local1)

		If (LAnd (BFWK, LAnd (ADPT, LNot (Local0)))) {
			// On AC power and battery is neither charging
			// nor discharging.  Linux expects a full battery
			// to report same capacity as last full charge.
			// https://bugzilla.kernel.org/show_bug.cgi?id=12632
			Store (GAU0, Local2)

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

		// Check the Battery Number
		If(LNotEqual(BDN0, BTNM)) {
			Notify(BATX, 0x81)
		}

		Return (PBST)
	}
}
