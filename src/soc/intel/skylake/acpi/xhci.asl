/* SPDX-License-Identifier: GPL-2.0-only */

#define PORTSCN_OFFSET 0x480
#define PORTSCXUSB3_OFFSET 0x540

#define WAKE_ON_CONNECT_DISCONNECT_ENABLE 0x6000000
#define RO_BITS_OFF_MASK ~0x80FE0012

/*
 * USB Port Wake Enable (UPWE) on usb attach/detach
 * Arg0 - Port Number
 * Arg1 - Port 1 Status and control offset
 * Arg2 - xHCI Memory-mapped address
 */
Method (UPWE, 3, Serialized)
{
	Local0 = Arg1 + ((Arg0 - 1) * 0x10)

	/* Map ((XMEM << 16) + Local0 in PSCR */
	OperationRegion (PSCR, SystemMemory, (Arg2 << 16) + Local0, 0x10)
	Field (PSCR, DWordAcc, NoLock, Preserve)
	{
		PSCT, 32,
	}
	Local0 = PSCT
	/*
	 * And port status/control reg with RO and RWS bits
	 * RO bits: 0, 2:3, 10:13, 24, 28:30
	 * RWS bits: 5:9, 14:16, 25:27
	 */
	Local0 = Local0 & RO_BITS_OFF_MASK
	/* Set WCE and WDE bits */
	Local0 = Local0 | WAKE_ON_CONNECT_DISCONNECT_ENABLE
	PSCT = Local0
}

/*
 * USB Wake Enable Setup (UWES)
 * Arg0 - Port enable bitmap
 * Arg1 - Port 1 Status and control offset
 * Arg2 - xHCI Memory-mapped address
 */
Method (UWES, 3, Serialized)
{
	Local0 = Arg0

	While (1) {
		FindSetRightBit (Local0, Local1)
		If (Local1 == 0) {
			Break
		}
		UPWE (Local1, Arg1, Arg2)
		/*
		 * Clear the lowest set bit in Local0 since it was
		 * processed.
		 */
		Local0 = Local0 & (Local0 - 1)
	}
}

/* XHCI Controller 0:14.0 */

Device (XHCI)
{
	Name (_ADR, 0x00140000)

	Name (_PRW, Package () { GPE0_PME_B0, 3 })

	Method (_DSW, 3)
	{
		PMEE = Arg0
		UWES ((\U2WE & 0x3FF), PORTSCN_OFFSET, XMEM)
		UWES ((\U3WE & 0x3F ), PORTSCXUSB3_OFFSET, XMEM)
	}

	Name (_S3D, 3)	/* D3 supported in S3 */
	Name (_S4D, 3)	/* D3 supported in S4 */
	Name (_S0W, 3)	/* D3 can wake device in S0 */
	Name (_S3W, 3)	/* D3 can wake system from S3 */
	Name (_S4W, 3)	/* D3 can wake system from S4 */

	OperationRegion (XPRT, PCI_Config, 0x00, 0x100)
	Field (XPRT, AnyAcc, NoLock, Preserve)
	{
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

	OperationRegion (XREG, SystemMemory, (XMEM << 16) + 0x8000, 0x200)
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
		If (^DVID != 0xFFFF) {
			If (!((^XMEM == 0xFFFF) || (^XMEM == 0x0000))) {

				/* Disable d3hot and SS link trunk clock gating */
				^D3HE = 0
				^STGE = 0

				/* If device is in D3, set back to D0 */
				If (^D0D3 == 3) {
					Local0 = 0
					^D0D3 = Local0
					Local0 = ^D0D3
				}

				/* Disable USB2 PHY SUS Well Power Gating */
				^UPSW = 0

				/*
				* Apply USB2 PHPY Power Gating workaround if needed.
				*/
				If (^^PMC.UWAB) {
					/* Write to MTPMC to have PMC disable power gating */
					^^PMC.MPMC = 1

					/* Wait for PCH_PM_STS.MSG_FULL_STS to be 0 */
					Local0 = 10
					While (^^PMC.PMFS) {
						If (!Local0) {
							Break
						}
						Local0--
						Sleep (10)
					}
				}
			}
		}
	}

	Method (_PS3, 0, Serialized)
	{
		If (^DVID != 0xFFFF) {
			If (!((^XMEM == 0xFFFF) || (^XMEM == 0x0000))) {

				/* Clear PME Status */
				^PMES = 1

				/* Enable PME */
				^PMEE= 1

				/* If device is in D3, set back to D0 */
				If (^D0D3 == 3) {
					Local0 = 0
					^D0D3 = Local0
					Local0 = ^D0D3
				}

				/* Enable USB2 PHY SUS Well Power Gating in D0/D0i2/D0i3/D3 */
				^UPSW = 3

				/* Enable d3hot and SS link trunk clock gating */
				^D3HE = 1
				^STGE = 1

				/* Now put device in D3 */
				Local0 = 3
				^D0D3 = Local0
				Local0 = ^D0D3

				/*
				* Apply USB2 PHPY Power Gating workaround if needed.
				* This code assumes XDCI is disabled, if it is enabled
				* then this must also check if it is in D3 state too.
				*/
				If (^^PMC.UWAB) {
					/* Write to MTPMC to have PMC enable power gating */
					^^PMC.MPMC = 3

					/* Wait for PCH_PM_STS.MSG_FULL_STS to be 0 */
					Local0 = 10
					While (^^PMC.PMFS) {
						If (!Local0) {
							Break
						}
						Local0--
						Sleep (10)
					}
				}
			}
		}
	}

	/* Root Hub for Skylake-LP PCH */
	Device (RHUB)
	{
		Name (_ADR, 0)

		// GPLD: Generate Port Location Data (PLD)
		Method (GPLD, 1, Serialized)
		 {

			Name (PCKG, Package (0x01)
			{
				Buffer (0x10) {}
			})

			// REV: Revision 0x02 for ACPI 5.0
			CreateField (DerefOf (PCKG[0]), 0, 0x07, REV)
			REV = 0x02

			// VISI: Port visibility to user per port
			CreateField (DerefOf (PCKG[0]), 0x40, 1, VISI)
			VISI = Arg0

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
