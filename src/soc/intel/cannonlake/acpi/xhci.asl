/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/gpe.h>

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
		If (Local1 == Zero) {
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

	Name (_PRW, Package () { GPE0_PME_B0, 4 })

	Method (_DSW, 3)
	{
		UWES ((\U2WE & 0xFFF), PORTSCN_OFFSET, XMEM)
		UWES ((\U3WE & 0x3F ), PORTSCXUSB3_OFFSET, XMEM)
	}

	Name (_S3D, 3)	/* D3 supported in S3 */
	Name (_S0W, 3)	/* D3 can wake device in S0 */
	Name (_S3W, 3)	/* D3 can wake system from S3 */

	Name (_S4D, 3)	/* D3 supported in S4 */
	Name (_S4W, 3)	/* D3 can wake system from S4 */

	OperationRegion (XPRT, PCI_Config, 0x00, 0x100)
	Field (XPRT, AnyAcc, NoLock, Preserve)
	{
		Offset (0x10),
		, 16,
		XMEM, 16,	/* MEM_BASE */
	}

	Method (_PS0, 0, Serialized)
	{

	}

	Method (_PS3, 0, Serialized)
	{

	}

	/* Root Hub for Cannonlake-LP PCH */
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
		Device (HS11) { Name (_ADR, 11) }
		Device (HS12) { Name (_ADR, 12) }

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
