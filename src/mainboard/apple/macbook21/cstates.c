/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>

static acpi_cstate_t cst_entries[] = {
	{
		.ctype = 1,
		.latency = 1,
		.power = 1000,
		.resource = {
			.space_id = ACPI_ADDRESS_SPACE_FIXED,
			.bit_width = ACPI_FFIXEDHW_VENDOR_INTEL,
			.bit_offset = ACPI_FFIXEDHW_CLASS_MWAIT,
			.access_size = ACPI_ACCESS_SIZE_UNDEFINED,
			.addrl = 0,
			.addrh = 0,
		}
	},
	{
		.ctype = 2,
		.latency = 1,
		.power = 500,
		.resource = {
			.space_id = ACPI_ADDRESS_SPACE_FIXED,
			.bit_width = ACPI_FFIXEDHW_VENDOR_INTEL,
			.bit_offset = ACPI_FFIXEDHW_CLASS_MWAIT,
			.access_size = ACPI_ACCESS_SIZE_UNDEFINED,
			.addrl = 0x10,
			.addrh = 0,
		}
	},
};

int get_cst_entries(acpi_cstate_t **entries)
{
	*entries = cst_entries;
	return ARRAY_SIZE(cst_entries);
}
