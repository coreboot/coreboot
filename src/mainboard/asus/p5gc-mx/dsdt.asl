/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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

#include <arch/acpi.h>
DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	0x02,		// DSDT revision: ACPI v2.0 and up
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20090419	// OEM revision
)
{
	/* #include "acpi/platform.asl" */

	// global NVS and variables
	#include <southbridge/intel/i82801gx/acpi/globalnvs.asl>
	#include <southbridge/intel/common/acpi/platform.asl>

	// General Purpose Events
	//#include "acpi/gpe.asl"

	// mainboard specific devices
	#include "acpi/mainboard.asl"

	// Thermal Zone
	//#include "acpi/thermal.asl"

	#include <cpu/intel/speedstep/acpi/cpu.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <northbridge/intel/i945/acpi/i945.asl>
			#include <southbridge/intel/i82801gx/acpi/ich7.asl>
		}
	}

	/* Chipset specific sleep states */
	#include <southbridge/intel/common/acpi/sleepstates.asl>
}
