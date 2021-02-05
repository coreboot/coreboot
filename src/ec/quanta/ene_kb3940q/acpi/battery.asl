/* SPDX-License-Identifier: GPL-2.0-only */

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
	Name (BFWK, 1)

	// Method to enable full battery workaround
	Method (BFWE)
	{
		BFWK = 1
	}

	// Method to disable full battery workaround
	Method (BFWD)
	{
		BFWK = 0
	}

	// Device insertion/removal control method that returns a device's status.
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
		PBIF [1] = BDC0 // Batt Design Capacity
		PBIF [2] = BFC0 // Batt Last Full Charge Capacity
		PBIF [4] = BDV0 // Batt Design Voltage
		Local1 = BFC0 / 100
		Local0 = Local1 * 10
		PBIF [5] = Local0
		Local0 = Local1 * 5
		PBIF [6] = Local0
		PBIF [9] = ToString(Concatenate(BATD, 0x00))	// Model Number
		PBIF [10] = ToDecimalString(BSN0)		// Serial Number
		PBIF [11] = ToString(Concatenate(BCHM, 0x00))	// Battery Type
		PBIF [12] = \BATV				// OEM information

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
		Local0 = BST0
		PBST [0] = Local0

		//
		// 1: BATTERY PRESENT RATE/CURRENT
		//
		Local1 = BPC0
		If (Local1 && 0x8000) {
			Local1 ^= 0xffff
			Local1++
		}
		PBST [1] = Local1

		//
		// 2: BATTERY REMAINING CAPACITY
		//
		Local1 = BRC0

		If (BFWK &&  ADPT && !Local0) {
			// On AC power and battery is neither charging
			// nor discharging.  Linux expects a full battery
			// to report same capacity as last full charge.
			// https://bugzilla.kernel.org/show_bug.cgi?id=12632
			Local2 = BFC0

			// See if within ~3% of full
			Local3 = Local2 >> 5
			If (Local1 > (Local2 - Local3) && Local1 < (Local2 + Local3))
			{
				Local1 = Local2
			}
		}
		PBST [2] = Local1

		//
		// 3: BATTERY PRESENT VOLTAGE
		//
		PBST [3] = BPV0

		Return (PBST)
	}
}
