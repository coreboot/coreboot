/* SPDX-License-Identifier: GPL-2.0-only */

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
		PBIF [0] = BAM0
		PBIF [1] = BDC0
		PBIF [2] = BFC0
		PBIF [4] = BDV0
		Local1 = BFC0 / 100
		Local1 *= 10
		PBIF [5] = Local1
		Local1 = BFC0 / 100
		Local1 *= 3
		PBIF [6] = Local1

		PBIF [9] = ToString (Concatenate (BATD, 0x00))  // model
		PBIF [10] = ToHexString (BSN0)  // serial
		PBIF [12] = ToString (BMFN)  // vendor

		BTNM = BDN0	// Save the battery number

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
		Local0 = BST0
		PBST [0] = Local0

		//
		// 1: BATTERY PRESENT RATE/CURRENT
		//

		Local1 = BAC0
		Local1 = 0xFFFF - Local1
		PBST [1] = Local1

		//
		// 2: BATTERY REMAINING CAPACITY
		//
		Local1 = BFC0 * GAU0
		Local2 = Local1 % 100
		Local1 /= 100

		If (BFWK && ADPT && !Local0) {
			// On AC power and battery is neither charging
			// nor discharging.  Linux expects a full battery
			// to report same capacity as last full charge.
			// https://bugzilla.kernel.org/show_bug.cgi?id=12632
			Local2 = GAU0

			// See if within ~3% of full
			Local3 = Local2 >> 5
			If ((Local1 > (Local2 - Local3)) && (Local1 < (Local2 + Local3)))
			{
				Local1 = Local2
			}
		}
		PBST [2] = Local1

		//
		// 3: BATTERY PRESENT VOLTAGE
		//
		PBST [3] = BPV0

		// Check the Battery Number
		If(BDN0 != BTNM) {
			Notify(BATX, 0x81)
		}

		Return (PBST)
	}
}
