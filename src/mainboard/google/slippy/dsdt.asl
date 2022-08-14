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
	#include "acpi/platform.asl"

	// Thermal handler
	#include "acpi/thermal.asl"

	// global NVS and variables
	#include <southbridge/intel/lynxpoint/acpi/globalnvs.asl>

	// CPU
	#include <cpu/intel/common/acpi/cpu.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <northbridge/intel/haswell/acpi/hostbridge.asl>
			#include <southbridge/intel/lynxpoint/acpi/pch.asl>

			#include <drivers/intel/gma/acpi/default_brightness_levels.asl>
		}
	}

	Scope (\_SB.PCI0.RP01)
	{
		Device (WLAN)
		{
			Name (_ADR, Zero)
		}
	}

	// Mainboard specific
	#include "acpi/mainboard.asl"

	#include <southbridge/intel/common/acpi/sleepstates.asl>
}
