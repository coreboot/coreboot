/* SPDX-License-Identifier: GPL-2.0-or-later */

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
	#include <soc/intel/common/block/acpi/acpi/globalnvs.asl>
	#include <soc/intel/common/block/acpi/acpi/platform.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <soc/intel/common/block/acpi/acpi/northbridge.asl>
			#include <soc/intel/meteorlake/acpi/southbridge.asl>
		}
	}

	#include <southbridge/intel/common/acpi/sleepstates.asl>
}
