/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2015 Google Inc.
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

/*
 * USB Port Wake Enable (UPWE) on usb attach/detach
 * Arg0 - Port Number
 * Arg1 - Port 1 Status and control offset
 * Arg2 - xHCI Memory-mapped address
 */
Method (UPWE, 3, Serialized)
{
	/* Local0 = Arg1 + ((Arg0 - 1) * 0x10) */
	Add (Arg1, Multiply (Subtract (Arg0, 1), 0x10), Local0)

	/* Map ((XMEM << 16) + Local0 in PSCR */
	OperationRegion (PSCR, SystemMemory,
			 Add (ShiftLeft (Arg2, 16), Local0), 0x10)
	Field (PSCR, DWordAcc, NoLock, Preserve)
	{
		PSCT, 32,
	}
	Store(PSCT, Local0)
	/*
	 * And port status/control reg with RO and RWS bits
	 * RO bits: 0, 2:3, 10:13, 24, 28:30
	 * RWS bits: 5:9, 14:16, 25:27
	 */
	And (Local0, ~0x80FE0012, Local0)
	/* Set WCE and WDE bits */
	Or (Local0, 0x6000000, Local0)
	Store(Local0, PSCT)
}

/*
 * USB Wake Enable Setup (UWES)
 * Arg0 - Port enable bitmap
 * Arg1 - Port 1 Status and control offset
 * Arg2 - xHCI Memory-mapped address
 */
Method (UWES, 3, Serialized)
{
	Store (Arg0, Local0)

	While (One) {
		FindSetRightBit (Local0, Local1)
		If (LEqual (Local1, Zero)) {
			Break
		}
		UPWE (Local1, Arg1, Arg2)
		/*
		 * Clear the lowest set bit in Local0 since it was
		 * processed.
		 * Local0 = Local0 & (Local0 - 1)
		 */
		And (Local0, Subtract (Local0, 1), Local0)
	}
}

/* XHCI Controller 0:14.0 */

Device (XHCI)
{
	Name (_ADR, 0x00140000)

	Name (_PRW, Package () { GPE0_PME_B0, 3 })

	Method (_DSW, 3)
	{
		Store (Arg0, PMEE)
		UWES (And (\U2WE, 0x3FF), 0x480, XMEM)
		UWES (And (\U3WE, 0x3F), 0x540, XMEM)
	}

	Name (_S3D, 3)	/* D3 supported in S3 */
	Name (_S4D, 3)	/* D3 supported in S4 */
	Name (_S0W, 3)	/* D3 can wake device in S0 */
	Name (_S3W, 3)	/* D3 can wake system from S3 */
	Name (_S4W, 3)	/* D3 can wake system from S4 */

	OperationRegion (XPRT, PCI_Config, 0x00, 0x100)
	Field (XPRT, AnyAcc, NoLock, Preserve)
	{
		Offset (0x0),
		DVID, 16,	/* VENDORID */
		Offset (0x10),
		, 16,
		XMEM, 16,	/* MEM_BASE */
		Offset (0x50),	/* XHCLKGTEN */
		, 2,
		STGE, 1,	/* SS Link Trunk clock gating enable */
		Offset (0x74),
		D0D3, 2,	/* POWERSTATE */
		, 6,
		PMEE, 1,	/* PME_EN */
		, 6,
		PMES, 1,	/* PME_STS */
		Offset (0xA2),
		, 2,
		D3HE, 1,	/* D3_hot_en */
	}

	OperationRegion (XREG, SystemMemory,
			 Add (ShiftLeft (XMEM, 16), 0x8000), 0x200)
	Field (XREG, DWordAcc, Lock, Preserve)
	{
		Offset (0x1c4),	/* USB2PMCTRL */
		, 2,
		UPSW, 2,	/* U2PSUSPGP */
	}

	Method (_PSC, 0, Serialized)
	{
		Return (^D0D3)
	}

	Method (_PS0, 0, Serialized)
	{
		If (LEqual (^DVID, 0xFFFF)) {
			Return
		}
		If (LOr (LEqual (^XMEM, 0xFFFF), LEqual (^XMEM, 0x0000))) {
			Return
		}

		/* Disable d3hot and SS link trunk clock gating */
		Store(Zero, ^D3HE)
		Store(Zero, ^STGE)

		/* If device is in D3, set back to D0 */
		If (LEqual (^D0D3, 3)) {
			Store (Zero, Local0)
			Store (Local0, ^D0D3)
			Store (^D0D3, Local0)
		}

		/* Disable USB2 PHY SUS Well Power Gating */
		Store (Zero, ^UPSW)

		/*
		 * Apply USB2 PHPY Power Gating workaround if needed.
		 */
		If (^^PMC.UWAB) {
			/* Write to MTPMC to have PMC disable power gating */
			Store (1, ^^PMC.MPMC)

			/* Wait for PCH_PM_STS.MSG_FULL_STS to be 0 */
			Store (10, Local0)
			While (^^PMC.PMFS) {
				If (LNot (Local0)) {
					Break
				}
				Decrement (Local0)
				Sleep (10)
			}
		}
	}

	Method (_PS3, 0, Serialized)
	{
		If (LEqual (^DVID, 0xFFFF)) {
			Return
		}
		If (LOr (LEqual (^XMEM, 0xFFFF), LEqual (^XMEM, 0x0000))) {
			Return
		}

		/* Clear PME Status */
		Store (1, ^PMES)

		/* Enable PME */
		Store (1, ^PMEE)

		/* If device is in D3, set back to D0 */
		If (LEqual (^D0D3, 3)) {
			Store (Zero, Local0)
			Store (Local0, ^D0D3)
			Store (^D0D3, Local0)
		}

		/* Enable USB2 PHY SUS Well Power Gating in D0/D0i2/D0i3/D3 */
		Store (3, ^UPSW)

		/* Enable d3hot and SS link trunk clock gating */
                Store(One, ^D3HE)
                Store(One, ^STGE)

		/* Now put device in D3 */
		Store (3, Local0)
		Store (Local0, ^D0D3)
		Store (^D0D3, Local0)

		/*
		 * Apply USB2 PHPY Power Gating workaround if needed.
		 * This code assumes XDCI is disabled, if it is enabled
		 * then this must also check if it is in D3 state too.
		 */
		If (^^PMC.UWAB) {
			/* Write to MTPMC to have PMC enable power gating */
			Store (3, ^^PMC.MPMC)

			/* Wait for PCH_PM_STS.MSG_FULL_STS to be 0 */
			Store (10, Local0)
			While (^^PMC.PMFS) {
				If (LNot (Local0)) {
					Break
				}
				Decrement (Local0)
				Sleep (10)
			}
		}
	}

	/* Root Hub for Skylake-LP PCH */
	Device (RHUB)
	{
		Name (_ADR, Zero)

		// GPLD: Generate Port Location Data (PLD)
		Method (GPLD, 1, Serialized)
		 {

			Name (PCKG, Package (0x01)
			{
				Buffer (0x10) {}
			})

			// REV: Revision 0x02 for ACPI 5.0
			CreateField (DerefOf (Index (PCKG, Zero)), Zero, 0x07, REV)
			Store (0x02, REV)

			// VISI: Port visibility to user per port
			CreateField (DerefOf (Index (PCKG, Zero)), 0x40, One, VISI)
			Store (Arg0, VISI)

			Return (PCKG)
		}

		/* USB2 */
		Device (HS01) { Name (_ADR, 1) }
		Device (HS02) { Name (_ADR, 2) }
		Device (HS03) { Name (_ADR, 3) }
		Device (HS04) { Name (_ADR, 4) }
		Device (HS05) { Name (_ADR, 5) }
		Device (HS06) { Name (_ADR, 6) }
		Device (HS07) { Name (_ADR, 7) }
		Device (HS08) { Name (_ADR, 8) }
		Device (HS09) { Name (_ADR, 9) }
		Device (HS10) { Name (_ADR, 10) }

		/* USBr */
		Device (USR1) { Name (_ADR, 11) }
		Device (USR2) { Name (_ADR, 12) }

		/* USB3 */
		Device (SS01) { Name (_ADR, 13) }
		Device (SS02) { Name (_ADR, 14) }
		Device (SS03) { Name (_ADR, 15) }
		Device (SS04) { Name (_ADR, 16) }
		Device (SS05) { Name (_ADR, 17) }
		Device (SS06) { Name (_ADR, 18) }
	}
}
