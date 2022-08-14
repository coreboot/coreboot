/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <baseboard/ec.h>
#include <baseboard/gpio.h>

DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20110725	// OEM revision
)
{
	#include <acpi/dsdt_top.asl>
	#include <soc/intel/common/block/acpi/acpi/platform.asl>
	#include <soc/intel/common/block/acpi/acpi/globalnvs.asl>
	#include <cpu/intel/common/acpi/cpu.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <soc/intel/common/block/acpi/acpi/northbridge.asl>
			#include <soc/intel/alderlake/acpi/southbridge.asl>
			#include <soc/intel/alderlake/acpi/tcss.asl>
		}
	}

	/* ChromeOS Embedded Controller */
	Scope (\_SB.PCI0.LPCB)
	{
		// ACPI code for EC SuperIO functions
		#include <ec/google/chromeec/acpi/superio.asl>
		// ACPI code for EC functions
		#include <ec/google/chromeec/acpi/ec.asl>
	}

	#include <southbridge/intel/common/acpi/sleepstates.asl>
}
