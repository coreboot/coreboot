/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <arch/acpigen.h>

static acpi_cstate_t cst_entries[] = {};

int get_cst_entries(acpi_cstate_t **entries)
{
	*entries = cst_entries;
	return ARRAY_SIZE(cst_entries);
}
