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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

Device (DPTF)
{
	Name (_HID, EISAID ("INT3400"))
	Name (_UID, 0)

	Name (IDSP, Package()
	{
		/* DPPM Passive Policy 1.0 */
		ToUUID ("42A441D6-AE6A-462B-A84B-4A8CE79027D3"),

		/* DPPM Critical Policy */
		ToUUID ("97C68AE7-15FA-499c-B8C9-5DA81D606E0A"),

		/* DPPM Cooling Policy */
		ToUUID ("16CAF1B7-DD38-40ED-B1C1-1B8A1913D531"),
	})

	Method (_STA)
	{
		If (LEqual (\DPTE, One)) {
			Return (0xF)
		} Else {
			Return (0x0)
		}
	}

	/*
	 * Arg0: Buffer containing UUID
	 * Arg1: Integer containing Revision ID of buffer format
	 * Arg2: Integer containing count of entries in Arg3
	 * Arg3: Buffer containing list of DWORD capabilities
	 * Return: Buffer containing list of DWORD capabilities
	 */
	Method (_OSC, 4, Serialized)
	{
		/* Check for Passive Policy UUID */
		If (LEqual (DeRefOf (Index (IDSP, 0)), Arg0)) {
			/* Initialize Thermal Devices */
			^TINI ()

#ifdef DPTF_ENABLE_CHARGER
			/* Initialize Charger Device */
			^TCHG.INIT ()
#endif
		}

		Return (Arg3)
	}

	/* Priority based _TRT */
	Name (TRTR, 1)

	Method (_TRT)
	{
		Return (\_SB.DTRT)
	}

	/* Convert from Degrees C to 1/10 Kelvin for ACPI */
	Method (CTOK, 1) {
		/* 10th of Degrees C */
		Multiply (Arg0, 10, Local0)

		/* Convert to Kelvin */
		Add (Local0, 2732, Local0)

		Return (Local0)
	}

	/* Include Thermal Participants */
	#include "thermal.asl"

#ifdef DPTF_ENABLE_CHARGER
	/* Include Charger Participant */
	#include "charger.asl"
#endif

}

Scope (\_SB.PCI0)
{
	#include "cpu.asl"
}
