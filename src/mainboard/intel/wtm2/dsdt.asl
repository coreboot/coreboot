/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#define ENABLE_TPM

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
	#include "acpi/platform.asl"

	// global NVS and variables
	#include <soc/intel/broadwell/acpi/globalnvs.asl>

	// General Purpose Events
	//#include "acpi/gpe.asl"

	// CPU
	#include <soc/intel/broadwell/acpi/cpu.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <soc/intel/broadwell/acpi/systemagent.asl>
			#include <soc/intel/broadwell/acpi/pch.asl>
		}
	}

	// Thermal handler
	#include "acpi/thermal.asl"

	// Chrome OS specific
	#include "acpi/chromeos.asl"
	#include <vendorcode/google/chromeos/acpi/chromeos.asl>

	// Chipset specific sleep states
	#include <soc/intel/broadwell/acpi/sleepstates.asl>

	// Mainboard specific
	#include "acpi/mainboard.asl"
}
