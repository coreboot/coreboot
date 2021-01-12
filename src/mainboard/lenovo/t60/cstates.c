/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <southbridge/intel/i82801gx/i82801gx.h>

static acpi_cstate_t cst_entries[] = {
	{ 1,  1, 1000, { 0x7f, 1, 2, 0, 1, 0 } },
	{ 2,  1,  500, { 0x01, 8, 0, 0, DEFAULT_PMBASE + LV2, 0 } },
	{ 3, 17,  250, { 0x01, 8, 0, 0, DEFAULT_PMBASE + LV3, 0 } },
};

int get_cst_entries(acpi_cstate_t **entries)
{
	*entries = cst_entries;
	return ARRAY_SIZE(cst_entries);
}
