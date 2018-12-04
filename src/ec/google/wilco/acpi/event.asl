/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* ACPI_POWER_RECORD */
Name (ECPR, 0)

Method (ECQP, 0, Serialized)
{
	Local0 = R (APWR)
	Local1 = Local0 ^ ECPR
	ECPR = Local0

	If (EBIT (APAC, Local1)) {
		Printf ("AC Power Status Changed")
		Notify (AC, 0x80)
	}

	If (EBIT (APB1, Local1)) {
		If (EBIT (APB1, Local0)) {
			Printf ("BAT0 Inserted")
			Notify (BAT0, 0x81)
		} Else {
			Printf ("BAT0 Removed")
			Notify (BAT0, 0x80)
			Notify (BAT0, 0x81)
		}
	}

	If (EBIT (APC1, Local1)) {
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
		Notify (^LID, 0x80)
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
		Notify (^WLCO, 0x90)
	}
}

/* Handle events in PmEv3 */
Method (ECQ3, 1, Serialized)
{
	Printf ("EVT3: %o", Arg0)

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

/* Get Event Buffer */
Method (QSET, 0, Serialized)
{
	/* Get count of event bytes */
	Local0 = R (QSEC)
	Name (QBUF, Buffer (Local0) {})

	/* Fill QS event buffer with Local0 bytes */
	For (Local1 = 0, Local1 < Local0, Local1++) {
		QBUF[Local1] = R (QSEB)
	}

	Printf ("QS = %o", QBUF)
	Return (QBUF)
}
