/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20110725
)
{
	#include <acpi/dsdt_top.asl>
	#include <soc/intel/common/block/acpi/acpi/platform.asl>
	#include <soc/intel/apollolake/acpi/globalnvs.asl>
	#include <cpu/intel/common/acpi/cpu.asl>

	Device (\_SB.PCI0)
	{
		/* Apollo Lake */
		#include <soc/intel/apollolake/acpi/northbridge.asl>
		#include <soc/intel/apollolake/acpi/southbridge.asl>
		#include <soc/intel/apollolake/acpi/pch_hda.asl>

		#include <drivers/intel/gma/acpi/default_brightness_levels.asl>

		/* PS/2 Keyboard */
		#include <drivers/pc80/pc/ps2_controller.asl>
	}

	#include <southbridge/intel/common/acpi/sleepstates.asl>

	/* Star Labs EC */
	#include <ec/starlabs/merlin/acpi/ec.asl>

	Scope (\_SB)
	{
		/* HID Driver */
		#include <ec/starlabs/merlin/acpi/hid.asl>

		/* Suspend Methods */
		#include <ec/starlabs/merlin/acpi/suspend.asl>
	}

	#include "acpi/mainboard.asl"
}
