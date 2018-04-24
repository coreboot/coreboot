/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Intel Corp.
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

#include <variant/ec.h>
#include <variant/gpio.h>

DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	0x05,		// DSDT revision: ACPI v5.0
	"COREv4",	// OEM id
	"COREBOOT",	// OEM table id
	0x20110725	// OEM revision
)
{
	/* global NVS and variables */
	#include <soc/intel/apollolake/acpi/globalnvs.asl>

	/* CPU */
	#include <soc/intel/apollolake/acpi/cpu.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <soc/intel/apollolake/acpi/northbridge.asl>
			#include <soc/intel/apollolake/acpi/southbridge.asl>
			#include <soc/intel/apollolake/acpi/pch_hda.asl>
		}
	}

	/* Chrome OS specific */
	#include <vendorcode/google/chromeos/acpi/chromeos.asl>

	/* Chipset specific sleep states */
	#include <soc/intel/apollolake/acpi/sleepstates.asl>

	/* Chrome OS Embedded Controller */
	Scope (\_SB.PCI0.LPCB)
	{
		/* ACPI code for EC SuperIO functions */
		#include <ec/google/chromeec/acpi/superio.asl>
		/* ACPI code for EC functions */
		#include <ec/google/chromeec/acpi/ec.asl>
	}

	/* Dynamic Platform Thermal Framework */
	Scope (\_SB)
	{
		/* Per board variant specific definitions. */
		#include <variant/acpi/dptf.asl>
		/* Include soc specific DPTF changes */
		#include <soc/intel/apollolake/acpi/dptf.asl>
		/* Include common dptf ASL files */
		#include <soc/intel/common/acpi/dptf/dptf.asl>
	}
}
