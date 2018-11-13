/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Intel Corporation
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

DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	0x02,		// DSDT revision: ACPI v2.0 and up
	"COREv4",	// OEM id
	"COREBOOT",	// OEM table id
	0x20110725	// OEM revision
)
{
	// Some generic macros
	#include <soc/intel/icelake/acpi/platform.asl>

	// global NVS and variables
	#include <soc/intel/icelake/acpi/globalnvs.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <soc/intel/icelake/acpi/northbridge.asl>
			#include <soc/intel/icelake/acpi/southbridge.asl>
		}
	}

#if IS_ENABLED(CONFIG_CHROMEOS)
	// Chrome OS specific
	#include <vendorcode/google/chromeos/acpi/chromeos.asl>
#endif

	// Chipset specific sleep states
	#include <soc/intel/icelake/acpi/sleepstates.asl>

}
