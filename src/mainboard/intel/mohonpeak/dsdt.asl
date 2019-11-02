/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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
DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	0x02,		// DSDT revision: ACPI v2.0 and up
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20110725	// OEM revision
)
{
	// Include mainboard configuration
	#include <acpi/mainboard.asl>

	// Include debug methods
	#include <arch/x86/acpi/debug.asl>

	// Some generic macros
	#include <southbridge/intel/common/acpi/platform.asl>
	#include "acpi/platform.asl"

	// global NVS and variables
	#include <southbridge/intel/fsp_rangeley/acpi/globalnvs.asl>

	#include "acpi/thermal.asl"

	#include <cpu/intel/common/acpi/cpu.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <northbridge/intel/fsp_rangeley/acpi/rangeley.asl>
			#include <southbridge/intel/fsp_rangeley/acpi/soc.asl>
		}
	}

	/* Chipset specific sleep states */
	#include <southbridge/intel/common/acpi/sleepstates.asl>
}
