/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <variant/ec.h>

DefinitionBlock (
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x00010001	/* OEM Revision */
	)
{
	#include <acpi/dsdt_top.asl>
	#include <soc.asl>

	/* ChromeOS Embedded Controller */
	Scope (\_SB.PCI0.LPCB)
	{
		/* ACPI code for EC SuperIO functions */
		#include <ec/google/chromeec/acpi/superio.asl>
		/* ACPI code for EC functions */
		#include <ec/google/chromeec/acpi/ec.asl>
	}
}
