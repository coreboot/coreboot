/* Thermal Threshold Event Handler */
Method (TEVT, 1, Serialized)
{
	Switch (ToInteger (Arg0))
	{
#ifdef DPTF_TSR0_SENSOR_ID
		Case (DPTF_TSR0_SENSOR_ID) { Notify (^TSR0, 0x90) }
#endif
#ifdef DPTF_TSR1_SENSOR_ID
		Case (DPTF_TSR1_SENSOR_ID) { Notify (^TSR1, 0x90) }
#endif
#ifdef DPTF_TSR2_SENSOR_ID
		Case (DPTF_TSR2_SENSOR_ID) { Notify (^TSR2, 0x90) }
#endif
	}
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
	Name (NTTH, 5)  /* 5 degree notification threshold */
	Name (LTM0, 0)  /* Last recorded temperature */
	Name (CTYP, 0)  /* Cooling policy */

	Method (_STA)
	{
		If (LEqual (\DPTE, One)) {
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

	/* Set Cooling Policy
	 *   Arg0 - Cooling policy mode, 1=active, 0=passive
	 *   Arg1 - Acoustic Limit
	 *   Arg2 - Power Limit
	 */
	Method (_SCP, 3, Serialized)
	{
		If (LEqual (Arg0, 0)) {
			Store (0, CTYP)
		} Else {
			Store (1, CTYP)
		}

		/* DPTF Thermal Trip Points Changed Event */
		Notify (TSR0, 0x91)
	}

	/* Device Temperature Indication */
	Method (_DTI, 1)
	{
		Store (Arg0, LTM0)
		Notify (TSR0, 0x91)
	}

	/* Notification Temperature Threshold */
	Method (_NTT)
	{
		Return (^^CTOK (NTTH))
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
	Name (NTTH, 5)  /* 5 degree notification threshold */
	Name (LTM1, 0)
	Name (CTYP, 0)  /* Cooling policy */

	Method (_STA)
	{
		If (LEqual (\DPTE, One)) {
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

	/* Set Cooling Policy
	 *   Arg0 - Cooling policy mode, 1=active, 0=passive
	 *   Arg1 - Acoustic Limit
	 *   Arg2 - Power Limit
	 */
	Method (_SCP, 3, Serialized)
	{
		If (LEqual (Arg0, 0)) {
			Store (0, CTYP)
		} Else {
			Store (1, CTYP)
		}

		/* DPTF Thermal Trip Points Changed Event */
		Notify (TSR1, 0x91)
	}

	/* Device Temperature Indication */
	Method (_DTI, 1)
	{
		Store (Arg0, LTM1)
		Notify (TSR1, 0x91)
	}

	/* Notification Temperature Threshold */
	Method (_NTT)
	{
		Return (^^CTOK (NTTH))
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
	Name (NTTH, 5)  /* 5 degree notification threshold */
	Name (LTM2, 0)
	Name (CTYP, 0)  /* Cooling policy */

	Method (_STA)
	{
		If (LEqual (\DPTE, One)) {
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

	/* Set Cooling Policy
	 *   Arg0 - Cooling policy mode, 1=active, 0=passive
	 *   Arg1 - Acoustic Limit
	 *   Arg2 - Power Limit
	 */
	Method (_SCP, 3, Serialized)
	{
		If (LEqual (Arg0, 0)) {
			Store (0, CTYP)
		} Else {
			Store (1, CTYP)
		}

		/* DPTF Thermal Trip Points Changed Event */
		Notify (TSR2, 0x91)
	}

	/* Device Temperature Indication */
	Method (_DTI, 1)
	{
		Store (Arg0, LTM2)
		Notify (TSR2, 0x91)
	}

	/* Notification Temperature Threshold */
	Method (_NTT)
	{
		Return (^^CTOK (NTTH))
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
}
#endif
