/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#define THINKPAD_EC_GPE 28
#define BRIGHTNESS_UP \BRTU
#define BRIGHTNESS_DOWN \BRTD
#define ACPI_VIDEO_DEVICE \_SB.PCI0.GFX0

#include <arch/acpi.h>
DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	0x02,		// DSDT revision: ACPI v2.0 and up
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20090419	// OEM revision
)
{
	#include "acpi/platform.asl"

	// global NVS and variables
	#include <southbridge/intel/i82801gx/acpi/globalnvs.asl>
	#include <southbridge/intel/common/acpi/platform.asl>

	// General Purpose Events
	#include "acpi/gpe.asl"

	// mainboard specific devices
	#include "acpi/mainboard.asl"

	Scope (\)
	{
		// backlight control, display switching, lid
		#include "acpi/video.asl"
	}

	#include <cpu/intel/speedstep/acpi/cpu.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <northbridge/intel/i945/acpi/i945.asl>
			#include <southbridge/intel/i82801gx/acpi/ich7.asl>
		}
	}

	#include <southbridge/intel/common/acpi/sleepstates.asl>

	// Dock support code
	#include "acpi/dock.asl"
}
