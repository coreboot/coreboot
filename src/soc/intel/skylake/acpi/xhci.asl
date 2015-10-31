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

/* XHCI Controller 0:14.0 */

Device (XHCI)
{
	Name (_ADR, 0x00140000)

	Name (_PRW, Package () { GPE0_PME_B0, 3 })

	Method (_DSW, 3)
	{
		Store (Arg0, PMEE)
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
		Offset (0x74),
		D0D3, 2,	/* POWERSTATE */
		, 6,
		PMEE, 1,	/* PME_EN */
		, 6,
		PMES, 1,	/* PME_STS */
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
