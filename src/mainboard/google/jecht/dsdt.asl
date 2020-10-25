/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20110725	// OEM revision
)
{
	#include <southbridge/intel/common/acpi/platform.asl>
	#include "acpi/platform.asl"

	// Thermal handler
	#include <variant/acpi/thermal.asl>

	// global NVS and variables
	#include <soc/intel/broadwell/pch/acpi/globalnvs.asl>

	// CPU
	#include <cpu/intel/common/acpi/cpu.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <soc/intel/broadwell/acpi/hostbridge.asl>
			#include <soc/intel/broadwell/pch/acpi/pch.asl>
		}
	}

	// Chrome OS specific
	#include <vendorcode/google/chromeos/acpi/chromeos.asl>

	#include <southbridge/intel/common/acpi/sleepstates.asl>

	// Mainboard specific
	#include "acpi/mainboard.asl"
}
