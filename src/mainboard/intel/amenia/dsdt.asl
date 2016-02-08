/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
 * (Written by Lance Zhao <lijian.zhao@intel.com> for Intel Corp.)
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

DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	0x02,		// DSDT revision: ACPI v2.0
	"COREv4",	// OEM id
	"COREBOOT",	// OEM table id
	0x20110725	// OEM revision
)
{
	Scope (\_SB) {
		Device (PCI0)
		{
                #include <soc/intel/apollolake/acpi/northbridge.asl>
                #include <soc/intel/apollolake/acpi/southbridge.asl>
		}
	}
	/* Mainboard Specific devices */
	#include "acpi/mainboard.asl"

        /* Chipset specific sleep states */
        #include <soc/intel/apollolake/acpi/sleepstates.asl>

	#include "acpi/superio.asl"

}
