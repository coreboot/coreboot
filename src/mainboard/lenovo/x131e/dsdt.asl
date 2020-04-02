/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#define THINKPAD_EC_GPE 22
#define BRIGHTNESS_UP \_SB.PCI0.GFX0.INCB
#define BRIGHTNESS_DOWN \_SB.PCI0.GFX0.DECB
#define ACPI_VIDEO_DEVICE \_SB.PCI0.GFX0
#define EC_LENOVO_H8_ME_WORKAROUND 1

#include <arch/acpi.h>
DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	0x02,		// DSDT revision: ACPI v2.0 and up
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20141018	// OEM revision
)
{
	#include "acpi/platform.asl"
	#include <cpu/intel/common/acpi/cpu.asl>
	#include <southbridge/intel/common/acpi/platform.asl>

	// global NVS and variables
	#include <southbridge/intel/bd82x6x/acpi/globalnvs.asl>

	#include <southbridge/intel/common/acpi/sleepstates.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <northbridge/intel/sandybridge/acpi/sandybridge.asl>
			#include <drivers/intel/gma/acpi/default_brightness_levels.asl>
			#include <southbridge/intel/bd82x6x/acpi/pch.asl>
		}
	}
}
