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

#define THINKPAD_EC_GPE 17
#define BRIGHTNESS_UP \_SB.PCI0.GFX0.INCB
#define BRIGHTNESS_DOWN \_SB.PCI0.GFX0.DECB
#define ACPI_VIDEO_DEVICE \_SB.PCI0.GFX0
#define EC_LENOVO_H8_ME_WORKAROUND 1

DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	0x03,		/* DSDT revision: ACPI v3.0 */
	"COREv4",	/* OEM id */
	"COREBOOT",     /* OEM table id */
	0x20130325	/* OEM revision */
)
{
	/* Some generic macros */
	#include "acpi/platform.asl"

	/* global NVS and variables */
	#include <southbridge/intel/bd82x6x/acpi/globalnvs.asl>

	/* General Purpose Events */
	#include "acpi/gpe.asl"

	#include <cpu/intel/model_206ax/acpi/cpu.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <northbridge/intel/nehalem/acpi/nehalem.asl>
			#include <southbridge/intel/bd82x6x/acpi/pch.asl>
			#include <southbridge/intel/bd82x6x/acpi/default_irq_route.asl>

			#include <drivers/intel/gma/acpi/default_brightness_levels.asl>
		}
		Device (UNCR)
		{
			Name (_BBN, 0xFF)
			Name (_ADR, 0x00)
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

/*
 * LPC Trusted Platform Module
 */
Scope (\_SB.PCI0.LPCB)
{
	#include <drivers/pc80/tpm/acpi/tpm.asl>
}

	/* Chipset specific sleep states */
	#include <southbridge/intel/i82801gx/acpi/sleepstates.asl>

	/* Dock support code */
	#include "acpi/dock.asl"
}
