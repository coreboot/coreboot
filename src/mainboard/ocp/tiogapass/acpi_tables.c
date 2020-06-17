/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_gnvs.h>
#include <intelblocks/acpi.h>
#include <soc/acpi.h>

extern const unsigned char AmlCode[];

void acpi_create_gnvs(struct global_nvs *gnvs)
{
	acpi_init_gnvs(gnvs);
}

void mainboard_fill_fadt(acpi_fadt_t *fadt)
{
	fadt->preferred_pm_profile = PM_ENTERPRISE_SERVER;
}
