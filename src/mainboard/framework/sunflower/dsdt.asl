/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <mainboard/framework/common/ec.h>

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
	#include <soc/intel/common/block/acpi/acpi/platform.asl>
	#include <soc/intel/common/block/acpi/acpi/globalnvs.asl>
	#include <cpu/intel/common/acpi/cpu.asl>

	Device (\_SB.PCI0) {
		#include <soc/intel/common/block/acpi/acpi/northbridge.asl>
		#include <soc/intel/alderlake/acpi/southbridge.asl>
		#include <soc/intel/alderlake/acpi/tcss.asl>
		#include <drivers/intel/gma/acpi/default_brightness_levels.asl>
	}

	#include "tablet.asl"

	#include <southbridge/intel/common/acpi/sleepstates.asl>

	#include <mainboard/framework/common/ec.asl>
}
