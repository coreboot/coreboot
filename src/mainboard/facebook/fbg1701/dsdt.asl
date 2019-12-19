/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 Google Inc.
 * Copyright (C) 2015-2018 Intel Corp.
 * Copyright (C) 2018 Eltan B.V.
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

#define SDCARD_CD 81 /* Not used */

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
		}
	}

	/* Chipset specific sleep states */
	#include "acpi/sleepstates.asl"
	#include "acpi/mainboard.asl"
}
