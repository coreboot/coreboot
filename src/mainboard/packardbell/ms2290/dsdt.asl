/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
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
	0x20140108	/* OEM revision */
)
{
	#include <southbridge/intel/common/acpi/platform.asl>

	#include "acpi/platform.asl"

	/* global NVS and variables */
	#include <southbridge/intel/bd82x6x/acpi/globalnvs.asl>

	/* General Purpose Events */
	#include "acpi/gpe.asl"

	#include <cpu/intel/common/acpi/cpu.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <northbridge/intel/nehalem/acpi/nehalem.asl>
			#include <southbridge/intel/bd82x6x/acpi/pch.asl>

			#include <drivers/intel/gma/acpi/default_brightness_levels.asl>
		}
		Device (UNCR)
		{
			Name (_BBN, 0xFF)
			Name (RID, 0x00)
			Name (_HID, EisaId ("PNP0A03"))
			Name (_CRS, ResourceTemplate ()
				{
				WordBusNumber (ResourceProducer, MinFixed, MaxFixed, PosDecode,
						0x0000,	     /* Granularity */
						0x00FF,	     /* Range Minimum */
						0x00FF,	     /* Range Maximum */
						0x0000,	     /* Translation Offset */
						0x0001,	     /* Length */
						,, )
				})
			Device (SAD)
			{
				Name (_ADR, 0x01)
				Name (RID, 0x00)
				OperationRegion (SADC, PCI_Config, 0x00, 0x0100)
				Field (SADC, DWordAcc, NoLock, Preserve)
				{
					Offset (0x40),
					PAM0,   8,
					PAM1,   8,
					PAM2,   8,
					PAM3,   8,
					PAM4,   8,
					PAM5,   8,
					PAM6,   8
				}
			}
		}
	}

	/* Chipset specific sleep states */
	#include <southbridge/intel/common/acpi/sleepstates.asl>
}
