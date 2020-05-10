/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	0x02,		/* DSDT revision: ACPI v2.0 and up */
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20110725	/* OEM revision */
)
{
	Scope (\_SB) {
		Device (PCI0)
		{
			Name (_HID, EISAID ("PNP0A08"))	/* PCIe */
		}
	}

	#include <southbridge/intel/common/acpi/sleepstates.asl>

}
