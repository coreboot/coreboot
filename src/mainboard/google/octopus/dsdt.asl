/* SPDX-License-Identifier: GPL-2.0-only */

#include <variant/ec.h>
#include <variant/gpio.h>

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
	#include <acpi/dsdt_top.asl>
	/* global NVS and variables */
	#include <soc/intel/apollolake/acpi/globalnvs.asl>

	/* CPU */
	#include <cpu/intel/common/acpi/cpu.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <soc/intel/apollolake/acpi/northbridge.asl>
			#include <soc/intel/apollolake/acpi/southbridge.asl>
			#include <soc/intel/apollolake/acpi/pch_hda.asl>
			#include <drivers/intel/gma/acpi/default_brightness_levels.asl>
		}
	}

	#include <southbridge/intel/common/acpi/sleepstates.asl>

	/* ChromeOS Embedded Controller */
	Scope (\_SB.PCI0.LPCB)
	{
		/* ACPI code for EC SuperIO functions */
		#include <ec/google/chromeec/acpi/superio.asl>
		/* ACPI code for EC functions */
		#include <ec/google/chromeec/acpi/ec.asl>
	}

	/* Dynamic Platform Thermal Framework */
	Scope (\_SB)
	{
		/* Per board variant specific definitions. */
		#include <variant/acpi/dptf.asl>
		/* Include soc specific DPTF changes */
		#include <soc/intel/apollolake/acpi/dptf.asl>
		/* Include common dptf ASL files */
		#include <soc/intel/common/acpi/dptf/dptf.asl>
	}
}
