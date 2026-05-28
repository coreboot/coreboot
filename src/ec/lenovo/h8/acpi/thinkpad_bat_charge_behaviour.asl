/* SPDX-License-Identifier: GPL-2.0-only */

/* Charge behaviour control */
#define DISABLE_INHIBITCHARGE    0x1
#define ENABLE_INHIBITCHARGE     0x2
#define DISABLE_FORCEDISCHARGE   0x3
#define ENABLE_FORCEDISCHARGE    0x4

/* Return values for BDSG method */
#define FORCEDISCHARGE_SUPPORTED 0x100
#define FORCEDISCHARGE_ACTIVE    0x001

/* Return values for BICG method */
#define INHIBITCHARGE_SUPPORTED  0x20
#define INHIBITCHARGE_ACTIVE     0x01

/* EC Registers */
Field (ERAM, ByteAcc, NoLock, Preserve)
{
	Offset (0x0f),
			B0IC,  1,	/* BAT0 inhibit-charge active */
			B1IC,  1,	/* BAT1 inhibit-charge active */
			B0FD,  1,	/* BAT0 force-discharge active */
			B1FD,  1,	/* BAT1 force-discharge active */
	Offset (0x21),
			BAET, 16,	/* Effective time (for inhibit-charge) */
	Offset (0xb4),
			B0CB,  8,	/* BAT0 charge behaviour control */
	Offset (0xb5),
			B1CB,  8,	/* BAT1 charge behaviour control */
}

/*
 * Set state for force-discharge:
 *  Bit 8-9: Battery ID (0x1=BAT0; 0x2=BAT1)
 *  Bit 1: Break on AC attach
 *  Bit 0: Enable or disable
 */
Method (BDSS, 1, NotSerialized)
{
	Local0 = Arg0 & 0x1        // New state
	Local1 = (Arg0 >> 8) & 0x3 // Battery ID
	Local2 = (Arg0 >> 1) & 0x1 // Break on AC attach
	Local3 = 0x0               // EC RAM value

	If (Local0)
	{
		/*
		 * Silently ignore it if AC is not attached.
		 * This should match stock BIOS behaviour.
		 */
		If (!HPAC)
		{
			Return (0x0)
		}

		Local3 = ENABLE_FORCEDISCHARGE
	}
	Else
	{
		Local3 = DISABLE_FORCEDISCHARGE
	}

	/*
	 * "Break on AC attach" is not supported by modern ECs.
	 * It's not used by thinkpad_acpi, return error if requested.
	 * Intentionally unsupported to match stock BIOS behaviour.
	 */
	If (!Local2)
	{
		/* BAT0 */
		If (Local1 == 1)
		{
			B0CB = Local3
			Return (0x0)
		}

		/* BAT1 */
		If (Local1 == 2)
		{
			B1CB = Local3
			Return (0x0)
		}
	}

	Return (1 << 31)
}

/*
 * Argument: 0x1 for BAT0; 0x2 for BAT1
 * Returns the current state for force-discharge:
 *  Bit 8: Feature supported
 *  Bit 0: Currently active
 */
Method (BDSG, 1, NotSerialized)
{
	Local0 = 0x0
	Local1 = 0x0
	Local2 = 0x0

	/* BAT0 */
	If (Arg0 == 1)
	{
		Local0 = B0PR
		Local1 = B0FD
	}
	/* BAT1 */
	ElseIf (Arg0 == 2)
	{
		Local0 = B1PR
		Local1 = B1FD
	}
	Else
	{
		Return (1 << 31)
	}

	If (Local0)
	{
		Local2 |= FORCEDISCHARGE_SUPPORTED

		If (Local1)
		{
			Local2 |= FORCEDISCHARGE_ACTIVE
		}
	}

	Return (Local2)
}

/*
 * Set state for inhibit-charge:
 *  Bit 8-23: Effective time in minutes (0xffff indicates forever)
 *  Bit 4-5: Battery ID (0x1=BAT0; 0x2=BAT1)
 *  Bit 0: Enable or disable
 */
Method (BICS, 1, NotSerialized)
{
	Local0 = Arg0 & 0x1           // New state
	Local1 = (Arg0 >> 4) & 0x3    // Battery ID
	Local2 = (Arg0 >> 8) & 0xffff // Effective time
	Local3 = 0x0                  // EC RAM value
	Local4 = 0x0

	If (Local2 != 0xffff)
	{
		Local4 = Local2 >> 8
	}

	/* Only allow effective time <256 minutes (1-byte) or 0xffff. */
	If (Local4 == 0x0)
	{
		If (Local0)
		{
			Local3 = ENABLE_INHIBITCHARGE
		}
		Else
		{
			Local3 = DISABLE_INHIBITCHARGE
		}

		/* BAT0 */
		If (Local1 == 1)
		{
			BAET = Local2
			B0CB = Local3
			Return (0x0)
		}

		/* BAT1 */
		If (Local1 == 2)
		{
			BAET = Local2
			B1CB = Local3
			Return (0x0)
		}
	}

	Return (1 << 31)
}

/*
 * Argument: 0x1 for BAT0; 0x2 for BAT1
 * Returns the current state for inhibit-charge:
 *  Bit 5: Feature supported
 *  Bit 0: Currently active
 */
Method (BICG, 1, NotSerialized)
{
	Local0 = 0x0
	Local1 = 0x0
	Local2 = 0x0

	/* BAT0 */
	If (Arg0 == 1)
	{
		Local0 = B0PR
		Local1 = B0IC
	}
	/* BAT1 */
	ElseIf (Arg0 == 2)
	{
		Local0 = B1PR
		Local1 = B1IC
	}
	Else
	{
		Return (1 << 31)
	}

	If (Local0)
	{
		Local2 |= INHIBITCHARGE_SUPPORTED

		If (Local1)
		{
			Local2 |= INHIBITCHARGE_ACTIVE
		}
	}

	Return (Local2)
}

/*
 * RBCB - ResetBatteryChargeBehaviour, implemented for _Q27 method in ec.asl.
 * Argument reserved for future extension: AC attached (1) or detached (0)
 */
Method (RBCB, 1, NotSerialized)
{
	/* AC detach */
	If (!Arg0)
	{
		/* BAT0: Disable force-discharge */
		BDSS(0x100)

		/* BAT1: Disable force-discharge */
		BDSS(0x200)
	}
}
