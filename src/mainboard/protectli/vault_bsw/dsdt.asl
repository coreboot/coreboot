/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x00010001	/* OEM revision */
)
{
	#include <acpi/dsdt_top.asl>
	#include "onboard.h"

	#include <acpi/platform.asl>
	#include <acpi/globalnvs.asl>
	#include <cpu/intel/common/acpi/cpu.asl>

	Device (\_SB.PCI0)
	{
		#include <acpi/southcluster.asl>

		Device (RP03)
		{
			Name (_ADR, 0x001C0002)  // _ADR: Address
			OperationRegion(RPXX, PCI_Config, 0x00, 0x10)
		}
	}

	#include <southbridge/intel/common/acpi/sleepstates.asl>

	#include "acpi/mainboard.asl"
}
