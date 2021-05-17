/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>

#if CONFIG(BOARD_ASUS_P8H61_M_LX)
#define BOARD_DSDT_REVISION 0x20171231	/* OEM revision */
#else
#define BOARD_DSDT_REVISION 0x20141018
#endif

DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	BOARD_DSDT_REVISION
)
{
	#include <acpi/dsdt_top.asl>
	#include "acpi/platform.asl"
	#include <cpu/intel/common/acpi/cpu.asl>
	#include <southbridge/intel/common/acpi/platform.asl>
	#include <southbridge/intel/bd82x6x/acpi/globalnvs.asl>
	#include <southbridge/intel/common/acpi/sleepstates.asl>

	Device (\_SB.PCI0)
	{
		#include <northbridge/intel/sandybridge/acpi/sandybridge.asl>
		#include <southbridge/intel/bd82x6x/acpi/pch.asl>
	}
}
