/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20140108	/* OEM revision */
)
{
	#include <acpi/dsdt_top.asl>
	#include <southbridge/intel/common/acpi/platform.asl>

	#include "acpi/platform.asl"

	/* General Purpose Events */
	#include "acpi/gpe.asl"

	#include <cpu/intel/common/acpi/cpu.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <northbridge/intel/ironlake/acpi/ironlake.asl>

			/* TBD: Remove. */
			Name(\XHCI, 0)
			#include <southbridge/intel/bd82x6x/acpi/pch.asl>

			#include <drivers/intel/gma/acpi/default_brightness_levels.asl>
		}
		#include <northbridge/intel/ironlake/acpi/uncore.asl>
	}

	#include <southbridge/intel/common/acpi/sleepstates.asl>
}
