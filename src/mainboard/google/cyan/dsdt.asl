/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 Google Inc.
 * Copyright (C) 2015 Intel Corp.
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
	0x02,		/* DSDT revision: ACPI v2.0 and up */
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20110725	/* OEM revision */
)
{
	#include <acpi/platform.asl>

	/* global NVS and variables */
	#include <acpi/globalnvs.asl>

	#include <cpu/intel/common/acpi/cpu.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <acpi/southcluster.asl>
#if CONFIG(BOARD_GOOGLE_TERRA)
			#include <variant/acpi/cpu.asl>
#else
			#include <acpi/dptf/cpu.asl>
#endif
			#include <acpi/lpe.asl>
		}

		/* Dynamic Platform Thermal Framework */
		#include "acpi/dptf.asl"
	}
	Scope (\_SB.PCI0)
	{
		Device (RP03)
		{
			Name (_ADR, 0x001C0002)  // _ADR: Address
			OperationRegion(RPXX, PCI_Config, 0x00, 0x10)

			/* Wifi Device */
			#include <soc/intel/common/acpi/wifi.asl>
		}
	}
	#include <vendorcode/google/chromeos/acpi/chromeos.asl>

	/* Chipset specific sleep states */
	#include <southbridge/intel/common/acpi/sleepstates.asl>

	#include "acpi/mainboard.asl"
}
