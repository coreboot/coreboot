/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
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
	#include "acpi/platform.asl"

	Name(_S0, Package() { 0x00, 0x00, 0x00, 0x00 })
	Name(_S5, Package() { 0x07, 0x00, 0x00, 0x00 })

	Scope (\_SB)
	{
		Device (PCI0)
		{
			#include <acpi/southcluster.asl>
			#include <acpi/pcie1.asl>
		}

		#include <acpi/uncore.asl>
	}

	#include "acpi/mainboard.asl"
}
