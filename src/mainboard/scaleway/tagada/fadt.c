/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <arch/acpi.h>

#include <soc/acpi.h>
#include <soc/soc_util.h>

void motherboard_fill_fadt(acpi_fadt_t *fadt)
{
	fadt->reserved = 0;
	fadt->preferred_pm_profile = PM_ENTERPRISE_SERVER;
}
