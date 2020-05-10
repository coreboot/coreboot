/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <soc/acpi.h>

void motherboard_fill_fadt(acpi_fadt_t *fadt)
{
	fadt->reserved = 0;
	fadt->preferred_pm_profile = PM_ENTERPRISE_SERVER;
}
