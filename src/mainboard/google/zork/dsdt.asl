/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <variant/ec.h>

/* DefinitionBlock Statement */
#include <acpi/acpi.h>

DefinitionBlock (
	"DSDT.AML",	/* Output filename */
	"DSDT",		/* Signature */
	0x02,		/* DSDT Revision, needs to be 2 for 64bit */
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x00010001	/* OEM Revision */
	)
{	/* Start of ASL file */
	/* #include <arch/x86/acpi/debug.asl> */	/* as needed */

	/* global NVS and variables */
	#include <globalnvs.asl>

	/* Globals for the platform */
	#include <variant/acpi/mainboard.asl>

	/* PCI IRQ mapping for the Southbridge */
	#include <pcie.asl>

	/* Describe the processor tree (\_PR) */
	#include <cpu.asl>

	/* Contains the supported sleep states for this chipset */
	#include <sleepstates.asl>

	/* Contains the Sleep methods (WAK, PTS, GTS, etc.) */
	#include <variant/acpi/sleep.asl>

	/* Contains _SWS methods */
	#include <acpi_wake_source.asl>

	/* System Bus */
	Scope(\_SB) { /* Start \_SB scope */
		/* global utility methods expected within the \_SB scope */
		#include <arch/x86/acpi/globutil.asl>

		/* Describe the SOC */
		#include <soc.asl>

	} /* End \_SB scope */

	/* Thermal handler */
	#include <variant/acpi/thermal.asl>

	/* Chrome OS specific */
	#include <vendorcode/google/chromeos/acpi/chromeos.asl>

	/* Chrome OS Embedded Controller */
	Scope (\_SB.PCI0.LPCB)
	{
		/* ACPI code for EC SuperIO functions */
		#include <ec/google/chromeec/acpi/superio.asl>
		/* ACPI code for EC functions */
		#include <ec/google/chromeec/acpi/ec.asl>
		/* ACPI code for EC I2C Audio Tunnel */
		#include <variant/acpi/audio.asl>
	}
}
/* End of ASL file */
