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
		Store (One, BFWK)
	}

	// Method to disable full battery workaround
	Method (BFWD)
	{
		Store (Zero, BFWK)
	}

	// Swap bytes in a word
	Method (SWAB, 1, NotSerialized)
	{
		ShiftRight (Arg0, 8, Local0)
		ShiftLeft (Arg0, 8, Local1)
		And (Local1, 0xFF00, Local1)
		Or (Local0, Local1, Local0)
		If (Local0 == 0xFFFF) {
			Store (0xFFFFFFFF, Local0)
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
		Store (SWAB (BTDA), PBIF[1])
		Store (SWAB (BTDF), PBIF[2])
		Store (SWAB (BTDV), PBIF[4])
		Store (SWAB (BTDL), PBIF[6])

		// Get battery info from mainboard
		Store (\BATM, PBIF[9])
		Store (\BATS, PBIF[10])
		Store (\BATV, PBIF[12])

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
		Store (BTST, Local0)
		Store (Zero, Local1)

		// Check if AC is present
		If (ACEX) {
			// Set only charging/discharging bits
			And (Local0, 0x03, Local1)
		} Else {
			// Always discharging when on battery power
			Store (0x01, Local1)
		}

		// Flag if the battery level is critical
		And (Local0, 0x04, Local4)
		Or (Local1, Local4, Local1)
		Store (Local1, PBST[0])

		// Notify if battery state has changed since last time
		If (Local1 != BSTP) {
			Store (Local1, BSTP)
			Notify (BAT0, 0x80)
		}

		//
		// 1: BATTERY PRESENT RATE
		//

		Store (SWAB (BTPR), Local1)
		If (Local1 != 0xFFFFFFFF && Local1 >= 0x8000) {
			Xor (Local1, 0xFFFF, Local1)
			Local1++
		}
		Store (Local1, PBST[1])

		//
		// 2: BATTERY REMAINING CAPACITY
		//
		Store (SWAB (BTRA), Local1)
		If (Local1 != 0xFFFFFFFF && Local1 >= 0x8000) {
			Xor (Local1, 0xFFFF, Local1)
			Local1++
		}

		If (BFWK && ACEX && !Local0) {
			// On AC power and battery is neither charging
			// nor discharging.  Linux expects a full battery
			// to report same capacity as last full charge.
			// https://bugzilla.kernel.org/show_bug.cgi?id=12632
			Store (SWAB (BTDF), Local2)

			// See if within ~3% of full
			ShiftRight (Local2, 5, Local3)
			If (LGreater (Local1, Local2 - Local3) && Local1 < Local2 + Local3)
			{
				Store (Local2, Local1)
			}
		}
		Store (Local1, PBST[2])

		//
		// 3: BATTERY PRESENT VOLTAGE
		//
		Store (SWAB (BTVO), PBST[3])

		Return (PBST)
	}
}
