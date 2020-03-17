/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/acpi.h>
#include <baseboard/ec.h>
#include <baseboard/gpio.h>

DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	0x02,		// DSDT revision: ACPI v2.0 and up
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20110725	// OEM revision
)
{
	#include <soc/intel/icelake/acpi/platform.asl>

	// global NVS and variables
	#include <soc/intel/common/block/acpi/acpi/globalnvs.asl>

	// CPU
	#include <cpu/intel/common/acpi/cpu.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <soc/intel/common/block/acpi/acpi/northbridge.asl>
			#include <soc/intel/icelake/acpi/southbridge.asl>
		}
	}

#if CONFIG(CHROMEOS)
	// Chrome OS specific
	#include <vendorcode/google/chromeos/acpi/chromeos.asl>
#endif

#if CONFIG(EC_GOOGLE_CHROMEEC)
	/* Chrome OS Embedded Controller */
		Scope (\_SB.PCI0.LPCB)
		{
			/* ACPI code for EC SuperIO functions */
			#include <ec/google/chromeec/acpi/superio.asl>
			/* ACPI code for EC functions */
			#include <ec/google/chromeec/acpi/ec.asl>
		}
#endif

	#include <southbridge/intel/common/acpi/sleepstates.asl>

	// Mainboard specific
	#include "acpi/mainboard.asl"

}
