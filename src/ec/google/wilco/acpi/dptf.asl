/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Dynamic Platform Thermal Framework support
 */

/* Mutex for EC PAT interface */
Mutex (PATM, 1)

/* Read requested temperature sensor */
Method (TSRD, 1, Serialized)
{
	If (Acquire (^PATM, 1000)) {
		Return (0)
	}

	/* Set sensor ID */
	W (DWTI, ToInteger (Arg0))

	Local0 = R (DRTV)

	Release (^PATM)
	Return (\_SB.DPTF.CTOK (Local0))
}

/*
 * Set Aux Trip Point 0
 *   Arg0 = Temp Sensor ID
 *   Arg1 = Value to set
 */
Method (PAT0, 2, Serialized)
{
	If (Acquire (^PATM, 1000)) {
		Return (0)
	}

	/* Set sensor ID */
	W (DWTI, ToInteger (Arg0))

	/* Set LOW trip point for this sensor */
	W (DWTL, \_SB.DPTF.KTOC (Arg1))

	Release (^PATM)
	Return (1)
}

/*
 * Set Aux Trip Point 1
 *   Arg0 = Temp Sensor ID
 *   Arg1 = Value to set
 */
Method (PAT1, 2, Serialized)
{
	If (Acquire (^PATM, 1000)) {
		Return (0)
	}

	/* Set sensor ID */
	W (DWTI, ToInteger (Arg0))

	/* Set HIGH trip point for this sensor */
	W (DWTH, \_SB.DPTF.KTOC (Arg1))

	Release (^PATM)
	Return (1)
}

/*
 * Disable Aux Trip Points
 *   Arg0 = Temp Sensor ID
 */
Method (PATD, 1, Serialized)
{
	If (Acquire (^PATM, 1000)) {
		Return (0)
	}

	/* Set sensor ID */
	W (DWTI, ToInteger (Arg0))

	/* Disable LOW and HIGH trip points */
	W (DWTL, 0xff)
	W (DWTH, 0xff)

	Release (^PATM)
	Return (1)
}

/*
 * Handle sensor trip events
 */
Method (PATX, 0, Serialized)
{
	Local0 = R (DRTQ)
	Local1 = Local0

	Printf ("Sensor trip mask: %o", Local0)

	If (!Acquire (^PATM, 1000)) {

		/* Handle bits that are set */
		While (FindSetRightBit (Local1, Local2))
		{
#ifdef HAVE_THERM_EVENT_HANDLER
			/* DPTF will Notify sensor devices */
			\_SB.DPTF.TEVT (Local2)
#endif

			/* Clear current sensor number */
			Local1 &= ~(1 << (Local2 - 1))
		}

		Release (^PATM)
	}

	/* Clear sensor events */
	W (DWTQ, Local0)
}

#ifdef EC_ENABLE_MULTIPLE_DPTF_PROFILES
/*
 * Read current Device DPTF Profile Number
 */
Method (RCDP, 0, NotSerialized)
{
	Local0 = R(DRTI)
	If (Local0 == 0) {
		Return (R(OTBL))
	} else {
		Return (Local0 - 1)
	}
}
#endif
