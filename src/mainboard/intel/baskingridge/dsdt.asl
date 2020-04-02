/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#define ENABLE_TPM

#include <arch/acpi.h>
DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	0x02,		// DSDT revision: ACPI v2.0 and up
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20110725	// OEM revision
)
{
	#include "acpi/platform.asl"

	// global NVS and variables
	#include <southbridge/intel/lynxpoint/acpi/globalnvs.asl>

	#include "acpi/thermal.asl"

	#include <cpu/intel/common/acpi/cpu.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <northbridge/intel/haswell/acpi/haswell.asl>
			#include <southbridge/intel/lynxpoint/acpi/pch.asl>

			#include <drivers/intel/gma/acpi/default_brightness_levels.asl>
		}
	}

	#include <vendorcode/google/chromeos/acpi/chromeos.asl>

	#include <southbridge/intel/common/acpi/sleepstates.asl>
}
