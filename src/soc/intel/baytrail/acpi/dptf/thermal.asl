/* SPDX-License-Identifier: GPL-2.0-only */

/* Thermal Threshold Event Handler */
#define HAVE_THERM_EVENT_HANDLER

#if CONFIG(EC_SUPPORTS_DPTF_TEVT)
Method (TEVT, 1, NotSerialized)
{
	Store (ToInteger (Arg0), Local0)

#ifdef DPTF_TSR0_SENSOR_ID
	If (Local0 == DPTF_TSR0_SENSOR_ID) {
		Notify (^TSR0, 0x90)
	}
#endif
#ifdef DPTF_TSR1_SENSOR_ID
	If (Local0 == DPTF_TSR1_SENSOR_ID) {
		Notify (^TSR1, 0x90)
	}
#endif
#ifdef DPTF_TSR2_SENSOR_ID
	If (Local0 == DPTF_TSR2_SENSOR_ID) {
		Notify (^TSR2, 0x90)
	}
#endif
}
#endif

/* Thermal device initialization - Disable Aux Trip Points */
Method (TINI)
{
#ifdef DPTF_TSR0_SENSOR_ID
	^TSR0.PATD ()
#endif
#ifdef DPTF_TSR1_SENSOR_ID
	^TSR1.PATD ()
#endif
#ifdef DPTF_TSR2_SENSOR_ID
	^TSR2.PATD ()
#endif
}

#ifdef DPTF_TSR0_SENSOR_ID
Device (TSR0)
{
	Name (_HID, EISAID ("INT3403"))
	Name (_UID, 1)
	Name (PTYP, 0x03)
	Name (TMPI, DPTF_TSR0_SENSOR_ID)
	Name (_STR, Unicode (DPTF_TSR0_SENSOR_NAME))
	Name (GTSH, 20) /* 2 degree hysteresis */

	Method (_STA)
	{
		If (\DPTE == One) {
			Return (0xF)
		} Else {
			Return (0x0)
		}
	}

	Method (_TMP, 0, Serialized)
	{
		Return (\_SB.PCI0.LPCB.EC0.TSRD (TMPI))
	}

	Method (_PSV)
	{
		Return (^^CTOK (DPTF_TSR0_PASSIVE))
	}

	Method (_CRT)
	{
		Return (^^CTOK (DPTF_TSR0_CRITICAL))
	}

	Name (PATC, 2)

	/* Set Aux Trip Point */
	Method (PAT0, 1, Serialized)
	{
		\_SB.PCI0.LPCB.EC0.PAT0 (TMPI, Arg0)
	}

	/* Set Aux Trip Point */
	Method (PAT1, 1, Serialized)
	{
		\_SB.PCI0.LPCB.EC0.PAT1 (TMPI, Arg0)
	}

	/* Disable Aux Trip Point */
	Method (PATD, 0, Serialized)
	{
		\_SB.PCI0.LPCB.EC0.PATD (TMPI)
	}
}
#endif

#ifdef DPTF_TSR1_SENSOR_ID
Device (TSR1)
{
	Name (_HID, EISAID ("INT3403"))
	Name (_UID, 2)
	Name (PTYP, 0x03)
	Name (TMPI, DPTF_TSR1_SENSOR_ID)
	Name (_STR, Unicode (DPTF_TSR1_SENSOR_NAME))
	Name (GTSH, 20) /* 2 degree hysteresis */

	Method (_STA)
	{
		If (\DPTE == One) {
			Return (0xF)
		} Else {
			Return (0x0)
		}
	}

	Method (_TMP, 0, Serialized)
	{
		Return (\_SB.PCI0.LPCB.EC0.TSRD (TMPI))
	}

	Method (_PSV)
	{
		Return (^^CTOK (DPTF_TSR1_PASSIVE))
	}

	Method (_CRT)
	{
		Return (^^CTOK (DPTF_TSR1_CRITICAL))
	}

	Name (PATC, 2)

	/* Set Aux Trip Point */
	Method (PAT0, 1, Serialized)
	{
		\_SB.PCI0.LPCB.EC0.PAT0 (TMPI, Arg0)
	}

	/* Set Aux Trip Point */
	Method (PAT1, 1, Serialized)
	{
		\_SB.PCI0.LPCB.EC0.PAT1 (TMPI, Arg0)
	}

	/* Disable Aux Trip Point */
	Method (PATD, 0, Serialized)
	{
		\_SB.PCI0.LPCB.EC0.PATD (TMPI)
	}
}
#endif

#ifdef DPTF_TSR2_SENSOR_ID
Device (TSR2)
{
	Name (_HID, EISAID ("INT3403"))
	Name (_UID, 3)
	Name (PTYP, 0x03)
	Name (TMPI, DPTF_TSR2_SENSOR_ID)
	Name (_STR, Unicode (DPTF_TSR2_SENSOR_NAME))
	Name (GTSH, 20) /* 2 degree hysteresis */

	Method (_STA)
	{
		If (\DPTE == One) {
			Return (0xF)
		} Else {
			Return (0x0)
		}
	}

	Method (_TMP, 0, Serialized)
	{
		Return (\_SB.PCI0.LPCB.EC0.TSRD (TMPI))
	}

	Method (_PSV)
	{
		Return (^^CTOK (DPTF_TSR2_PASSIVE))
	}

	Method (_CRT)
	{
		Return (^^CTOK (DPTF_TSR2_CRITICAL))
	}

	Name (PATC, 2)

	/* Set Aux Trip Point */
	Method (PAT0, 1, Serialized)
	{
		\_SB.PCI0.LPCB.EC0.PAT0 (TMPI, Arg0)
	}

	/* Set Aux Trip Point */
	Method (PAT1, 1, Serialized)
	{
		\_SB.PCI0.LPCB.EC0.PAT1 (TMPI, Arg0)
	}

	/* Disable Aux Trip Point */
	Method (PATD, 0, Serialized)
	{
		\_SB.PCI0.LPCB.EC0.PATD (TMPI)
	}
}
#endif
