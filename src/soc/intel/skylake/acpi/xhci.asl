/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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

/* XHCI Controller 0:14.0 */

Device (XHCI)
{
	Name (_ADR, 0x00140000)

	Name (PLSD, 5) /* Port Link State - RxDetect */
	Name (PLSP, 7) /* Port Link State - Polling */

	OperationRegion (XPRT, PCI_Config, 0x00, 0x100)
	Field (XPRT, AnyAcc, NoLock, Preserve)
	{
		Offset (0x0),
		DVID, 16,
		Offset (0x10),
		, 16,
		XMEM, 16,	/* MEM_BASE */
		Offset (0x74),
		D0D3, 2,
		, 6,
		PMEE, 1,	/* PME_EN */
		, 6,
		PMES, 1,	/* PME_STS */
		Offset (0xA8),
		, 13,
		MW13, 1,
		MW14, 1,
		, 17,
		Offset (0xb0),
		, 13,
		MB13, 1,
		MB14, 1,
		, 17,
		Offset (0xd0),
		PR2, 32,	/* USB2PR */
		PR2M, 32,	/* USB2PRM */
		PR3, 32,	/* USB3PR */
		PR3M, 32,	/* USB3PRM */
	}

	Method (USRA,0){
		Return(11)
	}

	Method (SSPA,0){
		Return (13)
	}

	/* Clear status bits */
	Method (LPCL, 0, Serialized)
	{
		OperationRegion (XREG, SystemMemory,
			 ShiftLeft (^XMEM, 16), 0x600)
		Field (XREG, DWordAcc, Lock, Preserve)
		{
			Offset (0x510), /* PORTSCNUSB3[0]*/
			PSC0, 32,
			Offset (0x520), /* PORTSCNUSB3[1]*/
			PSC1, 32,
			Offset (0x530), /* PORTSCNUSB3[2]*/
			PSC2, 32,
			Offset (0x540), /* PORTSCNUSB3[3]*/
			PSC3, 32,
		}

		/* Port Enabled/Disabled (Bit 1)*/
		Name (PEDB, ShiftLeft (1, 1))

		/* Change Status (Bits 23:17)*/
		Name (CHST, ShiftLeft (0x7f, 17))

		/* Port 0 */
		And (PSC0, Not (PEDB), Local0)
		Or (Local0, CHST, PSC0)

		/* Port 1 */
		And (PSC1, Not (PEDB), Local0)
		Or (Local0, CHST, PSC1)

		/* Port 2 */
		And (PSC2, Not (PEDB), Local0)
		Or (Local0, CHST, PSC2)

		/* Port 3 */
		And (PSC3, Not (PEDB), Local0)
		Or (Local0, CHST, PSC3)
	}

	Method (LPS0, 0, Serialized)
	{
		OperationRegion (XREG, SystemMemory,
				 ShiftLeft (^XMEM, 16), 0x600)
		Field (XREG, DWordAcc, Lock, Preserve)
		{
			Offset (0x510), // PORTSCNUSB3
			, 5,
			PLS1, 4,	// [8:5] Port Link State
			PPR1, 1,	// [9] Port Power
			, 7,
			CSC1, 1,	// [17] Connect Status Change
			, 1,
			WRC1, 1,	// [19] Warm Port Reset Change
			, 11,
			WPR1, 1,	// [31] Warm Port Reset
			Offset (0x520), // PORTSCNUSB3
			, 5,
			PLS2, 4,	// [8:5] Port Link State
			PPR2, 1,	// [9] Port Power
			, 7,
			CSC2, 1,	// [17] Connect Status Change
			, 1,
			WRC2, 1,	// [19] Warm Port Reset Change
			, 11,
			WPR2, 1,	// [31] Warm Port Reset
			Offset (0x530), // PORTSCNUSB3
			, 5,
			PLS3, 4,	// [8:5] Port Link State
			PPR3, 1,	// [9] Port Power
			, 7,
			CSC3, 1,	// [17] Connect Status Change
			, 1,
			WRC3, 1,	// [19] Warm Port Reset Change
			, 11,
			WPR3, 1,	// [31] Warm Port Reset
			Offset (0x540), // PORTSCNUSB3
			, 5,
			PLS4, 4,	// [8:5] Port Link State
			PPR4, 1,	// [9] Port Power
			, 7,
			CSC4, 1,	// [17] Connect Status Change
			, 1,
			WRC4, 1,	// [19] Warm Port Reset Change
			, 11,
			WPR4, 1,	// [31] Warm Port Reset
		}

		/* Wait for all powered ports to finish polling*/
		Store (10, Local0)
		While (LOr (LOr (LAnd (LEqual (PPR1, 1), LEqual (PLS1, PLSP)),
				 LAnd (LEqual (PPR2, 1), LEqual (PLS2, PLSP))),
			    LOr (LAnd (LEqual (PPR3, 1), LEqual (PLS3, PLSP)),
				 LAnd (LEqual (PPR4, 1), LEqual (PLS4, PLSP)))))
		{
			If (LEqual (Local0, 0)) {
				Break
			}
			Decrement (Local0)
			Stall (10)
		}

		/* For each USB3 Port:*/
		/*   If port is disconnected (PLS=5 PP=1 CSC=0)*/
		/*     1) Issue warm reset (WPR=1)*/
		/*     2) Poll for warm reset complete (WRC=0)*/
		/*     3) Write 1 to port status to clear*/

		/* Local# indicate if port is reset*/
		Store (0, Local1)
		Store (0, Local2)
		Store (0, Local3)
		Store (0, Local4)

		If (LAnd (LEqual (PLS1, PLSD),
		          LAnd (LEqual (CSC1, 0), LEqual (PPR1, 1)))) {
			Store (1, WPR1)	      /* Issue warm reset*/
			Store (1, Local1)
		}
		If (LAnd (LEqual (PLS2, PLSD),
		          LAnd (LEqual (CSC2, 0), LEqual (PPR2, 1)))) {
			Store (1, WPR2)	      /* Issue warm reset*/
			Store (1, Local2)
		}
		If (LAnd (LEqual (PLS3, PLSD),
		          LAnd (LEqual (CSC3, 0), LEqual (PPR3, 1)))) {
			Store (1, WPR3)	      /* Issue warm reset*/
			Store (1, Local3)
		}
		If (LAnd (LEqual (PLS4, PLSD),
		          LAnd (LEqual (CSC4, 0), LEqual (PPR4, 1)))) {
			Store (1, WPR4)	      /* Issue warm reset*/
			Store (1, Local4)
		}

		/* Poll for warm reset complete on all ports that were reset*/
		Store (10, Local0)
		While (LOr (LOr (LAnd (LEqual (Local1, 1), LEqual (WRC1, 0)),
				 LAnd (LEqual (Local2, 1), LEqual (WRC2, 0))),
			    LOr (LAnd (LEqual (Local3, 1), LEqual (WRC3, 0)),
			         LAnd (LEqual (Local4, 1), LEqual (WRC4, 0)))))
		{
			If (LEqual (Local0, 0)) {
				Break
			}
			Decrement (Local0)
			Stall (10)
		}

		/* Clear status bits in all ports */
		LPCL ()
	}

	Method (_PSC, 0, NotSerialized)
	{
		Return (^D0D3)
	}

	Method (_PS0, 0, Serialized)
	{
	}
	Method (_PS3, 0, Serialized)
	{
	}

	Name (_PRW, Package(){ 0x6d, 3 })

	/* Leave USB ports on for to allow Wake from USB */

	Method (_S3D,0)	/* Highest D State in S3 State*/
	{
		Return (3)
	}

	Method (_S4D,0)	/* Highest D State in S4 State*/
	{
		Return (3)
	}
	Device (HS01)
	{
		Name(_ADR, 0x01)
	}
	Device (HS02)
	{
		Name(_ADR, 0x02)
	}
	Device (HS03)
	{
		Name(_ADR, 0x03)
	}
	Device (HS04)
	{
		Name(_ADR, 0x04)
	}
	Device (HS05)
	{
		Name(_ADR, 0x05)
	}
	Device (HS06)
	{
		Name(_ADR, 0x06)
	}
	Device (HS07)
	{
		Name(_ADR, 0x07)
	}
	Device (HS08)
	{
		Name(_ADR, 0x08)
	}
	Device (HS09)
	{
		Name(_ADR, 0x09)
	}
	Device (HS10)
	{
		Name(_ADR, 0x10)
	}
	Device (USR1)
	{
		Method(_ADR) { Return (Add(USRA(),0)) }
	}
	Device (USR2)
	{
		Method(_ADR) { Return (Add(USRA(),1)) }
	}
	Device (SS01)
	{
		Method(_ADR) { Return (Add(SSPA(),0)) }
	}
	Device (SS02)
	{
		Method(_ADR) { Return (Add(SSPA(),1)) }
	}
	Device (SS03)
	{
		Method(_ADR) { Return (Add(SSPA(),2)) }
	}
	Device (SS04)
	{
		Method(_ADR) { Return (Add(SSPA(),3)) }
	}
	Device (SS05)
	{
		Method(_ADR) { Return (Add(SSPA(),4)) }
	}
	Device (SS06)
	{
		Method(_ADR) { Return (Add(SSPA(),5)) }
	}
}

