/* SPDX-License-Identifier: GPL-2.0-or-later */


#include <acpi/acpi.h>
DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	0x02,		// DSDT revision: ACPI v2.0 and up
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20110725	// OEM revision
)
{
	// platform ACPI tables
	#include "acpi/platform.asl"

	// global NVS and variables
	#include <soc/intel/xeon_sp/acpi/globalnvs.asl>

	#include <cpu/intel/common/acpi/cpu.asl>

	// Xeon-SP ACPI tables
	Scope (\_SB) {
		#include <soc/intel/xeon_sp/acpi/uncore.asl>
	}
}
