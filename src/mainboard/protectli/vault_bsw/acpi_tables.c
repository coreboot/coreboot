/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>

void mainboard_fill_fadt(acpi_fadt_t *fadt)
{
	fadt->preferred_pm_profile = PM_MOBILE;
	fadt->iapc_boot_arch = ACPI_FADT_LEGACY_DEVICES;
}
