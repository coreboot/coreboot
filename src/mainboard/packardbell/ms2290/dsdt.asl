/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20140108	/* OEM revision */
)
{
	#include <southbridge/intel/common/acpi/platform.asl>

	#include "acpi/platform.asl"

	/* global NVS and variables */
	#include <southbridge/intel/ibexpeak/acpi/globalnvs.asl>

	/* General Purpose Events */
	#include "acpi/gpe.asl"

	#include <cpu/intel/common/acpi/cpu.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <northbridge/intel/ironlake/acpi/ironlake.asl>
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

	#include <southbridge/intel/common/acpi/sleepstates.asl>
}
