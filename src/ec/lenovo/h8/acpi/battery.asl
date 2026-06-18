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
			BAMA, 16,		/* 16-bit read required; 8-bit access at 0xa1 returns garbage! */
#if CONFIG(H8_HAS_BAT_INFO_EXTENDED)
	Offset(0xa4),
			BACC, 16,		/* Battery Cycle Count */
#endif
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
	^BPAG(Arg0 | 1) /* Battery management */
	Local1 = BAMA
	Local1 >>= 0x0f
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

	/*
	 * Values are in mAh but we want mWh.
	 * This is required to match power unit!
	 */
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

/*
 * (Get) Battery Information
 *
 * Arg0: BATS template for legacy _BIF method
 * Arg1: BATX template for modern _BIX method
 * Arg2: Battery ID (0x1=BAT0; 0x2=BAT1)
 */
Method(BINF, 3, Serialized)
{
	BINX(Arg1, Arg2)

	Arg0 [0x00] = DerefOf(Arg1 [0x01]) // Power Unit
	Arg0 [0x01] = DerefOf(Arg1 [0x02]) // Design Capacity
	Arg0 [0x02] = DerefOf(Arg1 [0x03]) // Last Full Charge Capacity
	Arg0 [0x03] = DerefOf(Arg1 [0x04]) // Battery Technology

	// Design Voltage (keep different default value!)
	Local0 = DerefOf(Arg1 [0x05])
	If (Local0 != 0xffffffff)
	{
		Arg0 [0x04] = Local0
	}

	Arg0 [0x05] = DerefOf(Arg1 [0x06]) // Design Capacity of Warning
	Arg0 [0x06] = DerefOf(Arg1 [0x07]) // Design Capacity of Low

	// Battery Capacity Granularity 1 (keep different default value!)
	Local0 = DerefOf(Arg1 [0x0e])
	If (Local0 != 0xffffffff)
	{
		Arg0 [0x07] = Local0
	}

	// Battery Capacity Granularity 2 (keep different default value!)
	Local0 = DerefOf(Arg1 [0x0f])
	If (Local0 != 0xffffffff)
	{
		Arg0 [0x08] = Local0
	}

	Arg0 [0x09] = DerefOf(Arg1 [0x10]) // Model Number
	Arg0 [0x0a] = DerefOf(Arg1 [0x11]) // Serial Number
	Arg0 [0x0b] = DerefOf(Arg1 [0x12]) // Battery Type
	Arg0 [0x0c] = DerefOf(Arg1 [0x13]) // OEM Information

	Return (Arg0)
}

/*
 * (Get) Battery Information Extended
 *
 * Arg0: Template with default values
 * Arg1: Battery ID (0x1=BAT0; 0x2=BAT1)
 */
Method(BINX, 2, Serialized)
{
	Acquire(ECLK, 0xffff)
	^BPAG(Arg1 | 1) /* Battery management */
	Local0 = BAMA
	Local0 >>= 0x0f
#if CONFIG(H8_HAS_BAT_INFO_EXTENDED)
	Local5 = BACC
#endif
	^BPAG(Arg1) /* Battery dynamic information */
	Local2 = BAFC
	^BPAG(Arg1 | 2) /* Battery static design data */
	Local1 = BADC
	Local3 = BADV
	Local4 = 0

	If (Local0)
	{
		Local1 *= 10
		Local2 *= 10
		Local4 = 200
	}
	ElseIf (Local3)
	{
		Local4 = 200000 / Local3
	}

	Arg0 [0x01] = Local0 ^ 1	// Power Unit
	Arg0 [0x02] = Local1		// Design Capacity
	Arg0 [0x03] = Local2		// Last Full Charge Capacity
	Arg0 [0x05] = Local3		// Design Voltage
	Arg0 [0x06] = Local2 / 20	// Design Capacity of Warning
	Arg0 [0x07] = Local4		// Design Capacity of Low
#if CONFIG(H8_HAS_BAT_INFO_EXTENDED)
	Arg0 [0x08] = Local5		// Cycle Count
#endif

	// Serial Number
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
	Arg0 [0x11] = SERN

	// Battery Type
	^BPAG(Arg1 | 4)
	Name (TYPE, Buffer() { 0, 0, 0, 0, 0 })
	TYPE = BATY
	Arg0 [0x12] = TYPE

	// OEM Information
	^BPAG(Arg1 | 5)
	Arg0 [0x13] = BAOE

	// Model Number
	^BPAG(Arg1 | 6)
	Arg0 [0x10] = BANA

	Release(ECLK)
	Return (Arg0)
}

Device (BAT0)
{
	Name (_HID, EisaId ("PNP0C0A"))
	Name (_UID, 0x00)
	Name (_PCL, Package () { \_SB })

	// Template for _BIF
	Name (BATS, Package ()
	{
		0x00,			// 00: Power Unit (report in mWh)
		0xFFFFFFFF,		// 01: Design Capacity
		0xFFFFFFFF,		// 02: Last Full Charge Capacity
		0x01,			// 03: Battery Technology
		10800,			// 04: Design Voltage (mV)
		0x00,			// 05: Design Capacity of Warning
		0x00,			// 06: Design Capacity of Low
		1,			// 07: Battery Capacity Granularity 1
		1,			// 08: Battery Capacity Granularity 2
		"",			// 09: Model Number
		"",			// 0A: Serial Number
		"",			// 0B: Battery Type
		""			// 0C: OEM Information
	})

	// Template for _BIX
	Name (BATX, Package ()
	{
		0x00,			// 00: Revision
		0x00,			// 01: Power Unit (report in mWh)
		0xFFFFFFFF,		// 02: Design Capacity
		0xFFFFFFFF,		// 03: Last Full Charge Capacity
		0x01,			// 04: Battery Technology
		0xFFFFFFFF,		// 05: Design Voltage (mV)
		0x00,			// 06: Design Capacity of Warning
		0x00,			// 07: Design Capacity of Low
		0xFFFFFFFF,		// 08: Cycle Count
		0x00017318,		// 09: Measurement Accuracy
		0xFFFFFFFF,		// 0A: Max Sampling Time
		0xFFFFFFFF,		// 0B: Min Sampling Time
		0x03E8,			// 0C: Max Averaging Interval
		0x01F4,			// 0D: Min Averaging Interval
		0xFFFFFFFF,		// 0E: Battery Capacity Granularity 1
		0xFFFFFFFF,		// 0F: Battery Capacity Granularity 2
		"",			// 10: Model Number
		"",			// 11: Serial Number
		"",			// 12: Battery Type
		""			// 13: OEM Information
					// 14: Battery Swapping Capability (not available in revision 0!)
	})

	Method (_BIF, 0, NotSerialized)
	{
		Return (BINF(BATS, BATX, 0x00))
	}

#if CONFIG(H8_HAS_BAT_INFO_EXTENDED)
	Method (_BIX, 0, NotSerialized)
	{
		Return (BINX(BATX, 0x00))
	}
#endif

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

	// Template for _BIF
	Name (BATS, Package ()
	{
		0x00,			// 00: Power Unit (report in mWh)
		0xFFFFFFFF,		// 01: Design Capacity
		0xFFFFFFFF,		// 02: Last Full Charge Capacity
		0x01,			// 03: Battery Technology
		10800,			// 04: Design Voltage (mV)
		0x00,			// 05: Design Capacity of Warning
		0x00,			// 06: Design Capacity of Low
		1,			// 07: Battery Capacity Granularity 1
		1,			// 08: Battery Capacity Granularity 2
		"",			// 09: Model Number
		"",			// 0A: Serial Number
		"",			// 0B: Battery Type
		""			// 0C: OEM Information
	})

	// Template for _BIX
	Name (BATX, Package ()
	{
		0x00,			// 00: Revision
		0x00,			// 01: Power Unit (report in mWh)
		0xFFFFFFFF,		// 02: Design Capacity
		0xFFFFFFFF,		// 03: Last Full Charge Capacity
		0x01,			// 04: Battery Technology
		0xFFFFFFFF,		// 05: Design Voltage (mV)
		0x00,			// 06: Design Capacity of Warning
		0x00,			// 07: Design Capacity of Low
		0xFFFFFFFF,		// 08: Cycle Count
		0x00017318,		// 09: Measurement Accuracy
		0xFFFFFFFF,		// 0A: Max Sampling Time
		0xFFFFFFFF,		// 0B: Min Sampling Time
		0x03E8,			// 0C: Max Averaging Interval
		0x01F4,			// 0D: Min Averaging Interval
		0xFFFFFFFF,		// 0E: Battery Capacity Granularity 1
		0xFFFFFFFF,		// 0F: Battery Capacity Granularity 2
		"",			// 10: Model Number
		"",			// 11: Serial Number
		"",			// 12: Battery Type
		""			// 13: OEM Information
					// 14: Battery Swapping Capability (not available in revision 0!)
	})

	Method (_BIF, 0, NotSerialized)
	{
		Return (BINF(BATS, BATX, 0x10))
	}

#if CONFIG(H8_HAS_BAT_INFO_EXTENDED)
	Method (_BIX, 0, NotSerialized)
	{
		Return (BINX(BATX, 0x10))
	}
#endif

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
