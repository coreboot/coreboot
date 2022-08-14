/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20110725
)
{
	#include <acpi/dsdt_top.asl>
	#include <cpu/intel/common/acpi/cpu.asl>
	#include <soc/intel/common/block/acpi/acpi/platform.asl>
	#include <soc/intel/common/block/acpi/acpi/globalnvs.asl>
	#include <southbridge/intel/common/acpi/sleepstates.asl>

	Device (\_SB.PCI0)
	{
		#include <soc/intel/skylake/acpi/systemagent.asl>
		#include <soc/intel/skylake/acpi/pch.asl>
		#include <drivers/intel/gma/acpi/default_brightness_levels.asl>
	}

	#include "acpi/mainboard.asl"
}
