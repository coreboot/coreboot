/* SPDX-License-Identifier: GPL-2.0-only */

#define THINKPAD_EC_GPE 0x12
#define BRIGHTNESS_UP \_SB.PCI0.GFX0.INCB
#define BRIGHTNESS_DOWN \_SB.PCI0.GFX0.DECB

#include "smi.h"

#include <acpi/acpi.h>
DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20090419	// OEM revision
)
{
	#include <acpi/dsdt_top.asl>
	#include "acpi/platform.asl"

	// global NVS and variables
	#include <southbridge/intel/i82801hx/acpi/globalnvs.asl>
	#include <southbridge/intel/common/acpi/platform.asl>

	// General Purpose Events
	#include "acpi/gpe.asl"

	#include <cpu/intel/speedstep/acpi/cpu.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
			#include <northbridge/intel/gm965/acpi/gm965.asl>
			#include <southbridge/intel/i82801hx/acpi/ich8.asl>

			#include <drivers/intel/gma/acpi/default_brightness_levels.asl>
		}
	}

	#include <southbridge/intel/common/acpi/sleepstates.asl>

	// Dock support code
	#include "acpi/dock.asl"

	#include <ec/lenovo/h8/acpi/thinklight.asl>
}
