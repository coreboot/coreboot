/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/gpe.h>

/*
 * JSL has 8 USB2 ports, so the USB3 PORTSC registers start at
 * 0x480 + 8 * 0x10 = 0x500
 */
#define JSL_PORTSCXUSB3_OFFSET 0x500

/* Include UWES method for enabling USB wake */
#include <soc/intel/common/acpi/xhci_wake.asl>

/* XHCI Controller 0:14.0 */

Device (XHCI)
{
	Name (_ADR, 0x00140000)

	Name (_PRW, Package () { GPE0_PME_B0, 3 })

	OperationRegion (XPRT, PCI_Config, 0x00, 0x100)
	Field (XPRT, AnyAcc, NoLock, Preserve)
	{
		Offset (0x10),
		, 16,
		XMEM, 16,	/* MEM_BASE */
	}

	Method (_DSW, 3)
	{
		UWES ((\U2WE & 0xFF), PORTSCN_OFFSET, XMEM)
		UWES ((\U3WE & 0x3F ), JSL_PORTSCXUSB3_OFFSET, XMEM)
	}

	Name (_S3D, 3)	/* D3 supported in S3 */
	Name (_S0W, 3)	/* D3 can wake device in S0 */
	Name (_S3W, 3)	/* D3 can wake system from S3 */

	Method (_PS0, 0, Serialized)
	{

	}

	Method (_PS3, 0, Serialized)
	{

	}

	/* Root Hub for Jasperlake PCH */
	Device (RHUB)
	{
		Name (_ADR, 0)

		/* USB2 */
		Device (HS01) { Name (_ADR, 1) }
		Device (HS02) { Name (_ADR, 2) }
		Device (HS03) { Name (_ADR, 3) }
		Device (HS04) { Name (_ADR, 4) }
		Device (HS05) { Name (_ADR, 5) }
		Device (HS06) { Name (_ADR, 6) }
		Device (HS07) { Name (_ADR, 7) }
		Device (HS08) { Name (_ADR, 8) }

		/* USB3 */
		Device (SS01) { Name (_ADR, 9) }
		Device (SS02) { Name (_ADR, 10) }
		Device (SS03) { Name (_ADR, 11) }
		Device (SS04) { Name (_ADR, 12) }
		Device (SS05) { Name (_ADR, 13) }
		Device (SS06) { Name (_ADR, 14) }
	}
}
