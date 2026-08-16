/* SPDX-License-Identifier: GPL-2.0-only */

#define ICH10_COMPATIBLE (CONFIG_SOUTHBRIDGE_INTEL_COMMON_NUM_GPIOS >= 64)

#if ICH10_COMPATIBLE

/* 3 GPIO banks */
OperationRegion (GPLV, SystemIO, DEFAULT_GPIOBASE, 0x6c)
Field (GPLV, AnyAcc, NoLock, Preserve)
{
	Offset(0x0c),	// GPIO Level
	LVL0, 32,
	Offset(0x38),	// GPIO Level2
	LVL1, 32,
	Offset(0x48),	// GPIO Level3
	LVL2, 12,
}

#else

/* 2 GPIO banks */
OperationRegion (GPLV, SystemIO, DEFAULT_GPIOBASE, 0x40)
Field (GPLV, AnyAcc, NoLock, Preserve)
{
	Offset(0x0c),	// GPIO Level
	LVL0, 32,
	Offset(0x38),	// GPIO Level2
	LVL1, 32,
}

#endif

/*
 * Get GPIO level
 * Arg0 - GPIO Number
 * Returns: Level
 */
Method (GLVL, 0x1)
{
	/* Ignore invalid GPIO offsets */
	If (Arg0 > CONFIG_SOUTHBRIDGE_INTEL_COMMON_NUM_GPIOS)
	{
		Debug = "Invalid GPIO offset"
		Return (0)
	}

	Local0 = Arg0 >> 5
	If (Local0 == 0)
	{
		Local1 = LVL0
	}
	ElseIf (Local0 == 1)
	{
		Local1 = LVL1
	}
#if ICH10_COMPATIBLE
	ElseIf (Local0 == 2)
	{
		Local1 = LVL2
	}
#endif
	Else
	{
		Return (0)
	}

	Local0 = Local1 >> (Arg0 & 0x1f)
	Return (Local0 & 1)
}

/*
 * Set GPIO level
 * Arg0 - GPIO Number
 * Arg0 - Level
 */
Method (SLVL, 0x2)
{
	/* Ignore invalid GPIO offsets */
	If (Arg0 > CONFIG_SOUTHBRIDGE_INTEL_COMMON_NUM_GPIOS)
	{
		Debug = "Invalid GPIO offset"
		Return ()
	}

	Local0 = Arg0 >> 5
	If (Local0 == 0)
	{
		Local1 = LVL0
	}
	ElseIf (Local0 == 1)
	{
		Local1 = LVL1
	}
#if ICH10_COMPATIBLE
	ElseIf (Local0 == 2)
	{
		Local1 = LVL2
	}
#endif
	Else
	{
		Return ()
	}

	If (Arg1 == 0)
	{
		Local1 &= ~(1 << (Arg0 & 0x1f))
	}
	Else
	{
		Local1 |= 1 << (Arg0 & 0x1f)
	}

	If (Local0 == 0)
	{
		LVL0 = Local1
	}
	ElseIf (Local0 == 1)
	{
		LVL1 = Local1
	}
#if ICH10_COMPATIBLE
	ElseIf (Local0 == 2)
	{
		LVL2 = Local1
	}
#endif
}

/*
 * Set GPIO Output Value
 * Arg0 - GPIO Number
 */
Method (STXS, 1, Serialized)
{
	SLVL(Arg0, 1)
}

/*
 * Clear GPIO Output Value
 * Arg0 - GPIO Number
 */
Method (CTXS, 1, Serialized)
{
	SLVL(Arg0, 0)
}

/*
 * Get GPIO Input Value
 * Arg0 - GPIO Number
 */
Method (GRXS, 1, Serialized)
{
	Return (GLVL(Arg0))
}

/*
 * Get GPIO Output Value
 * Arg0 - GPIO Number
 */
Method (GTXS, 1, Serialized)
{
	Return (GLVL(Arg0))
}
