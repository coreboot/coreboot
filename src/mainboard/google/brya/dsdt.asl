/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <variant/ec.h>

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
	#include <soc/intel/common/acpi/platform.asl>

	/* global NVS and variables */
	#include <soc/intel/common/block/acpi/acpi/globalnvs.asl>

	/* CPU */
	#include <cpu/intel/common/acpi/cpu.asl>

	Scope (\_SB) {
#if CONFIG(HAVE_WWAN_POWER_SEQUENCE)
		#include "wwan_power.asl"
#endif
		Device (PCI0)
		{
			#include <soc/intel/common/block/acpi/acpi/northbridge.asl>
			#include <soc/intel/alderlake/acpi/southbridge.asl>
			#include <soc/intel/alderlake/acpi/tcss.asl>
		}
	}

	/* Chipset specific sleep states */
	#include <southbridge/intel/common/acpi/sleepstates.asl>

	/* Chrome OS Embedded Controller */
	Scope (\_SB.PCI0.LPCB)
	{
		/* ACPI code for EC SuperIO functions */
		#include <ec/google/chromeec/acpi/superio.asl>
		/* ACPI code for EC functions */
		#include <ec/google/chromeec/acpi/ec.asl>
        }
}
