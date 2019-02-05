/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <southbridge/intel/i82801jx/i82801jx.h>

#include <arch/acpi.h>
DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	0x02,		// DSDT revision: ACPI v2.0 and up
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x00000001	// OEM revision
)
{
	// global NVS and variables
	#include <southbridge/intel/common/acpi/platform.asl>
	#include <southbridge/intel/i82801jx/acpi/globalnvs.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <northbridge/intel/x4x/acpi/x4x.asl>
			#include <southbridge/intel/i82801jx/acpi/ich10.asl>
			#include <drivers/intel/gma/acpi/default_brightness_levels.asl>
		}
	}

	/* Chipset specific sleep states */
	#include <southbridge/intel/common/acpi/sleepstates.asl>
}
