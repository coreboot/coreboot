/* SPDX-License-Identifier: GPL-2.0-or-later */

#define BRIGHTNESS_UP \_SB.PCI0.GFX0.INCB
#define BRIGHTNESS_DOWN \_SB.PCI0.GFX0.DECB
#define EC_LENOVO_H8_ME_WORKAROUND 1
#define THINKPAD_EC_GPE 17

#include <acpi/acpi.h>
DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20141018	// OEM revision
)
{
	#include <acpi/dsdt_top.asl>
	#include "acpi/platform.asl"
	#include <cpu/intel/common/acpi/cpu.asl>
	#include <southbridge/intel/common/acpi/platform.asl>
	#include <southbridge/intel/lynxpoint/acpi/globalnvs.asl>
	#include <southbridge/intel/common/acpi/sleepstates.asl>

	Device (\_SB.PCI0)
	{
		#include <northbridge/intel/haswell/acpi/hostbridge.asl>
		#include <drivers/intel/gma/acpi/default_brightness_levels.asl>
		#include <southbridge/intel/lynxpoint/acpi/pch.asl>
	}
}
