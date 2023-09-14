/* SPDX-License-Identifier: GPL-2.0-only */

Device (BAT0)
{
	Name (_HID, EisaId ("PNP0C0A") /* Control Method Battery */)  // _HID: Hardware ID
	Name (_UID, Zero)  // _UID: Unique ID
	Name (_PCL, Package (0x01)  // _PCL: Power Consumer List
	{
		_SB
	})

	Name (BTEX, Zero)

	// Test if EC0 is ready, and if it is, if the battery is present
	Method (BTOK, 0, NotSerialized)
	{
		If (^^ECOK) {
			If (BTEX) {
				Return (One)
			}
		}

		Return (Zero)
	}

	Method (_STA, 0, NotSerialized)  // _STA: Status
	{
		If (BTOK ()) {
			Printf ("BAT0: _STA: present")
			Return (0x1F)
		}

		Printf ("BAT0: _STA: not present")
		Return (0x0F)
	}

	Name (PBIF, Package (0x0D)	// Persistent battery information
	{
		One, // 0 - Power Unit - mA/mAh
		0xFFFFFFFF, // 1 - Design Capacity
		0xFFFFFFFF, // 2 - Last Full Charge Capacity
		One, // 3 - Battery Technology
		0xFFFFFFFF, // 4 - Design Voltage
		Zero, // 5 - Design Capacity of Warning
		Zero, // 6 - Design Capacity of Low
		Zero, // 7 - Battery Capacity Granularity 1
		Zero, // 8 - Battery Capacity Granularity 2
		"", // 9 - Model Number
		"", // 10 - Serial Number
		"", // 11 - Battery Type
		"" // 12 - OEM Information
	})

	Method (IVBI, 0, NotSerialized)	// Set invalid battery information
	{
		PBIF [1] = 0xFFFFFFFF
		PBIF [2] = 0xFFFFFFFF
		PBIF [5] = Zero
		PBIF [6] = Zero
		PBIF [7] = Zero
		PBIF [8] = Zero
		PBIF [9] = ""
		PBIF [10] = ""
		PBIF [11] = ""
	}

	Method (UPBI, 0, Serialized)	// Update battery information
	{
		If (BTOK ()) {
			Local0 = ^^BTDC // design cap
			Local1 = ^^BTFC // last full capacity
			// Design capacity
			PBIF [1] = Local0
			// Last full charge capacity
			PBIF [2] = Local1
			// Warn/low capacities - 15% and 10% of design capacity
			PBIF [5] = Local1 * 15 / 100
			PBIF [6] = Local1 * 10 / 100
			// Granularity is 1% of design capacity
			PBIF [7] = Local0 / 100
			PBIF [8] = Local0 / 100
			PBIF [9] = "BAT"
			PBIF [10] = "0001"
			PBIF [11] = "LION"
		}
		Else {
			IVBI ()
		}
	}

	Method (_BIF, 0, NotSerialized)  // _BIF: Battery Information
	{
		UPBI ()
		Return (PBIF)
	}

	Name (PBST, Package (0x04)	// Persistent battery state
	{
		Zero, // 0 - Battery state
		0xFFFFFFFF, // 1 - Battery present rate
		0xFFFFFFFF, // 2 - Battery remaining capacity
		0xFFFFFFFF // 3 - Battery present voltage
	})

	Method (IVBS, 0, NotSerialized)	// Invalid battery state
	{
		PBST [0] = Zero
		PBST [1] = 0xFFFFFFFF
		PBST [2] = 0xFFFFFFFF
		PBST [3] = 0xFFFFFFFF
	}

	Method (UPBS, 0, Serialized)
	{
		If (BTOK ()) {
			// Status flags - 3 bits; this EC does not report the
			// charge limiting state
			PBST [0] = ^^BTST
			// Present rate
			PBST [1] = ^^BTCR
			// Remaining capacity
			PBST [2] = ^^BTRC
			// Present voltage
			PBST [3] = ^^BTVT
		}
		Else {
			IVBS ()
		}
	}

	Method (_BST, 0, NotSerialized)  // _BST: Battery Status
	{
		UPBS ()
		Return (PBST)
	}
}
