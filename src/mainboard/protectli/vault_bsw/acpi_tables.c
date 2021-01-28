/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_gnvs.h>
#include <soc/acpi.h>
#include <soc/nvs.h>

void mainboard_fill_gnvs(struct global_nvs *gnvs)
{
	/* Enable USB ports in S3 */
	gnvs->s3u0 = 1;
	gnvs->s3u1 = 1;
}

void mainboard_fill_fadt(acpi_fadt_t *fadt)
{
	fadt->preferred_pm_profile = PM_MOBILE;
	fadt->iapc_boot_arch = ACPI_FADT_LEGACY_DEVICES;
}
