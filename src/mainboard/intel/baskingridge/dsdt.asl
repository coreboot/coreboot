/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright 2011 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#define ENABLE_TPM

DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	0x02,		// DSDT revision: ACPI v2.0
	"COREv4",	// OEM id
	"COREBOOT",     // OEM table id
	0x20110725	// OEM revision
)
{
	// Some generic macros
	#include "acpi/platform.asl"

	// global NVS and variables
	#include <southbridge/intel/lynxpoint/acpi/globalnvs.asl>

	// General Purpose Events
	//#include "acpi/gpe.asl"

	#include "acpi/thermal.asl"

	#include "../../../cpu/intel/haswell/acpi/cpu.asl"

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <northbridge/intel/haswell/acpi/haswell.asl>
			#include <southbridge/intel/lynxpoint/acpi/pch.asl>
		}
	}

	#include "acpi/chromeos.asl"
	#include <vendorcode/google/chromeos/acpi/chromeos.asl>

	/* Chipset specific sleep states */
	#include <southbridge/intel/lynxpoint/acpi/sleepstates.asl>
}
