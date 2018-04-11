/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* Thermal Threshold Event Handler */
Method (TEVT, 1, NotSerialized)
{

#ifdef DPTF_TSR0_SENSOR_ID
	If (LEqual (ToInteger (Arg0), DPTF_TSR0_SENSOR_ID)) {
		Notify (^TSR0, 0x90)
	}
#endif
#ifdef DPTF_TSR1_SENSOR_ID
	If (LEqual (ToInteger (Arg0), DPTF_TSR1_SENSOR_ID)) {
		Notify (^TSR1, 0x90)
	}
#endif
#ifdef DPTF_TSR2_SENSOR_ID
	If (LEqual (ToInteger (Arg0), DPTF_TSR2_SENSOR_ID)) {
		Notify (^TSR2, 0x90)
	}
#endif
#ifdef DPTF_TSR3_SENSOR_ID
	If (LEqual (ToInteger (Arg0), DPTF_TSR3_SENSOR_ID)) {
		Notify (^TSR3, 0x90)
	}
#endif
}

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
#ifdef DPTF_TSR3_SENSOR_ID
	^TSR3.PATD ()
#endif
}

/* Thermal Trip Points Change Event Handler */
Method (TPET)
{
#ifdef DPTF_TSR0_SENSOR_ID
	Notify (^TSR0, 0x81)
#endif
#ifdef DPTF_TSR1_SENSOR_ID
	Notify (^TSR1, 0x81)
#endif
#ifdef DPTF_TSR2_SENSOR_ID
	Notify (^TSR2, 0x81)
#endif
#ifdef DPTF_TSR3_SENSOR_ID
	Notify (^TSR3, 0x81)
#endif
}

/*
 * Method to return trip temperature value depending upon the device mode.
 * Arg0 --> Value to return when device is in tablet mode
 * Arg1 --> Value to return when device is not in tablet mode.
 */
Method (DTRP, 2, Serialized)
{
#ifdef EC_ENABLE_TABLET_EVENT
	If (LEqual (\_SB.PCI0.LPCB.EC0.RCTM, One)) {
		Return (CTOK (Arg0))
	} Else {
#endif
		Return (CTOK (Arg1))
#ifdef EC_ENABLE_TABLET_EVENT
	}
#endif
}

#ifdef DPTF_TSR0_SENSOR_ID

#ifndef DPTF_TSR0_TABLET_PASSIVE
#define DPTF_TSR0_TABLET_PASSIVE DPTF_TSR0_PASSIVE
#endif
#ifndef DPTF_TSR0_TABLET_CRITICAL
#define DPTF_TSR0_TABLET_CRITICAL DPTF_TSR0_CRITICAL
#endif

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
		Return (DTRP (DPTF_TSR0_TABLET_PASSIVE, DPTF_TSR0_PASSIVE))
	}

	Method (_CRT)
	{
		Return (DTRP (DPTF_TSR0_TABLET_CRITICAL, DPTF_TSR0_CRITICAL))
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

#ifdef DPTF_ENABLE_FAN_CONTROL
#ifdef DPTF_TSR0_ACTIVE_AC0
	Method (_AC0)
	{
		Return (\_SB.DPTF.CTOK (DPTF_TSR0_ACTIVE_AC0))
	}
#endif
#ifdef DPTF_TSR0_ACTIVE_AC1
	Method (_AC1)
	{
		Return (\_SB.DPTF.CTOK (DPTF_TSR0_ACTIVE_AC1))
	}
#endif
#ifdef DPTF_TSR0_ACTIVE_AC2
	Method (_AC2)
	{
		Return (\_SB.DPTF.CTOK (DPTF_TSR0_ACTIVE_AC2))
	}
#endif
#ifdef DPTF_TSR0_ACTIVE_AC3
	Method (_AC3)
	{
		Return (\_SB.DPTF.CTOK (DPTF_TSR0_ACTIVE_AC3))
	}
#endif
#ifdef DPTF_TSR0_ACTIVE_AC4
	Method (_AC4)
	{
		Return (\_SB.DPTF.CTOK (DPTF_TSR0_ACTIVE_AC4))
	}
#endif
#ifdef DPTF_TSR0_ACTIVE_AC5
	Method (_AC5)
	{
		Return (\_SB.DPTF.CTOK (DPTF_TSR0_ACTIVE_AC5))
	}
#endif
#ifdef DPTF_TSR0_ACTIVE_AC6
	Method (_AC6)
	{
		Return (\_SB.DPTF.CTOK (DPTF_TSR0_ACTIVE_AC6))
	}
#endif
#endif
}
#endif

#ifdef DPTF_TSR1_SENSOR_ID

#ifndef DPTF_TSR1_TABLET_PASSIVE
#define DPTF_TSR1_TABLET_PASSIVE DPTF_TSR1_PASSIVE
#endif
#ifndef DPTF_TSR1_TABLET_CRITICAL
#define DPTF_TSR1_TABLET_CRITICAL DPTF_TSR1_CRITICAL
#endif

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
		Return (DTRP (DPTF_TSR1_TABLET_PASSIVE, DPTF_TSR1_PASSIVE))
	}

	Method (_CRT)
	{
		Return (DTRP (DPTF_TSR1_TABLET_CRITICAL, DPTF_TSR1_CRITICAL))
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

#ifndef DPTF_TSR2_TABLET_PASSIVE
#define DPTF_TSR2_TABLET_PASSIVE DPTF_TSR2_PASSIVE
#endif
#ifndef DPTF_TSR2_TABLET_CRITICAL
#define DPTF_TSR2_TABLET_CRITICAL DPTF_TSR2_CRITICAL
#endif

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
		Return (DTRP (DPTF_TSR2_TABLET_PASSIVE, DPTF_TSR2_PASSIVE))
	}

	Method (_CRT)
	{
		Return (DTRP (DPTF_TSR2_TABLET_CRITICAL, DPTF_TSR2_CRITICAL))
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

#ifdef DPTF_TSR3_SENSOR_ID

#ifndef DPTF_TSR3_TABLET_PASSIVE
#define DPTF_TSR3_TABLET_PASSIVE DPTF_TSR3_PASSIVE
#endif
#ifndef DPTF_TSR3_TABLET_CRITICAL
#define DPTF_TSR3_TABLET_CRITICAL DPTF_TSR3_CRITICAL
#endif

Device (TSR3)
{
	Name (_HID, EISAID ("INT3403"))
	Name (_UID, 4)
	Name (PTYP, 0x03)
	Name (TMPI, DPTF_TSR3_SENSOR_ID)
	Name (_STR, Unicode (DPTF_TSR3_SENSOR_NAME))
	Name (GTSH, 20) /* 2 degree hysteresis */

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
		Return (DTRP (DPTF_TSR3_TABLET_PASSIVE, DPTF_TSR3_PASSIVE))
	}

	Method (_CRT)
	{
		Return (DTRP (DPTF_TSR3_TABLET_CRITICAL, DPTF_TSR3_CRITICAL))
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
