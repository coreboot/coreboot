/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
DefinitionBlock (
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20220909
)
{
	#include <acpi/dsdt_top.asl>
	#include <soc.asl>

	Name(LIDS, 0)

	Scope (\_SB.PCI0)
	{
		/* PS/2 Keyboard */
		#include <drivers/pc80/pc/ps2_keyboard.asl>
	}

	/* Star Labs EC */
	#include <ec/starlabs/merlin/acpi/ec.asl>

	Scope (\_SB)
	{
		/* Suspend Methods */
		#include <ec/starlabs/merlin/acpi/suspend.asl>
	}

	#include "acpi/mainboard.asl"
}
