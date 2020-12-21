/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_gnvs.h>
#include <arch/ioapic.h>
#include <soc/acpi.h>
#include <soc/nvs.h>
#include <string.h>

void acpi_create_gnvs(struct global_nvs *gnvs)
{
	memset(gnvs, 0, sizeof(*gnvs));

	acpi_init_gnvs(gnvs);

	/* Enable USB ports in S3 */
	gnvs->s3u0 = 1;
	gnvs->s3u1 = 1;
}

unsigned long acpi_fill_madt(unsigned long current)
{
	/* Local APICs */
	current = acpi_create_madt_lapics(current);

	/* IOAPIC */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current, 2, IO_APIC_ADDR, 0);

	current = acpi_madt_irq_overrides(current);

	return current;
}

void mainboard_fill_fadt(acpi_fadt_t *fadt)
{
	fadt->preferred_pm_profile = PM_MOBILE;
	fadt->iapc_boot_arch = ACPI_FADT_LEGACY_DEVICES;
}
