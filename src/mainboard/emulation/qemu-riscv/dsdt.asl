/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * This DSDT source is meant to be empty since riscv qemu has its own
 * method to retrieve DSDT blob via fw_cfg.
 */

#include <acpi/acpi.h>

DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20110725	// OEM revision
)
{
}
