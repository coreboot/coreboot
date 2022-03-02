/* SPDX-License-Identifier: GPL-2.0-only */

/* DefinitionBlock Statement */
#include <acpi/acpi.h>
DefinitionBlock (
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x00010001	/* OEM Revision */
	)
{	/* Start of ASL file */
	#include <acpi/dsdt_top.asl>

	/* global NVS and variables */
	#include <globalnvs.asl>

	/* Describe the USB Overcurrent pins */
	#include "acpi/usb_oc.asl"

	/* PCI IRQ mapping for the Southbridge */
	#include <pcie.asl>

	/* Power state notification */
	#include <pnot.asl>

	/* Contains the supported sleep states for this chipset */
	#include <soc/amd/common/acpi/sleepstates.asl>

	/* Contains the Sleep methods (WAK, PTS, GTS, etc.) */
	#include "acpi/sleep.asl"

	/* System Bus */
	Scope(\_SB) { /* Start \_SB scope */
		/* global utility methods expected within the \_SB scope */
		#include <arch/x86/acpi/globutil.asl>

		/* IRQ Routing mapping for this platform (in \_SB scope) */
		#include "acpi/routing.asl"

		Device(PWRB) {
			Name(_HID, EISAID("PNP0C0C"))
			Name(_UID, 0xAA)
			Name(_PRW, Package () {3, 0x04})
			Name(_STA, 0x0B)
		}

		/* Describe the SOC */
		#include <soc.asl>

		/* Describe the Fintek F81803A SIO */
		#define SUPERIO_DEV SIO0
		#define SUPERIO_PNP_BASE 0x4E
		#define F81803A_SHOW_UARTA
		#define F81803A_SHOW_PME
		#include <superio/fintek/f81803a/acpi/superio.asl>

	} /* End \_SB scope */

	/* Define the General Purpose Events for the platform */
	#include "acpi/gpe.asl"
}
/* End of ASL file */
