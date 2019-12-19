/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
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
	#include <soc/intel/apollolake/acpi/platform.asl>

	/* global NVS and variables */
	#include <soc/intel/apollolake/acpi/globalnvs.asl>

	/* CPU */
	#include <cpu/intel/common/acpi/cpu.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <soc/intel/apollolake/acpi/northbridge.asl>
			#include <soc/intel/apollolake/acpi/southbridge.asl>
			#include <soc/intel/apollolake/acpi/pch_hda.asl>
		}
	}

	/* Chipset specific sleep states */
	#include <southbridge/intel/common/acpi/sleepstates.asl>
}
