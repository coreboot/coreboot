/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2012 Google Inc.
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
	0x02,		// DSDT revision: ACPI v2.0
	"COREv4",	// OEM id
	"COREBOOT",	// OEM table id
	0x20110725	// OEM revision
)
{
	// Some generic macros
	#include "acpi/platform.asl"

	// Thermal handler
	#include "acpi/thermal.asl"

	// global NVS and variables
	#include <southbridge/intel/lynxpoint/acpi/globalnvs.asl>

	// General Purpose Events
	//#include "acpi/gpe.asl"

	// CPU
	#include <cpu/intel/haswell/acpi/cpu.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <northbridge/intel/haswell/acpi/haswell.asl>
			#include <southbridge/intel/lynxpoint/acpi/pch.asl>

			#include <drivers/intel/gma/acpi/default_brightness_levels.asl>
		}
	}

	Scope (\_SB.PCI0.RP01)
	{
		Device (WLAN)
		{
			Name (_ADR, Zero)
		}
	}

	// Mainboard specific
	#include "acpi/mainboard.asl"

	// Chrome OS specific
	#include <vendorcode/google/chromeos/acpi/chromeos.asl>

	// Chipset specific sleep states
	#include <southbridge/intel/lynxpoint/acpi/sleepstates.asl>
}
