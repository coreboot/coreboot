/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>

#define SDCARD_CD 81 /* Not used */

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
	#include <acpi/platform.asl>
	#include <acpi/globalnvs.asl>
	#include <cpu/intel/common/acpi/cpu.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <acpi/southcluster.asl>
		}
	}

	#include <southbridge/intel/common/acpi/sleepstates.asl>

	#include "acpi/mainboard.asl"
}
