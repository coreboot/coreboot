/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Inc.
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

#ifndef DPTF_CPU_PASSIVE
#define DPTF_CPU_PASSIVE	80
#endif

#ifndef DPTF_CPU_CRITICAL
#define DPTF_CPU_CRITICAL	90
#endif

#ifndef DPTF_CPU_ACTIVE_AC0
#define DPTF_CPU_ACTIVE_AC0	90
#endif

#ifndef DPTF_CPU_ACTIVE_AC1
#define DPTF_CPU_ACTIVE_AC1	80
#endif

#ifndef DPTF_CPU_ACTIVE_AC2
#define DPTF_CPU_ACTIVE_AC2	70
#endif

#ifndef DPTF_CPU_ACTIVE_AC3
#define DPTF_CPU_ACTIVE_AC3	60
#endif

#ifndef DPTF_CPU_ACTIVE_AC4
#define DPTF_CPU_ACTIVE_AC4	50
#endif

External (\_PR.CP00._TSS, MethodObj)
External (\_PR.CP00._TPC, MethodObj)
External (\_PR.CP00._PTC, PkgObj)
External (\_PR.CP00._TSD, PkgObj)
External (\_PR.CP00._PSS, MethodObj)

Device (B0DB)
{
	Name (_ADR, 0x000B0000)  /* Bus 0, Device B, Function 0 */

	Method (_STA)
	{
		If (LEqual (\DPTE, One)) {
			Return (0xF)
		} Else {
			Return (0x0)
		}
	}

	/*
	 * Processor Throttling Controls
	 */

	Method (_TSS)
	{
		If (CondRefOf (\_PR.CP00._TSS)) {
			Return (\_PR.CP00._TSS)
		} Else {
			Return (Package ()
			{
				Package () { 0, 0, 0, 0, 0 }
			})
		}
	}

	Method (_TPC)
	{
		If (CondRefOf (\_PR.CP00._TPC)) {
			Return (\_PR.CP00._TPC)
		} Else {
			Return (0)
		}
	}

	Method (_PTC)
	{
		If (CondRefOf (\_PR.CP00._PTC)) {
			Return (\_PR.CP00._PTC)
		} Else {
			Return (Package ()
			{
				Buffer () { 0 },
				Buffer () { 0 }
			})
		}
	}

	Method (_TSD)
	{
		If (CondRefOf (\_PR.CP00._TSD)) {
			Return (\_PR.CP00._TSD)
		} Else {
			Return (Package ()
			{
				Package () { 5, 0, 0, 0, 0 }
			})
		}
	}

	Method (_TDL)
	{
		If (CondRefOf (\_PR.CP00._TSS)) {
			Store (SizeOf (\_PR.CP00._TSS ()), Local0)
			Decrement (Local0)
			Return (Local0)
		} Else {
			Return (0)
		}
	}

	/*
	 * Processor Performance Control
	 */

	Method (_PPC)
	{
		Return (0)
	}

	Method (SPPC, 1)
	{
		Store (Arg0, \PPCM)

		/* Notify OS to re-read _PPC limit on each CPU */
		\PPCN ()
	}

	Method (_PSS)
	{
		If (CondRefOf (\_PR.CP00._PSS)) {
			Return (\_PR.CP00._PSS)
		} Else {
			Return (Package ()
			{
				Package () { 0, 0, 0, 0, 0, 0 }
			})
		}
	}

	Method (_PDL)
	{
		/* Check for mainboard specific _PDL override */
		If (CondRefOf (\_SB.MPDL)) {
			Return (\_SB.MPDL)
		} ElseIf (CondRefOf (\_PR.CP00._PSS)) {
			Store (SizeOf (\_PR.CP00._PSS ()), Local0)
			Decrement (Local0)
			Return (Local0)
		} Else {
			Return (0)
		}
	}

	/* Return PPCC table defined by mainboard */
	Method (PPCC)
	{
		Return (\_SB.MPPC)
	}

	Method (_CRT)
	{
		Return (\_SB.DPTF.CTOK(DPTF_CPU_CRITICAL))
	}

	Method (_PSV)
	{
		Return (\_SB.DPTF.CTOK(DPTF_CPU_PASSIVE))
	}

	Method (_AC0)
	{
		Return (\_SB.DPTF.CTOK(DPTF_CPU_ACTIVE_AC0))
	}

	Method (_AC1)
	{
		Return (\_SB.DPTF.CTOK(DPTF_CPU_ACTIVE_AC1))
	}

	Method (_AC2)
	{
		Return (\_SB.DPTF.CTOK(DPTF_CPU_ACTIVE_AC2))
	}

	Method (_AC3)
	{
		Return (\_SB.DPTF.CTOK(DPTF_CPU_ACTIVE_AC3))
	}

	Method (_AC4)
	{
		Return (\_SB.DPTF.CTOK(DPTF_CPU_ACTIVE_AC4))
	}
}
