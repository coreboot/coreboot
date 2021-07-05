/* SPDX-License-Identifier: GPL-2.0-only */

#define EC_GPE_SCI 0x6e

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
	#include <soc/intel/common/block/acpi/acpi/globalnvs.asl>
	#include <cpu/intel/common/acpi/cpu.asl>

	Device (\_SB.PCI0)
	{
		/* Tiger Lake */
		#include <soc/intel/common/block/acpi/acpi/northbridge.asl>
		#include <soc/intel/tigerlake/acpi/southbridge.asl>
		#include <soc/intel/tigerlake/acpi/tcss.asl>

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
