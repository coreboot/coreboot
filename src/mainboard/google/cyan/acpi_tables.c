/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <arch/acpi.h>
#include <arch/ioapic.h>
#include <soc/acpi.h>
#include <soc/nvs.h>

void acpi_create_gnvs(global_nvs_t *gnvs)
{
	acpi_init_gnvs(gnvs);

	/* Enable USB ports in S3 */
	gnvs->s3u0 = 1;
	gnvs->s3u1 = 1;

	/* Disable USB ports in S5 */
	gnvs->s5u0 = 0;
	gnvs->s5u1 = 0;

	/* Enable DPTF */
	gnvs->dpte = 1;

	/* Disable PMIC I2C port for ACPI for all boards except cyan */
	if (!CONFIG(BOARD_GOOGLE_CYAN))
		gnvs->dev.lpss_en[LPSS_NVS_I2C2] = 0;
}

unsigned long acpi_fill_madt(unsigned long current)
{
	/* Local APICs */
	current = acpi_create_madt_lapics(current);

	/* IOAPIC */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current,
				2, IO_APIC_ADDR, 0);

	current = acpi_madt_irq_overrides(current);

	return current;
}
