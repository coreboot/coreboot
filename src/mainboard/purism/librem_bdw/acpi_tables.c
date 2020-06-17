/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpi_gnvs.h>
#include <arch/ioapic.h>
#include <soc/acpi.h>
#include <soc/nvs.h>

void acpi_create_gnvs(struct global_nvs *gnvs)
{
	acpi_init_gnvs(gnvs);
}

unsigned long acpi_fill_madt(unsigned long current)
{
	/* Local APICs */
	current = acpi_create_madt_lapics(current);

	/* IOAPIC */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current,
				2, IO_APIC_ADDR, 0);

	return acpi_madt_irq_overrides(current);
}
