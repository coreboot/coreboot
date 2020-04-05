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
	#include <soc/intel/baytrail/acpi/platform.asl>

	// global NVS and variables
	#include <soc/intel/baytrail/acpi/globalnvs.asl>

	#include <cpu/intel/common/acpi/cpu.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
			//#include <soc/intel/baytrail/acpi/northcluster.asl>
			#include <soc/intel/baytrail/acpi/southcluster.asl>
			#include <drivers/intel/gma/acpi/default_brightness_levels.asl>
		}

		/* Dynamic Platform Thermal Framework */
		#include "acpi/dptf.asl"
	}

	#include <vendorcode/google/chromeos/acpi/chromeos.asl>

	#include <southbridge/intel/common/acpi/sleepstates.asl>

	#include "acpi/mainboard.asl"
}
