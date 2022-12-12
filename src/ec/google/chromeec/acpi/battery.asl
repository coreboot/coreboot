/* SPDX-License-Identifier: GPL-2.0-only */

// Scope (EC0)

/* Mutex for EC battery index interface */
Mutex (BATM, 0)

// Wait for desired battery index to be presented in shared memory
//   Arg0 = battery index
//   Returns Zero on success, One on error.
Method (BTSW, 1)
{
#ifdef EC_ENABLE_SECOND_BATTERY_DEVICE
	If (BTIX == Arg0) {
		Return (Zero)
	}
	If (Arg0 >= BTCN) {
		Return (One)
	}
	\_SB.PCI0.LPCB.EC0.BTID = Arg0
	Local0 = 5      // Timeout 5 msec
	While (BTIX != Arg0)
	{
		Sleep (1)
		Local0--
		If (Local0 == 0)
		{
			Return (One)
		}
	}
#else
	If (Arg0 != 0) {
		Return (One)
	}
#endif
	Return (Zero)
}

// _STA implementation.
//   Arg0 = battery index
Method (BSTA, 1, Serialized)
{
	If (Acquire (^BATM, 1000)) {
		Return (Zero)
	}

	If (And(Not(BTSW (Arg0)), BTEX)) {
		Local0 = 0x1F
	} Else {
		Local0 = 0x0F
	}

	Release (^BATM)
	Return (Local0)
}

// _BIF implementation.
//   Arg0 = battery index
//   Arg1 = PBIF
Method (BBIF, 2, Serialized)
{
	If (Acquire (^BATM, 1000)) {
		Return (Arg1)
	}

	If (BTSW (Arg0)) {
		Release (^BATM)
		Return (Arg1)
	}
	// Last Full Charge Capacity
	Arg1[2] = BTDF

	// Design Voltage
	Arg1[4] = BTDV

	// Design Capacity
	Local0 = BTDA
	Arg1[1] = Local0

	// Design Capacity of Warning
	Local2 = Local0 * DWRN / 100
	Arg1[5] = Local2

	// Design Capacity of Low
	Local2 = Local0 * DLOW / 100
	Arg1[6] = Local2

	// Get battery info from mainboard
	Arg1[9] = ToString(Concatenate(BMOD, 0x00))
	Arg1[10] = ToString(Concatenate(BSER, 0x00))
	Arg1[12] = ToString(Concatenate(BMFG, 0x00))

	Release (^BATM)
	Return (Arg1)
}

// _BIX implementation.
//   Arg0 = battery index
//   Arg1 = PBIX
Method (BBIX, 2, Serialized)
{
	If (Acquire (^BATM, 1000)) {
		Return (Arg1)
	}

	If (BTSW (Arg0)) {
		Release (^BATM)
		Return (Arg1)
	}
	// Last Full Charge Capacity
	Arg1[3] = BTDF

	// Design Voltage
	Arg1[5] = BTDV

	// Design Capacity
	Local0 = BTDA
	Arg1[2] = Local0

	// Design Capacity of Warning
	Local2 = Local0 * DWRN / 100
	Arg1[6] = Local2

	// Design Capacity of Low
	Local2 = Local0 * DLOW / 100
	Arg1[7] = Local2

	// Cycle Count
	Arg1[8] = BTCC

	// Get battery info from mainboard
	Arg1[16] = ToString(Concatenate(BMOD, 0x00))
	Arg1[17] = ToString(Concatenate(BSER, 0x00))
	Arg1[19] = ToString(Concatenate(BMFG, 0x00))

	Release (^BATM)
	Return (Arg1)
}

// _BST implementation.
//   Arg0 = battery index
//   Arg1 = PBST
//   Arg2 = BSTP
//   Arg3 = BFWK
Method (BBST, 4, Serialized)
{
	If (Acquire (^BATM, 1000)) {
		Return (Arg1)
	}

	If (BTSW (Arg0)) {
		Release (^BATM)
		Return (Arg1)
	}
	//
	// 0: BATTERY STATE
	//
	// bit 0 = discharging
	// bit 1 = charging
	// bit 2 = critical level
	//
	Local1 = Zero

	// Check if AC is present
	If (ACEX) {
		If (BFCG) {
			Local1 = 0x02
		} ElseIf (BFDC) {
			Local1 = 0x01
		}
	} Else {
		// Always discharging when on battery power
		Local1 = 0x01
	}

	// Check for critical battery level
	If (BFCR) {
		Or (Local1, 0x04, Local1)
	}
	Arg1[0] = Local1

	// Notify if battery state has changed since last time
	If (Local1 != DeRefOf (Arg2)) {
		Arg2 = Local1
		If (Arg0 == 0) {
			Notify (BAT0, 0x80)
		}
#ifdef EC_ENABLE_SECOND_BATTERY_DEVICE
		Else {
			Notify (BAT1, 0x80)
		}
#endif
	}

	//
	// 1: BATTERY PRESENT RATE
	//
	Arg1[1] = BTPR

	//
	// 2: BATTERY REMAINING CAPACITY
	//
	Local1 = BTRA
	If (Arg3 && ACEX && !(BFDC && BFCG)) {
		// On AC power and battery is neither charging
		// nor discharging.  Linux expects a full battery
		// to report same capacity as last full charge.
		// https://bugzilla.kernel.org/show_bug.cgi?id=12632
		Local2 = BTDF

		// See if within ~6% of full
		ShiftRight (Local2, 4, Local3)
		If (Local1 > Local2 - Local3 && Local1 < Local2 + Local3)
		{
			Local1 = Local2
		}
	}
	Arg1[2] = Local1

	//
	// 3: BATTERY PRESENT VOLTAGE
	//
	Arg1[3] = BTVO

	Release (^BATM)
	Return (Arg1)
}

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

	// Workaround for full battery status, disabled by default
	Name (BFWK, Zero)

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

	Method (_STA, 0, Serialized)
	{
		Return (BSTA (0))
	}

	Method (_BIF, 0, Serialized)
	{
		Return (BBIF (0, PBIF))
	}

	Method (_BIX, 0, Serialized)
	{
		Return (BBIX (0, PBIX))
	}

	Method (_BST, 0, Serialized)
	{
		Return (BBST (0, PBST, RefOf (BSTP), BFWK))
	}
}

#ifdef EC_ENABLE_SECOND_BATTERY_DEVICE
Device (BAT1)
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

	// Workaround for full battery status, disabled by default
	Name (BFWK, Zero)

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

	Method (_STA, 0, Serialized)
	{
		Return (BSTA (1))
	}

	Method (_BIF, 0, Serialized)
	{
		Return (BBIF (1, PBIF))
	}

	Method (_BIX, 0, Serialized)
	{
		Return (BBIX (1, PBIX))
	}

	Method (_BST, 0, Serialized)
	{
		Return (BBST (1, PBST, RefOf (BSTP), BFWK))
	}
}
#endif
