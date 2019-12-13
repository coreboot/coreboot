/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
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
#include <variant/ec.h>
#include <variant/gpio.h>

DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	0x02,		/* DSDT revision: ACPI v2.0 and up */
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20110725	/* OEM revision */
)
{
	#include <soc/intel/cannonlake/acpi/platform.asl>

	/* global NVS and variables */
	#include <soc/intel/common/block/acpi/acpi/globalnvs.asl>

	/* CPU */
	#include <cpu/intel/common/acpi/cpu.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <soc/intel/cannonlake/acpi/northbridge.asl>
			#include <soc/intel/cannonlake/acpi/southbridge.asl>
		}
	}

#if CONFIG(CHROMEOS)
	/* Chrome OS specific */
	#include <vendorcode/google/chromeos/acpi/chromeos.asl>
#endif

	/* Chipset specific sleep states */
	#include <southbridge/intel/common/acpi/sleepstates.asl>

	/* Low power idle table */
	#include <soc/intel/cannonlake/acpi/lpit.asl>

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
		#include <soc/intel/cannonlake/acpi/dptf.asl>
		/* Include common dptf ASL files */
		#include <soc/intel/common/acpi/dptf/dptf.asl>
	}
}
