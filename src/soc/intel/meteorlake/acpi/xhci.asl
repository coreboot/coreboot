/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/gpe.h>

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
		UWES ((\U2WE & 0xFFF), PORTSCN_OFFSET, XMEM)
		UWES ((\U3WE & 0x3F ), PORTSCXUSB3_OFFSET, XMEM)
	}

	Name (_S3D, 3)	/* D3 supported in S3 */
	Name (_S0W, 3)	/* D3 can wake device in S0 */
	Name (_S3W, 3)	/* D3 can wake system from S3 */

	Method (_PS0, 0, Serialized)
	{
		/* Disable Clock Gating */
		^^PCRA (PID_XHCI, 0x0,  ~(1 << 3))
	}

	Method (_PS3, 0, Serialized)
	{
		/* Enable Clock Gating */
		^^PCRO (PID_XHCI, 0x0, 1 << 3)
	}

	/* Root Hub for Meteorlake */
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
		Device (HS09) { Name (_ADR, 9) }
		Device (HS10) { Name (_ADR, 10) }
		/* USB3 */
		Device (SS01) { Name (_ADR, 11) }
		Device (SS02) { Name (_ADR, 12) }
	}
}
