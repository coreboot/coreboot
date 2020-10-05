/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>

DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20141018	/* OEM revision */
)
{
	#include "acpi/platform.asl"
	#include "acpi/superio.asl"
	#include <southbridge/intel/common/acpi/platform.asl>
	#include <southbridge/intel/lynxpoint/acpi/globalnvs.asl>
	#include <southbridge/intel/common/acpi/sleepstates.asl>
	#include <cpu/intel/common/acpi/cpu.asl>

	Device (\_SB.PCI0)
	{
		#include <northbridge/intel/haswell/acpi/haswell.asl>
		#include <southbridge/intel/lynxpoint/acpi/pch.asl>
		#include <drivers/intel/gma/acpi/default_brightness_levels.asl>
	}
}
