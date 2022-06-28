/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include "variant/ec.h"
#include "variant/gpio.h"

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

	// global NVS and variables
	#include <soc/intel/common/block/acpi/acpi/globalnvs.asl>

	// CPU
	#include <cpu/intel/common/acpi/cpu.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <soc/intel/common/block/acpi/acpi/northbridge.asl>
			#include <soc/intel/tigerlake/acpi/southbridge.asl>
			#include <soc/intel/tigerlake/acpi/tcss.asl>
#if CONFIG(VARIANT_HAS_MIPI_CAMERA)
			#include <soc/intel/common/block/acpi/acpi/ipu.asl>
#endif
		}
		/* Mainboard hooks */
		#include "mainboard.asl"
	}

	// ChromeOS Embedded Controller
	Scope (\_SB.PCI0.LPCB)
	{
		// ACPI code for EC SuperIO functions
		#include <ec/google/chromeec/acpi/superio.asl>
		// ACPI code for EC functions
		#include <ec/google/chromeec/acpi/ec.asl>
	}

	#include <southbridge/intel/common/acpi/sleepstates.asl>

#if CONFIG(VARIANT_HAS_MIPI_CAMERA)
	/* Camera */
	#include <variant/acpi/mipi_camera.asl>
#endif /* VARIANT_HAS_MIPI_CAMERA */
}
