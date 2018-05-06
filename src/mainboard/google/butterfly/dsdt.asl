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

DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	0x02,		// DSDT revision: ACPI v2.0
	"COREv4",	// OEM id
	"COREBOOT",	// OEM table id
	0x20110725	// OEM revision
)
{
	#include <southbridge/intel/bd82x6x/acpi/platform.asl>

	// Some generic macros
	#include "acpi/platform.asl"
	#include "acpi/mainboard.asl"

	// Thermal handler
	#include "acpi/thermal.asl"

	// global NVS and variables
	#include <southbridge/intel/bd82x6x/acpi/globalnvs.asl>

	// General Purpose Events
	//#include "acpi/gpe.asl"

	#include <cpu/intel/model_206ax/acpi/cpu.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <northbridge/intel/sandybridge/acpi/sandybridge.asl>
			#include <southbridge/intel/bd82x6x/acpi/pch.asl>

			#include <drivers/intel/gma/acpi/default_brightness_levels.asl>
		}
	}

	#include <vendorcode/google/chromeos/acpi/chromeos.asl>

	/* Chipset specific sleep states */
	#include <southbridge/intel/bd82x6x/acpi/sleepstates.asl>
}
