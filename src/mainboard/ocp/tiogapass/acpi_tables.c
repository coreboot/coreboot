/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpi_gnvs.h>

void mainboard_fill_gnvs(struct global_nvs *gnvs)
{
	acpi_init_gnvs(gnvs);
}

void mainboard_fill_fadt(acpi_fadt_t *fadt)
{
	fadt->preferred_pm_profile = PM_ENTERPRISE_SERVER;
}
