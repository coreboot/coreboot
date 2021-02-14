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

	/* Globals for the platform */
	#include "acpi/mainboard.asl"

	/* Describe the USB Overcurrent pins */
	#include "acpi/usb_oc.asl"

	/* PCI IRQ mapping for the Southbridge */
	#include <southbridge/amd/pi/hudson/acpi/pcie.asl>

	/* Describe the processor tree (\_SB) */
	#include <cpu/amd/pi/00730F01/acpi/cpu.asl>

	/* Contains the supported sleep states for this chipset */
	#include <southbridge/amd/common/acpi/sleepstates.asl>

	/* Contains the Sleep methods (WAK, PTS, GTS, etc.) */
	#include "acpi/sleep.asl"

	/* System Bus */
	Scope(\_SB) { /* Start \_SB scope */
		/* global utility methods expected within the \_SB scope */
		#include <arch/x86/acpi/globutil.asl>

		/* Describe IRQ Routing mapping for this platform (within the \_SB scope) */
		#include "acpi/routing.asl"

		Device(PCI0) {
			/* Describe the AMD Northbridge */
			#include <northbridge/amd/pi/00730F01/acpi/northbridge.asl>

			/* Describe the AMD Fusion Controller Hub Southbridge */
			#include <southbridge/amd/pi/hudson/acpi/fch.asl>
		}

		/* Describe PCI INT[A-H] for the Southbridge */
		#include <southbridge/amd/pi/hudson/acpi/pci_int.asl>

	} /* End \_SB scope */

	/* Describe SMBUS for the Southbridge */
	#include <southbridge/amd/pi/hudson/acpi/smbus.asl>

	/* Define the General Purpose Events for the platform */
	#include "acpi/gpe.asl"
}
/* End of ASL file */
