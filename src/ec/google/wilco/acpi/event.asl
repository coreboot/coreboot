/* SPDX-License-Identifier: GPL-2.0-only */

/* ACPI_POWER_RECORD */
Name (ECPR, 0)

Method (ECQP, 0, Serialized)
{
	Local0 = R (PWSR)
	Local1 = Local0 ^ ECPR
	ECPR = Local0

	If (EBIT (ACEX, Local1)) {
		Printf ("AC Power Status Changed")
		Notify (AC, 0x80)
	}

	If (EBIT (BTEX, Local1)) {
		If (EBIT (BTEX, Local0)) {
			Printf ("BAT0 Inserted")
			Notify (BAT0, 0x81)
		} Else {
			Printf ("BAT0 Removed")
			Notify (BAT0, 0x80)
			Notify (BAT0, 0x81)
		}
	}

	/*
	 * Battery status is cleared when read so always use the value from
	 * PWSR directly regardless of the previous value stored in ECPR.
	 */
	If (EBIT (BTSC, Local0)) {
		Printf ("BAT0 Status Change")
		Notify (BAT0, 0x80)
	}
}

/* Handle events in PmEv1 */
Method (ECQ1, 1, Serialized)
{
	/* Power button pressed */
	If (EBIT (E1PB, Arg0)) {
		Printf ("Power Button Event")
		/* Do not notify \_SB.PWRB here to prevent double event */
	}

	/* LID state changed */
	If (EBIT (E1LD, Arg0)) {
		Printf ("Lid State Changed")
		Notify (^LID0, 0x80)
	}

	/* Power Event */
	If (EBIT (E1PW, Arg0)) {
		Printf ("Power Event")
		ECQP ()
	}

	/* Sleep Button */
	If (EBIT (E1SB, Arg0)) {
		Printf ("Sleep Button")
	}
}

/* Handle events in PmEv2 */
Method (ECQ2, 1, Serialized)
{
	Printf ("EVT2: %o", Arg0)

	If (EBIT (E2QS, Arg0)) {
		Printf ("QS EVENT")
		Notify (^WEVT, 0x90)
	}

	If (EBIT (E2OR, Arg0)) {
		If (R (OTBL)) {
			Printf ("EC event indicates tablet mode")
			Notify (^VBTN, ^VTBL)
		} Else {
			Printf ("EC event indicates laptop mode")
			Notify (^VBTN, ^VLAP)
		}
	}
}

/* Handle events in PmEv3 */
Method (ECQ3, 1, Serialized)
{
	Printf ("EVT3: %o", Arg0)

	If (EBIT (E3CP, Arg0)) {
		If (R (P2PB)) {
			Printf ("Power button pressed")
			Notify (^VBTN, ^VPPB)
		} Else {
			Printf ("Power button released")
			Notify (^VBTN, ^VRPB)
		}
	}

#ifdef EC_ENABLE_DPTF
	/* Theraml Events */
	If (EBIT (E3TH, Arg0)) {
		^PATX ()
	}
#endif
}

/* Handle events in PmEv4 */
Method (ECQ4, 1, Serialized)
{
	Printf ("EVT4: %o", Arg0)
}

/* Process all events */
Method (_Q66, 0, Serialized)
{
	Local0 = R (EVT1)
	If (Local0) {
		ECQ1 (Local0)
	}

	Local0 = R (EVT2)
	If (Local0) {
		ECQ2 (Local0)
	}

	Local0 = R (EVT3)
	If (Local0) {
		ECQ3 (Local0)
	}

	Local0 = R (EVT4)
	If (Local0) {
		ECQ4 (Local0)
	}
}

/* UCSI SCI uses a unique event code */
Method (_Q79, 0, Serialized)
{
	If (ISSX == Zero) {
		Printf ("EC _Q79 UCSI Event")
		Notify (^UCSI, 0x80)
		^UCEP = Zero
	} Else {
		Printf ("EC _Q79 UCSI Event Masked in S0ix")
		^UCEP = 1
	}
}
