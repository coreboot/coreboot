/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#include <arch/acpi.h>
DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	0x02,		// DSDT revision: ACPI v2.0 and up
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20110725	// OEM revision
)
{
	// platform ACPI tables
	#include "acpi/platform.asl"

	// global NVS and variables
	#include <soc/intel/xeon_sp/skx/acpi/globalnvs.asl>

	#include <cpu/intel/common/acpi/cpu.asl>

	// Xeon-SP ACPI tables
	Scope (\_SB) {
		#include <soc/intel/xeon_sp/skx/acpi/uncore.asl>
	}
}
