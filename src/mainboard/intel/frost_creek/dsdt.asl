/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>

DefinitionBlock (
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20240225	/* OEM revision */
)
{
	#include <acpi/dsdt_top.asl>

	#include <soc/intel/common/block/acpi/acpi/globalnvs.asl>

	#include <cpu/intel/common/acpi/cpu.asl>

	/* SNR ACPI tables */
	#include <soc/intel/snowridge/acpi/uncore.asl>

	#include <soc/intel/snowridge/acpi/southcluster.asl>
}
