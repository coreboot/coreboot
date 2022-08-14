/* SPDX-License-Identifier: GPL-2.0-only */

#define ENABLE_TPM

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

	#include <southbridge/intel/common/acpi/sleepstates.asl>

	#include "acpi/mainboard.asl"
}
