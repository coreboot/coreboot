/* SPDX-License-Identifier: GPL-2.0-or-later */

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
	Scope (\_SB) {
		Device (PCI0)
		{
			Name (_HID, EISAID ("PNP0A08"))	/* PCIe */
		}
	}

	#include <southbridge/intel/common/acpi/sleepstates.asl>

}
