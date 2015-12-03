/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 Google Inc.
 * Copyright (C) 2014 Sage Electronic Engineering, LLC.
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

#define INCLUDE_LPE  1
#define INCLUDE_SCC  1
#define INCLUDE_EHCI 1
#define INCLUDE_XHCI 1
#define INCLUDE_LPSS 1


DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	0x02,		// DSDT revision: ACPI v2.0
	"COREv4",	// OEM id
	"COREBOOT",	// OEM table id
	0x20110725	// OEM revision
)
{
	// Some generic macros
	#include <soc/intel/fsp_baytrail/acpi/platform.asl>

	// global NVS and variables
	#include <soc/intel/fsp_baytrail/acpi/globalnvs.asl>

	#include <soc/intel/fsp_baytrail/acpi/cpu.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <soc/intel/fsp_baytrail/acpi/southcluster.asl>
		}
	}

	/* Chipset specific sleep states */
	#include <soc/intel/fsp_baytrail/acpi/sleepstates.asl>

	#include "acpi/mainboard.asl"
}
