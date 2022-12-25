/* SPDX-License-Identifier: GPL-2.0-only */

#if !CONFIG(EC_USE_LGMR)
/* TODO: Consider actually enforcing mutex? */
Mutex (BMTX, 0)
#endif
Name (B0ST, 0)	/* Battery 0 status */

/*
 * EC Registers
 *
 * "EBID" is the battery page selector.
 *
 *
 * Data on the 128 bits following offset
 * 0xE0 is accessed in the following order:
 *
 * Information:
 *  Page 0: EBCM  # start on page 0 #
 *  Page 0: EBFC
 *  Page 1: EBDC  # switch to page 1 #
 *  Page 1: EBDV
 *  Page 1: EBSN
 *  Page 3: EBDN  # switch to page 3 #
 *  Page 4: EBCH  # switch to page 4 #
 *  Page 2: EBMN  # switch to page 2 #
 *
 * Status:
 *  Page 0: EBAC  # start on page 0 #
 *  Page 0: EBRC
 *  Page 0: EBFC
 *  Page 0: EBVO
 */
/* Page 0 */
Field (RAM, ByteAcc, Lock, Preserve)
{
	Offset (0xE0),
	EBRC, 16,	/* Battery remaining capacity */
	EBFC, 16,	/* Battery full charge capacity */
	EBPE, 16,
	EBAC, 16,	/* Battery present rate */
	EBVO, 16,	/* Battery voltage */
	    , 15,
	EBCM, 1,	/* Battery charging */
	EBCU, 16,
	EBTV, 16,
}

/* Page 1 */
Field (RAM, ByteAcc, Lock, Preserve)
{
	Offset (0xE0),
	EBDC, 16,	/* Battery design capacity */
	EBDV, 16,	/* Battery design voltage */
	EBSN, 16,	/* Battery serial number */
}

/* Page 2 */
Field (RAM, ByteAcc, NoLock, Preserve)
{
	Offset (0xE0),
	EBMN, 128,	/* Battery manufacturer */
}

/* Page 3 */
Field (RAM, ByteAcc, NoLock, Preserve)
{
	Offset (0xE0),
	EBDN, 128,	/* Battery model */
}

/* Page 4 */
Field (RAM, ByteAcc, NoLock, Preserve)
{
	Offset (0xE0),
	EBCH, 128,	/* Battery type */
}

#if CONFIG(EC_USE_LGMR)
OperationRegion (MBB0, SystemMemory, (LGMR + 0x80), 0xFF)
Field (MBB0, ByteAcc, Lock, Preserve)
{
	MBRC, 16,
	MBFC, 16,
	MBPE, 16,
	MBAC, 16,
	MBVO, 16,
	    , 15,
	MBCM, 1,
	MBCU, 16,
	MBTV, 16,
}

Field (MBB0, ByteAcc, Lock, Preserve)
{
	Offset (0x10),
	MBDC, 16,
	MBDV, 16,
	MBSN, 16,
}

Field (MBB0, ByteAcc, Lock, Preserve)
{
	Offset (0x40),
	MBMN, 128,
}

Field (MBB0, ByteAcc, Lock, Preserve)
{
	Offset (0x50),
	MBDN, 256,
}

Field (MBB0, ByteAcc, Lock, Preserve)
{
	Offset (0x70),
	MBCH, 128,
}
#endif

/*
 * Arg0: Battery number
 * Arg1: Battery Information Package
 * Arg2: Status
 */
#if !CONFIG(EC_USE_LGMR)
Method (GBIF, 3, Serialized)
{
	Acquire (BMTX, 0xFFFF)	// Due to EC paging, don't run this with another function
#else
Method (GBIF, 3, NotSerialized)
{
#endif
	If (Arg2)
	{
		Arg1[1] = 0xFFFFFFFF
		Arg1[2] = 0xFFFFFFFF
		Arg1[4] = 0xFFFFFFFF
		Arg1[5] = 0
		Arg1[6] = 0
	}
	Else
	{
#if CONFIG(EC_USE_LGMR)
		Local0 = MBCM
#else
		EBID = 0	// We don't know which page was active
		Local0 = EBCM
#endif
		Arg1[0] = (Local0 ^ 1)

#if CONFIG(EC_USE_LGMR)
		Local2 = MBFC
		Local1 = MBDC
#else
		Local2 = EBFC
		EBID = 1
		Local1 = EBDC
#endif
		If (Local0)
		{
			Local2 *= 10
			Local1 *= 10
		}

		Arg1[1] = Local1	// Design capacity
		Arg1[2] = Local2	// Last full charge capacity
#if CONFIG(EC_USE_LGMR)
		Arg1[4] = MBDV		// Design voltage
#else
		Arg1[4] = EBDV		// Design voltage
#endif
		Local6 = (Local2 / 100)	// Warning capacities; Remainders ignored
		Arg1[5] = (Local6 * 7)	/* Low: 7% */
		Arg1[6] = ((Local6 * 11) / 2)	/* Very low: 5.5% */
#if CONFIG(EC_USE_LGMR)
		Local7 = MBSN
#else
		Local7 = EBSN
#endif
		Name (SERN, Buffer (0x06) { "     " })
		/*
		 * Convert hex to decimal.
		 * - There appears to be a bug in the vendor's implementation:
		 *   The correct answer has, or can have, 5 digits, so Local6 = 5.
		 *   Also see "SERN" buffer.
		 * - Userspace prints reversed serial number?
		 */
		Local6 = 4
		While (Local7)
		{
			Local5 = Local7 % 10
			Local7 = Local7 / 10
			SERN[Local6] = (Local5 + 0x30)	// Add 0x30 to get numeric ASCII
			Local6--
		}

		Arg1[10] = SERN	// Serial number
#if CONFIG(EC_USE_LGMR)
		Arg1[9] = MBDN	// Model number
		Arg1[11] = MBCH	// Battery type
		Arg1[12] = MBMN	// OEM information
#else
		EBID = 3
		Arg1[9] = EBDN	// Model number
		EBID = 4
		Arg1[11] = EBCH	// Battery type
		EBID = 2
		Arg1[12] = EBMN	// OEM information
#endif
	}

#if !CONFIG(EC_USE_LGMR)
	Release (BMTX)
#endif
	Return (Arg1)
}

/*
 * Arg0: Battery number
 * Arg1: State information
 * Arg2: Power units
 * Arg3: Battery Status Package
 */
Method (GBST, 4, NotSerialized)	// All on one page
{
#if !CONFIG(EC_USE_LGMR)
	Acquire (BMTX, 0xFFFF)	// Due to EC paging, don't run this with another function
#endif
	If (Arg1 & 0x02)	// BIT1 in "MB0S/EB0S"
	{
		Local0 = 2
		If (Arg1 & 0x20)	// "EB0F"
		{
			Local0 = 0
		}
	}
	ElseIf (Arg1 & 0x04)	// BIT2 in "MB0S/EB0S"
	{
		Local0 = 1
	}
	Else
	{
		Local0 = 0
	}

	If (Arg1 & 0x10)	// "EB0L"
	{
		Local0 |= 0x04
	}

	If (Arg1 & 0x01)	// "EB0A"
	{
		/*
		 * Present rate is a 16bit signed int, positive while charging
		 * and negative while discharging.
		 */
#if CONFIG(EC_USE_LGMR)
		Local1 = MBAC
		Local2 = MBRC
		If (MACS)	// Charging
#else
		EBID = 0	// We don't know which page was active
		Local1 = EBAC
		Local2 = EBRC
		If (EACS)	// Charging
#endif
		{
			If (Arg1 & 0x20)	// "EB0F"
			{
#if CONFIG(EC_USE_LGMR)
				Local2 = MBFC
#else
				Local2 = EBFC
#endif
			}
		}

		If (Arg2)
		{
			Local2 *= 10
		}

#if CONFIG(EC_USE_LGMR)
		Local3 = MBVO
#else
		Local3 = EBVO
#endif
		/*
		 * The present rate value should be positive unless discharging. If so,
		 * negate present rate.
		 */
		If (Local1 >= 0x8000)
		{
			If (Local0 & 0x01)
			{
				Local1 = (0x00010000 - Local1)
			}
			Else
			{
				Local1 = 0	// Full battery, force to 0
			}
		}
		/*
		 * If that was not the case, we have an EC bug or inconsistency
		 * and force the value to 0.
		 */
		ElseIf ((Local0 & 0x02) == 0)
		{
			Local1 = 0
		}

		If (Arg2)
		{
			Local1 *= Local3
			Local1 /= 1000	/* Remainder ignored by vendor */
		}
	}
	Else
	{
		Local0 = 0
		Local1 = 0xFFFFFFFF
		Local2 = 0xFFFFFFFF
		Local3 = 0xFFFFFFFF
	}

	Arg3[0] = Local0
	Arg3[1] = Local1
	Arg3[2] = Local2
	Arg3[3] = Local3

#if !CONFIG(EC_USE_LGMR)
	Release (BMTX)
#endif
	Return (Arg3)
}

Device (BAT0)
{
	Name (_HID, EisaId ("PNP0C0A") /* Control Method Battery */)  // _HID: Hardware ID
	Name (_UID, 0)  // _UID: Unique ID
	Name (_PCL, Package () { \_SB })  // _PCL: Power Consumer List

	Name (B0IP, Package (0x0D)
	{
		1,		/* 0x00: Power Unit: mAh */
		0xFFFFFFFF,	/* 0x01: Design Capacity */
		0xFFFFFFFF,	/* 0x02: Last Full Charge Capacity */
		1,		/* 0x03: Battery Technology: Rechargeable */
		0xFFFFFFFF,	/* 0x04: Design Voltage */
		0,		/* 0x05: Design Capacity of Warning */
		0,		/* 0x06: Design Capacity of Low */
		1,		/* 0x07: Capacity Granularity 1 */
		1,		/* 0x08: Capacity Granularity 2 */
		"",		/* 0x09: Model Number */
		"100",		/* 0x0a: Serial Number */
		"Lion",		/* 0x0b: Battery Type */
		0		/* 0x0c: OEM Information */
	})
	Name (B0SP, Package (0x04)
	{
		0,		/* 0x00: Battery State */
		0xFFFFFFFF,	/* 0x01: Battery Present Rate */
		0xFFFFFFFF,	/* 0x02: Battery Remaining Capacity */
		0xFFFFFFFF	/* 0x03: Battery Present Voltage */
	})
	Method (_STA, 0, NotSerialized)  // _STA: Status
	{
		/* Bitwise AND by vendor is lossy? */
		Local1 = EB0A
		If (Local1 & 0x40)
		{
			Local1 = 0
		}

		B0ST = Local1
		If (Local1)
		{
			Return (0x1F)
		}
		Else
		{
			Return (0x0F)
		}
	}

	Method (_BIF, 0, NotSerialized)  // _BIF: Battery Information
	{
		Local6 = B0ST
		Local7 = 20
		While (Local6 && Local7)
		{
			If (EB0R)
			{
				Local6 = 0
			}
			Else
			{
				Sleep (500)
				Local7--
			}
		}

		Return (GBIF (0, B0IP, Local6))
	}

	Method (_BST, 0, NotSerialized)  // _BST: Battery Status
	{
		Local0 = (DerefOf (B0IP[0]) ^ 1)
#if CONFIG(EC_USE_LGMR)
		Local5 = MB0S
#else
		Local5 = EB0S
#endif
		Return (GBST (0, Local5, Local0, B0SP))
	}
}
