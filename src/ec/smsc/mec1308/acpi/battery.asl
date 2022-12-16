/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * The mainboard must define strings in the root scope to
 * report device-specific battery information to the OS.
 *
 *  BATM: Model
 *  BATS: Serial
 *  BATV: Vendor
 */

// Scope (EC0)

Device (BAT0)
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
	Name (BSTP, Zero)

	// Workaround for full battery status, enabled by default
	Name (BFWK, One)

	// Method to enable full battery workaround
	Method (BFWE)
	{
		BFWK = One
	}

	// Method to disable full battery workaround
	Method (BFWD)
	{
		BFWK = Zero
	}

	// Swap bytes in a word
	Method (SWAB, 1, NotSerialized)
	{
		ShiftRight (Arg0, 8, Local0)
		ShiftLeft (Arg0, 8, Local1)
		And (Local1, 0xFF00, Local1)
		Or (Local0, Local1, Local0)
		If (Local0 == 0xFFFF) {
			Local0 = 0xFFFFFFFF
		}
		Return (Local0)
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
		// Update fields from EC
		PBIF[1] = SWAB (BTDA)
		PBIF[2] = SWAB (BTDF)
		PBIF[4] = SWAB (BTDV)
		PBIF[6] = SWAB (BTDL)

		// Get battery info from mainboard
		PBIF[9] = \BATM
		PBIF[10] = \BATS
		PBIF[12] = \BATV

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
		Local0 = BTST
		Local1 = Zero

		// Check if AC is present
		If (ACEX) {
			// Set only charging/discharging bits
			And (Local0, 0x03, Local1)
		} Else {
			// Always discharging when on battery power
			Local1 = 0x01
		}

		// Flag if the battery level is critical
		And (Local0, 0x04, Local4)
		Or (Local1, Local4, Local1)
		PBST[0] = Local1

		// Notify if battery state has changed since last time
		If (Local1 != BSTP) {
			BSTP = Local1
			Notify (BAT0, 0x80)
		}

		//
		// 1: BATTERY PRESENT RATE
		//

		Local1 = SWAB (BTPR)
		If (Local1 != 0xFFFFFFFF && Local1 >= 0x8000) {
			Local1 ^= 0xFFFF
			Local1++
		}
		PBST[1] = Local1

		//
		// 2: BATTERY REMAINING CAPACITY
		//
		Local1 = SWAB (BTRA)
		If (Local1 != 0xFFFFFFFF && Local1 >= 0x8000) {
			Local1 ^= 0xFFFF
			Local1++
		}

		If (BFWK && ACEX && !Local0) {
			// On AC power and battery is neither charging
			// nor discharging.  Linux expects a full battery
			// to report same capacity as last full charge.
			// https://bugzilla.kernel.org/show_bug.cgi?id=12632
			Local2 = SWAB (BTDF)

			// See if within ~3% of full
			ShiftRight (Local2, 5, Local3)
			If (Local1 > Local2 - Local3 && Local1 < Local2 + Local3)
			{
				Local1 = Local2
			}
		}
		PBST[2] = Local1

		//
		// 3: BATTERY PRESENT VOLTAGE
		//
		PBST[3] = SWAB (BTVO)

		Return (PBST)
	}
}
