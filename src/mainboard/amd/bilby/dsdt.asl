/* SPDX-License-Identifier: GPL-2.0-only */

#define MAINBOARD_HAS_SPEAKER 1

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
	#include <globalnvs.asl>

	/* PCI IRQ mapping for the Southbridge */
	#include <pci_int_defs.asl>

	/* Power state notification to ALIB */
	#include <pnot.asl>

	/* Contains the supported sleep states for this chipset */
	#include <soc/amd/common/acpi/sleepstates.asl>

	/* Contains _SWS methods */
	#include <soc/amd/common/acpi/acpi_wake_source.asl>

	/* System Bus */
	Scope(\_SB) { /* Start \_SB scope */
		/* global utility methods expected within the \_SB scope */
		#include <arch/x86/acpi/globutil.asl>

		/* Describe the SOC */
		#include <soc.asl>

	} /* End \_SB scope */
}
/* End of ASL file */
