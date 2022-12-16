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
	PAGE = Arg0
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
	Local0 = 0
	^BPAG(Arg0 | 1)
	Local1 = BAMA
	^BPAG(Arg0) /* Battery dynamic information */

	/*
	 * Present rate is a 16bit signed int, positive while charging
	 * and negative while discharging.
	 */
	Local2 = BAPR

	If (Arg2) // Charging
	{
		Local0 |= 2
	}
	Else
	{
		If (Arg3) // Discharging
		{
			Local0 |= 1
			// Negate present rate
			Local2 = 0x10000 - Local2
		}
		Else // Full battery, force to 0
		{
			Local2 = 0
		}
	}

	/*
	 * The present rate value must be positive now, if it is not we have an
	 * EC bug or inconsistency and force the value to 0.
	 */
	If (Local2 >= 0x8000) {
		Local2 = 0
	}

	Arg1 [0] = Local0

	if (Local1) {
		Arg1 [2] = BARC * 10
		Local2 *= BAVO
		Arg1 [1] = Local2 / 1000
	} else {
		Arg1 [2] = BARC
		Arg1 [1] = Local2
	}
	Arg1 [3] = BAVO
	Release(ECLK)
	Return (Arg1)
}

Method(BINF, 2, Serialized)
{
	Acquire(ECLK, 0xffff)
	^BPAG(1 | Arg1) /* Battery 0 static information */
	Arg0 [0] = BAMA ^ 1
	Local0 = BAMA
	^BPAG(Arg1)
	Local2 = BAFC
	^BPAG(Arg1 | 2)
	Local1 = BADC

	if (Local0)
	{
		Local1 *= 10
		Local2 *= 10
	}

	Arg0 [1] = Local1	// Design Capacity
	Arg0 [2] = Local2	// Last full charge capacity
	Arg0 [4] = BADV		// Design Voltage
	Local0 = Local2 % 20	// FIXME: Local0 not used
	Arg0 [5] = Local2 / 20	// Warning capacity

	Local0 = BASN
	Name (SERN, Buffer (0x06) { "     " })
	Local1 = 4
	While (Local0)
	{
		Local2 = Local0 % 10
		Local0 /= 10
		SERN [Local1] = Local2 + 48
		Local1--
	}
	Arg0 [10] = SERN // Serial Number

	^BPAG(4 | Arg1)
	Name (TYPE, Buffer() { 0, 0, 0, 0, 0 })
	TYPE = BATY
	Arg0 [11] = TYPE // Battery type
	^BPAG(Arg1 | 5)
	Arg0 [12] = BAOE // OEM information
	^BPAG(Arg1 | 6)
	Arg0 [9] = BANA  // Model number
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
