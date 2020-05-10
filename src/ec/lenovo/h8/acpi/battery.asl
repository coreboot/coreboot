/* SPDX-License-Identifier: GPL-2.0-only */

Field(ERAM, ByteAcc, NoLock, Preserve)
{
	Offset (0x38),
			B0ST, 4,	/* Battery 0 state */
			    , 1,
			B0CH, 1,	/* Battery 0 charging */
			B0DI, 1,	/* Battery 0 discharging */
			B0PR, 1,	/* Battery 0 present */
	Offset (0x39),
			B1ST, 4,	/* Battery 1 state */
			    , 1,
			B1CH, 1,	/* Battery 1 charging, */
			B1DI, 1,        /* Battery 1 discharging,*/
			B1PR, 1		/* Battery 1 present */
}

/* EC Registers */
/* PAGE == 0x00 */
Field (ERAM, ByteAcc, NoLock, Preserve)
{
	Offset(0xa0),
			BARC, 16,		/* Battery remaining capacity */
			BAFC, 16,		/* Battery full charge capacity */
	Offset(0xa8),
			BAPR, 16,		/* Battery present rate */
			BAVO, 16,		/* Battery Voltage */
}

/* PAGE == 0x01 */
Field (ERAM, ByteAcc, NoLock, Preserve)
{
	Offset(0xa0),
			    , 15,
			BAMA,  1,
}

/* PAGE == 0x02 */
Field (ERAM, ByteAcc, NoLock, Preserve)
{
	Offset(0xa0),
			BADC, 16,		/* Design Capacity */
			BADV, 16,		/* Design voltage */
			    , 16,
			    , 16,
			    , 16,
			BASN, 16,
}

/* PAGE == 0x04: Battery type */
Field (ERAM, ByteAcc, NoLock, Preserve)
{
	Offset(0xa0),
			BATY, 32
}


/* PAGE == 0x05: Battery OEM information */
Field (ERAM, ByteAcc, NoLock, Preserve)
{
	Offset(0xa0),
			BAOE, 128
}

/* PAGE == 0x06: Battery name */
Field (ERAM, ByteAcc, NoLock, Preserve)
{
	Offset(0xa0),
			BANA, 128
}

/*
 * Switches the battery information page (16 bytes ERAM @0xa0) with an
 * optional compile-time delay.
 *
 * Arg0:
 *   bit7-4: Battery number
 *   bit3-0: Information page number
 */
Method(BPAG, 1, NotSerialized)
{
	Store(Arg0, PAGE)
#ifdef BATTERY_PAGE_DELAY_MS
	Sleep(BATTERY_PAGE_DELAY_MS)
#endif
}

/* Arg0: Battery
 * Arg1: Battery Status Package
 * Arg2: charging
 * Arg3: discharging
 */
Method(BSTA, 4, NotSerialized)
{
	Acquire(ECLK, 0xffff)
	Store(0, Local0)
	^BPAG(Or(1, Arg0))
	Store(BAMA, Local1)
	^BPAG(Arg0) /* Battery dynamic information */

	/*
	 * Present rate is a 16bit signed int, positive while charging
	 * and negative while discharging.
	 */
	Store(BAPR, Local2)

	If (Arg2) // Charging
	{
		Or(2, Local0, Local0)
	}
	Else
	{
		If (Arg3) // Discharging
		{
			Or(1, Local0, Local0)
			// Negate present rate
			Subtract(0x10000, Local2, Local2)
		}
		Else // Full battery, force to 0
		{
			Store(0, Local2)
		}
	}

	/*
	 * The present rate value must be positive now, if it is not we have an
	 * EC bug or inconsistency and force the value to 0.
	 */
	If (LGreaterEqual (Local2, 0x8000)) {
		Store(0, Local2)
	}

	Store(Local0, Index(Arg1, 0x00))

	if (Local1) {
		Multiply (BARC, 10, Index(Arg1, 2))
		Multiply (Local2, BAVO, Local2)
		Divide (Local2, 1000, , Index(Arg1, 1))
	} else {
		Store(BARC, Index(Arg1, 2))
		Store(Local2, Index(Arg1, 1))
	}
	Store(BAVO, Index(Arg1, 3))
	Release(ECLK)
	Return (Arg1)
}

Method(BINF, 2, Serialized)
{
	Acquire(ECLK, 0xffff)
	^BPAG(Or(1, Arg1)) /* Battery 0 static information */
	Xor(BAMA, 1, Index(Arg0, 0))
	Store(BAMA, Local0)
	^BPAG(Arg1)
	Store(BAFC, Local2)
	^BPAG(Or(2, Arg1))
	Store(BADC, Local1)

	if (Local0)
	{
		Multiply (Local1, 10, Local1)
		Multiply (Local2, 10, Local2)
	}

	Store(Local1, Index(Arg0, 1))	// Design Capacity
	Store(Local2, Index(Arg0, 2))	// Last full charge capacity
	Store(BADV, Index(Arg0, 4))	// Design Voltage
	Divide (Local2, 20, Local0, Index(Arg0, 5)) // Warning capacity

	Store (BASN, Local0)
	Name (SERN, Buffer (0x06) { "     " })
	Store (4, Local1)
	While (Local0)
	{
		Divide (Local0, 0x0A, Local2, Local0)
		Add (Local2, 48, Index (SERN, Local1))
		Decrement (Local1)
	}
	Store (SERN, Index (Arg0, 10)) // Serial Number

	^BPAG(Or(4, Arg1))
	Name (TYPE, Buffer() { 0, 0, 0, 0, 0 })
	Store(BATY, TYPE)
	Store(TYPE, Index (Arg0, 11)) // Battery type
	^BPAG(Or(5, Arg1))
	Store(BAOE, Index (Arg0, 12)) // OEM information
	^BPAG(Or(6, Arg1))
	Store(BANA, Index (Arg0, 9))  // Model number
	Release(ECLK)
	Return (Arg0)
}

Device (BAT0)
{
	Name (_HID, EisaId ("PNP0C0A"))
	Name (_UID, 0x00)
	Name (_PCL, Package () { \_SB })

	Name (BATS, Package ()
	{
		0x00,			// 0: PowerUnit: Report in mWh
		0xFFFFFFFF,		// 1: Design cap
		0xFFFFFFFF,		// 2: Last full charge cap
		0x01,			// 3: Battery Technology
		10800,			// 4: Design Voltage (mV)
		0x00,			// 5: Warning design capacity
		200,			// 6: Low design capacity
		1,			// 7: granularity1
		1,			// 8: granularity2
		"",			// 9: Model number
		"",			// A: Serial number
		"",			// B: Battery Type
		""			// C: OEM information
	})

	Method (_BIF, 0, NotSerialized)
	{
		Return (BINF(BATS, 0))
	}

	Name (BATI, Package ()
	{
		0,			// Battery State
					// Bit 0 - discharge
					// Bit 1 - charge
					// Bit 2 - critical state
		0,			// Battery present Rate
		0,			// Battery remaining capacity
		0			// Battery present voltage
	})

	Method (_BST, 0, NotSerialized)
	{
		if (B0PR) {
			Return (BSTA(0, BATI, B0CH, B0DI))
		} else {
			Return (Package () { 0, 0, 0, 0 })
		}
	}

	Method (_STA, 0, NotSerialized)
	{
		if (B0PR) {
			Return (0x1f)
		} else {
			Return (0x0f)
		}
	}
}

Device (BAT1)
{
	Name (_HID, EisaId ("PNP0C0A"))
	Name (_UID, 0x00)
	Name (_PCL, Package () { \_SB })

	Name (BATS, Package ()
	{
		0x00,			// 0: PowerUnit: Report in mWh
		0xFFFFFFFF,		// 1: Design cap
		0xFFFFFFFF,		// 2: Last full charge cap
		0x01,			// 3: Battery Technology
		10800,			// 4: Design Voltage (mV)
		0x00,			// 5: Warning design capacity
		200,			// 6: Low design capacity
		1,			// 7: granularity1
		1,			// 8: granularity2
		"",			// 9: Model number
		"",			// A: Serial number
		"",			// B: Battery Type
		""			// C: OEM information
	})

	Method (_BIF, 0, NotSerialized)
	{
		Return (BINF(BATS, 0x10))
	}

	Name (BATI, Package ()
	{
		0,			// Battery State
					// Bit 0 - discharge
					// Bit 1 - charge
					// Bit 2 - critical state
		0,			// Battery present Rate
		0,			// Battery remaining capacity
		0			// Battery present voltage
	})

	Method (_BST, 0, NotSerialized)
	{
		if (B1PR) {
			Return (BSTA(0x10, BATI, B1CH, B1DI))
		} else {
			Return (Package () { 0, 0, 0, 0 })
		}
	}

	Method (_STA, 0, NotSerialized)
	{
		if (B1PR) {
			Return (0x1f)
		} else {
			Return (0x0f)
		}
	}
}

/* Battery 0 critical */
Method(_Q24, 0, NotSerialized)
{
	Notify(BAT0, 0x80)
}

/* Battery 1 critical */
Method(_Q25, 0, NotSerialized)
{
	Notify(BAT1, 0x80)
}

/* Battery 0 attach/detach */
Method(_Q4A, 0, NotSerialized)
{
	Notify(BAT0, 0x81)
}

/* Battery 0 state change */
Method(_Q4B, 0, NotSerialized)
{
	Notify(BAT0, 0x80)
}

/* Battery 1 attach/detach */
Method(_Q4C, 0, NotSerialized)
{
	Notify(BAT1, 0x81)
}

/* Battery 1 state change */
Method(_Q4D, 0, NotSerialized)
{
	Notify(BAT1, 0x80)
}
