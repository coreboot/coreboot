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
	#include <soc/intel/apollolake/acpi/globalnvs.asl>
	#include <cpu/intel/common/acpi/cpu.asl>
	#include <southbridge/intel/common/acpi/sleepstates.asl>

	Scope (\_SB)
	{
		Device(PCI0)
		{
			#include <soc/intel/apollolake/acpi/northbridge.asl>
			#include <soc/intel/apollolake/acpi/southbridge.asl>
		}
		#include "acpi/dptf.asl"
		#include <soc/intel/apollolake/acpi/dptf.asl>
		#include <soc/intel/common/acpi/dptf/dptf.asl>
	}
}
