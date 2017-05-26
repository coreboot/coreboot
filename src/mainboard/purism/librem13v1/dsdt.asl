/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google Inc.
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
	0x05,		/* DSDT revision: ACPI v5.0 */
	"COREv4",	/* OEM id */
	"COREBOOT",	/* OEM table id */
	0x20160115	/* OEM revision */
)
{
	/* Some generic macros */
	#include <soc/intel/broadwell/acpi/platform.asl>

	/* Global NVS and variables */
	#include <soc/intel/broadwell/acpi/globalnvs.asl>

	/* CPU */
	#include <soc/intel/broadwell/acpi/cpu.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <soc/intel/broadwell/acpi/systemagent.asl>
			#include <soc/intel/broadwell/acpi/pch.asl>
		}
	}

	/* Chipset specific sleep states */
	#include <soc/intel/broadwell/acpi/sleepstates.asl>

	/* Mainboard specific */
	#include "acpi/mainboard.asl"
}
