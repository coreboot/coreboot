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
	#include <soc/intel/common/block/acpi/acpi/globalnvs.asl>
	#include <soc/intel/xeon_sp/gnr/acpi/gpe.asl>
	#include <southbridge/intel/common/acpi/sleepstates.asl>
	#include <commonlib/include/commonlib/console/post_codes.h>
	#include <arch/x86/acpi/post.asl>
	#include <arch/x86/acpi/debug.asl>
}
