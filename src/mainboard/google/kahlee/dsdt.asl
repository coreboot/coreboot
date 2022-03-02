/* SPDX-License-Identifier: GPL-2.0-only */

#include <variant/ec.h>

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

	/* Globals for the platform */
	#include <variant/acpi/mainboard.asl>

	/* PCI IRQ mapping for the Southbridge */
	#include <pcie.asl>

	/* Power state notification */
	#include <pnot.asl>

	/* Contains the supported sleep states for this chipset */
	#include <soc/amd/common/acpi/sleepstates.asl>

	/* Contains the Sleep methods (WAK, PTS, GTS, etc.) */
	#include <variant/acpi/sleep.asl>

	/* Contains _SWS methods */
	#include <soc/amd/common/acpi/acpi_wake_source.asl>

	/* System Bus */
	Scope(\_SB) { /* Start \_SB scope */
		/* global utility methods expected within the \_SB scope */
		#include <arch/x86/acpi/globutil.asl>

		/* IRQ Routing mapping for this platform (in \_SB scope) */
		#include <variant/acpi/routing.asl>

		/* Describe the SOC */
		#include <soc.asl>

	} /* End \_SB scope */

	/* Thermal handler */
	#include <variant/acpi/thermal.asl>

	/* Chrome OS Embedded Controller */
	Scope (\_SB.PCI0.LPCB)
	{
		/* ACPI code for EC SuperIO functions */
		#include <ec/google/chromeec/acpi/superio.asl>
		/* ACPI code for EC functions */
		#include <ec/google/chromeec/acpi/ec.asl>
	}

	/* Define the General Purpose Events for the platform */
	#include <variant/acpi/gpe.asl>
}
/* End of ASL file */
