/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>

static const acpi_cstate_t cst_entries[] = {};

int get_cst_entries(const acpi_cstate_t **entries)
{
	*entries = cst_entries;
	return ARRAY_SIZE(cst_entries);
}
