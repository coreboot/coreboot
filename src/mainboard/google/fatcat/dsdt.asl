/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <variant/ec.h>
#include <variant/gpio.h>

DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20240917
)
{
	#include <acpi/dsdt_top.asl>
	#include <soc/intel/common/block/acpi/acpi/platform.asl>

	/* global NVS and variables */
	#include <soc/intel/common/block/acpi/acpi/globalnvs.asl>

	#include <cpu/intel/common/acpi/cpu.asl>

	Device (\_SB.PCI0) {
		#include <soc/intel/common/block/acpi/acpi/northbridge.asl>
		#include <soc/intel/pantherlake/acpi/southbridge.asl>
		#include <soc/intel/pantherlake/acpi/tcss.asl>
	}

#if CONFIG(EC_GOOGLE_CHROMEEC)
	/* ChromeOS Embedded Controller */
	Scope (\_SB.PCI0.LPCB)
	{
		/* ACPI code for EC SuperIO functions */
		#include <ec/google/chromeec/acpi/superio.asl>
		/* ACPI code for EC functions */
		#include <ec/google/chromeec/acpi/ec.asl>
	}
#endif

	#include <southbridge/intel/common/acpi/sleepstates.asl>
}
