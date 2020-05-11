/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>

DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	0x02,		/* DSDT revision: ACPI 2.0 and up */
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20141018	/* OEM revision */
)
{
	#include <acpi/dsdt_top.asl>
	#include "acpi/platform.asl"
	#include <southbridge/intel/common/acpi/platform.asl>
	#include <southbridge/intel/lynxpoint/acpi/globalnvs.asl>
	#include <southbridge/intel/common/acpi/sleepstates.asl>
	#include <cpu/intel/common/acpi/cpu.asl>

	Device (\_SB.PCI0)
	{
		#include <northbridge/intel/haswell/acpi/hostbridge.asl>
		#include <southbridge/intel/lynxpoint/acpi/pch.asl>
	}
}
